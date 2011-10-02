#include "stdafx.h"
#include "gfx_func.h"
#include "debug.h"
#include "thread/thread.h"
#include "network/network_type.h"
#include "network/network_func.h"
#include "settings_type.h"
#include "string_func.h"
#include "irc_interface.hpp"
#include "sai/sai.hpp"

IRC IRCInterface::_conn;
ThreadObject *IRCInterface::_connection_thread;
ThreadObject *IRCInterface::_producer_thread;

irc_in_queue_item IRCInterface::_in_queue[IRC_BUFFER_SIZE];
int IRCInterface::_in_queue_reader;
int IRCInterface::_in_queue_writer;
ThreadMutex *_in_queue_mutex = ThreadMutex::New();

irc_out_queue_item IRCInterface::_out_queue[IRC_BUFFER_SIZE];
int IRCInterface::_out_queue_reader;
int IRCInterface::_out_queue_writer;
ThreadMutex *_out_queue_mutex = ThreadMutex::New();

bool IRCInterface::onchannel;

/* static */ bool IRCInterface::InQueueEmpty()
{
	return (_in_queue_reader == _in_queue_writer);
}

/* static */ void IRCInterface::InQueueWrite(const irc_in_queue_item *item)
{
	_in_queue_mutex->BeginCritical();

	// write to current writer
	memcpy(&_in_queue[_in_queue_writer], item, sizeof(irc_in_queue_item));

	// move writer
	_in_queue_writer++;
	if (_in_queue_writer == IRC_BUFFER_SIZE - 1)
		_in_queue_writer = 0;

	// do we have full buffer? move reader...
	if (_in_queue_writer == _in_queue_reader)
	{
		_in_queue_reader++;
		if (_in_queue_reader == IRC_BUFFER_SIZE - 1)
			_in_queue_reader = 0;
	}
	_in_queue_mutex->EndCritical();
}

/* static */ bool IRCInterface::InQueueRead(irc_in_queue_item *item)
{
	if (InQueueEmpty())
		return false;

	_in_queue_mutex->BeginCritical();

	memcpy(item, &_in_queue[_in_queue_reader], sizeof(irc_in_queue_item));

	_in_queue_reader++;
	if (_in_queue_reader == IRC_BUFFER_SIZE - 1)
		_in_queue_reader = 0;

	_in_queue_mutex->EndCritical();

	return true;
}


/* static */ bool IRCInterface::OutQueueEmpty()
{
	return (_out_queue_reader == _out_queue_writer);
}

/* static */ void IRCInterface::OutQueueWrite(const irc_out_queue_item *item)
{
	_out_queue_mutex->BeginCritical();

	// write to current writer
	memcpy(&_out_queue[_out_queue_writer], item, sizeof(irc_out_queue_item));

	// move writer
	_out_queue_writer++;
	if (_out_queue_writer == IRC_BUFFER_SIZE - 1)
		_out_queue_writer = 0;

	// do we have full buffer? move reader...
	if (_out_queue_writer == _out_queue_reader)
	{
		_out_queue_reader++;
		if (_out_queue_reader == IRC_BUFFER_SIZE - 1)
			_out_queue_reader = 0;
	}

	_out_queue_mutex->EndCritical();
}

/* static */ bool IRCInterface::OutQueueRead(irc_out_queue_item *item)
{
	if (OutQueueEmpty())
		return false;

	_out_queue_mutex->BeginCritical();

	memcpy(item, &_out_queue[_out_queue_reader], sizeof(irc_out_queue_item));

	_out_queue_reader++;
	if (_out_queue_reader == IRC_BUFFER_SIZE - 1)
		_out_queue_reader = 0;

	_out_queue_mutex->EndCritical();

	return true;
}

/* static */ void IRCInterface::ProcessQueuedMessage()
{
	irc_out_queue_item queue_item;
	if (IRCInterface::OutQueueRead(&queue_item))
	{
		if (!StrEmpty(queue_item.msg) && queue_item.msg[0] != '\1')
		{
			if (queue_item.msg[0] == '!')
			{
				char *cmd, *args;

				// invoke command callback
				if (!SAI::CallbackExists("OnIrcCommand"))
					return;

				char *space_pos;
				if ((space_pos = strchr(queue_item.msg, ' ')) != NULL)
				{
					// command +arg
					*space_pos = '\0';
					cmd = &queue_item.msg[1];
					args = (space_pos+1);				
				}
				else
				{
					cmd = &queue_item.msg[1];
					args = "";
				}
				SAI::InvokeCallback("OnIrcCommand", "isss", queue_item.is_public, queue_item.nick, cmd, args);
			} 
			else if (!queue_item.is_public)
			{
				// reformat
				char buf[MAX_IRC_MESSAGE_LENGTH];
				seprintf(buf, lastof(buf), "[irc] <%s> %s", queue_item.nick, queue_item.msg);
				NetworkServerSendChat(NETWORK_ACTION_CHAT, DESTTYPE_BROADCAST, NULL, buf, CLIENT_ID_SERVER, NULL);

				// bump back to irc server for others to know...
				IRCInterface::SendMsg(buf);
			}
		}
	}
}

/* static */ void IRCInterface::Initialize()
{
	ThreadObject *thread;

	// only start when irc_host is set
	if (StrEmpty(_settings_client.network.irc_host))
		return;

	// start irc server connection thread...
	if (ThreadObject::New(IRCInterface::IrcServerConnectionThread, NULL, &thread))
	{
		_connection_thread = thread;
	}

	// start irc server producer thread (to avoid send blocking)
	if (ThreadObject::New(IRCInterface::IrcServerProducerThread, NULL, &thread))
	{
		_producer_thread = thread;
	}
}

/* static */ void IRCInterface::Terminate()
{
	_conn.disconnect();
	// terminate thread
	//if (_thread)
	//	_thread->Exit();
}

/* static */ void IRCInterface::SendRaw(const char *fmt, ...)
{
	va_list argp;

	va_start(argp, fmt);
	IRCInterface::SendRaw(fmt, argp);
	va_end(argp);
}

/* static */ void IRCInterface::SendRaw(const char * fmt, va_list argp)
{
	irc_in_queue_item queue_item;

	vsnprintf(queue_item.msg, sizeof(queue_item.msg), fmt, argp); 
	IRCInterface::InQueueWrite(&queue_item);
}


/* static */ void IRCInterface::SendMsg(const char *msg)
{
	IRCInterface::SendMsg(_settings_client.network.irc_channel, (char *)msg);
}

/* static */ void IRCInterface::SendMsg(const char* destination, const char *msg)
{
	IRCInterface::SendRaw("PRIVMSG %s :%s\r\n", destination, (char *)msg);
}

/* static */ void IRCInterface::SendMsgFmt(const char *fmt, ...)
{
	va_list argp;
	char msg[MAX_IRC_MESSAGE_LENGTH];

	va_start(argp, fmt);
	vsnprintf(msg, sizeof(msg), fmt, argp);
	va_end(argp);

	IRCInterface::SendMsg(msg);
}

/* static */ void IRCInterface::SendNotice(const char *msg)
{
	IRCInterface::SendNotice(_settings_client.network.irc_channel, (char *)msg);
}

/* static */ void IRCInterface::SendNotice(const char* destination, const char *msg)
{
	IRCInterface::SendRaw("NOTICE %s :%s\r\n", destination, (char *)msg);
}

int end_of_motd(char* params, irc_reply_data* hostd, void* conn) /* our callback function */
{
	IRCInterface::JoinChannel();
	return 0;
}

int not_on_channel(char* params, irc_reply_data* hostd, void* conn) /* our callback function */
{
	IRCInterface::OnChannelLost();
	return 0;
}

int onevent_privmsg(char* params, irc_reply_data* hostd, void* conn) /* our callback function */
{
	IRC* irc_conn=(IRC*)conn;

	irc_out_queue_item out_item;

	out_item.is_public = strcmp(_settings_client.network.irc_nick, hostd->target) != 0;
	strncpy(out_item.msg, &params[1], MAX_IRC_MESSAGE_LENGTH);
	strncpy(out_item.nick, hostd->nick, MAX_IRC_NICK_LENGTH);

	IRCInterface::OutQueueWrite(&out_item);

	return 0;
}

void IRCInterface::OpSelf()
{
	if (!StrEmpty(_settings_client.network.irc_oper_user) && !StrEmpty(_settings_client.network.irc_oper_pass))
	{
		DEBUG(net, 2, "[irc] logging as operator");
		_conn.socksend("OPER %s %s\r\n", _settings_client.network.irc_oper_user, _settings_client.network.irc_oper_pass);
	}
}

void IRCInterface::JoinChannel()
{
	IRCInterface::OpSelf();

	DEBUG(net, 2, "[irc] joining channel %s", _settings_client.network.irc_channel);
	_conn.join(_settings_client.network.irc_channel);
	_conn.socksend("MODE %s +a %s\r\n", _settings_client.network.irc_channel, _settings_client.network.irc_nick);

	onchannel = true;
}

void IRCInterface::OnChannelLost()
{
	DEBUG(net, 2, "[irc] channel lost or cannot join");
	onchannel = false;
}


void IRCInterface::IrcServerConnectionThread(void *context)
{
	_conn.hook_irc_command("376", &end_of_motd);	// end of motd
	_conn.hook_irc_command("422", &end_of_motd);	// no mtd at all
	_conn.hook_irc_command("PRIVMSG", &onevent_privmsg);	// privmsg

	_conn.hook_irc_command("404", &not_on_channel);	// not on channel
	_conn.hook_irc_command("473", &not_on_channel);	// not on channel (invite only)
	_conn.hook_irc_command("475", &not_on_channel);	// not on channel

	while (1)
	{		
		// connecting to irc server
		DEBUG(net, 2, "[irc] connecting %s:%d", _settings_client.network.irc_host, _settings_client.network.irc_port);
		onchannel = false;

		int err = _conn.start(
				_settings_client.network.irc_host,
				_settings_client.network.irc_port,
				_settings_client.network.irc_nick,
				_settings_client.network.irc_nick,
				_settings_client.network.irc_nick,
				_settings_client.network.irc_pass);

		if (err == 0)
		{
			// irc server connected
			DEBUG(net, 2, "[irc] connected");

			_conn.message_loop();
			DEBUG(net, 2, "[irc] connection lost");
			_conn.disconnect();
		}
		else
		{
			// connection to irc server failed
			DEBUG(net, 2, "[irc] connection failed, errror %d", err);
		}
		CSleep(30000);
	}
}

void IRCInterface::IrcServerProducerThread(void *context)
{
	long notonchannel = 0;

	while (1)
	{
		//TODO: check joined channel
		if (_conn.connected)
		{	
			if (onchannel) {
				irc_in_queue_item queue_item;
				if (IRCInterface::InQueueRead(&queue_item))
				{
					if (!StrEmpty(queue_item.msg))
					{
						_conn.socksend_raw(queue_item.msg);
					}
				}
			} else {
				notonchannel++;
				if (notonchannel >= 300)
				{
					notonchannel = 0;
					IRCInterface::JoinChannel();
				}
			}
		}
		CSleep(30);
	}
}

