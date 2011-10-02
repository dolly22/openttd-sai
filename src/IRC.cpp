/*
	cpIRC - C++ class based IRC protocol wrapper
	Copyright (C) 2003 Iain Sheppard

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

	Contacting the author:
	~~~~~~~~~~~~~~~~~~~~~~

	email:	iainsheppard@yahoo.co.uk
	IRC:	#magpie @ irc.quakenet.org
*/


#include "IRC.hpp"
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define closesocket(s) close(s)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif
#include "stdafx.h"
#include "debug.h"

IRC::IRC()
{
	hooks=0;
	chan_users=0;
	connected=false;
	sentnick=false;
	sentpass=false;
	sentuser=false;
	cur_nick[0]=0;
}

IRC::~IRC()
{
	if (hooks)
		delete_irc_command_hook(hooks);
}

void IRC::insert_irc_command_hook(irc_command_hook* hook, char* cmd_name, int (*function_ptr)(char*, irc_reply_data*, void*))
{
	if (hook->function)
	{
		if (!hook->next)
		{
			hook->next=new irc_command_hook;
			hook->next->function=0;
			hook->next->irc_command=0;
			hook->next->next=0;
		}
		insert_irc_command_hook(hook->next, cmd_name, function_ptr);
	}
	else
	{
		hook->function=function_ptr;
		hook->irc_command=new char[strlen(cmd_name)+1];
		strcpy(hook->irc_command, cmd_name);
	}
}

void IRC::hook_irc_command(char* cmd_name, int (*function_ptr)(char*, irc_reply_data*, void*))
{
	if (!hooks)
	{
		hooks=new irc_command_hook;
		hooks->function=0;
		hooks->irc_command=0;
		hooks->next=0;
		insert_irc_command_hook(hooks, cmd_name, function_ptr);
	}
	else
	{
		insert_irc_command_hook(hooks, cmd_name, function_ptr);
	}
}

void IRC::delete_irc_command_hook(irc_command_hook* cmd_hook)
{
	if (cmd_hook->next)
		delete_irc_command_hook(cmd_hook->next);
	if (cmd_hook->irc_command)
		delete cmd_hook->irc_command;
	delete cmd_hook;
}

int IRC::start(char* server, int port, char* nick, char* user, char* name, char* pass)
{
	#ifdef WIN32
	HOSTENT* resolv;
	#else
	hostent* resolv;
	#endif
	sockaddr_in rem;

	if (connected)
		return 1;

	irc_socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (irc_socket==INVALID_SOCKET)
	{
		return 1;
	}
	resolv=gethostbyname(server);
	if (!resolv)
	{
		closesocket(irc_socket);
		return 1;
	}
	memcpy(&rem.sin_addr, resolv->h_addr, 4);
	rem.sin_family=AF_INET;
	rem.sin_port=htons(port);

	if (connect(irc_socket, (const sockaddr*)&rem, sizeof(rem))==SOCKET_ERROR)
	{
		closesocket(irc_socket);

		if  (WSAGetLastError() != 0)
			return WSAGetLastError();
		return 1;
	}

	if (pass != '\0')
		this->socksend("PASS %s\r\n", pass);
	this->socksend("NICK %s\r\n", nick);
	this->socksend("USER %s * 0 :%s\r\n", user, name);

	connected=true;
	
	//cur_nick=new char[strlen(nick)+1];
	strncpy(cur_nick, nick, sizeof(cur_nick));
	return 0;
}

void IRC::disconnect()
{
	if (connected)
	{
		connected=false;
		quit("Leaving");
		#ifdef WIN32
		shutdown(irc_socket, 2);
		#endif
		closesocket(irc_socket);
	}
}

int IRC::quit(char* quit_message)
{
	if (connected)
	{
		if (quit_message)
			this->socksend("QUIT %s\r\n", quit_message);
		else
			this->socksend("QUIT\r\n");
		if (fflush(dataout))
			return 1;
	}
	return 0;
}

int IRC::message_loop()
{
	char buffer[1024];
	int ret_len;

	if (!connected)
	{
		//printf("Not connected!\n");
		return 1;
	}

	while (1)
	{
		ret_len=recv(irc_socket, buffer, 1023, 0);
		if (ret_len==SOCKET_ERROR || !ret_len)
		{
			return 1;
		}
		buffer[ret_len]='\0';
		split_to_replies(buffer);
	}

	return 0;
}

void IRC::split_to_replies(char* data)
{
	char* p;

	while (p=strstr(data, "\r\n"))
	{
		*p='\0';
		parse_irc_reply(data);
		data=p+2;
	}
}

void IRC::parse_irc_reply(char* data)
{
	char* hostd;
	char* cmd;
	char* params;
	char buffer[514];
	irc_reply_data hostd_tmp;
	channel_user* cup;
	char* p;
	char* chan_temp;

	hostd_tmp.target=0;
	DEBUG(net, 3, "[irc] raw data %s", data);

	if (data[0]==':')
	{
		hostd=&data[1];
		cmd=strchr(hostd, ' ');
		if (!cmd)
			return;
		*cmd='\0';
		cmd++;
		params=strchr(cmd, ' ');
		if (params)
		{
			*params='\0';
			params++;
		}
		hostd_tmp.nick=hostd;
		hostd_tmp.ident=strchr(hostd, '!');
		if (hostd_tmp.ident)
		{
			*hostd_tmp.ident='\0';
			hostd_tmp.ident++;
			hostd_tmp.host=strchr(hostd_tmp.ident, '@');
			if (hostd_tmp.host)
			{
				*hostd_tmp.host='\0';
				hostd_tmp.host++;
			}
		}
		if (!strcmp(cmd, "NOTICE"))
		{
			hostd_tmp.target=params;
			params=strchr(hostd_tmp.target, ' ');
			if (params)
				*params='\0';
			params++;
			#ifdef __IRC_DEBUG__
			DEBUG(net, 3, "[irc] %s >-%s- %s", hostd_tmp.nick, hostd_tmp.target, &params[1]);
			#endif
		}
		else if (!strcmp(cmd, "PRIVMSG"))
		{
			hostd_tmp.target=params;
			params=strchr(hostd_tmp.target, ' ');
			if (!params)
				return;
			*(params++)='\0';
			#ifdef __IRC_DEBUG__
			DEBUG(net, 3, "[irc] %s: <%s> %s", hostd_tmp.target, hostd_tmp.nick, &params[1]);
			#endif
		}
		else if (!strcmp(cmd, "NICK"))
		{
			if (!strcmp(hostd_tmp.nick, cur_nick))
			{
				//delete [] cur_nick;
				//cur_nick=new char[strlen(params)+1];
				strncpy(cur_nick, params, sizeof(cur_nick));
			}
		}
		call_hook(cmd, params, &hostd_tmp);
	}
	else
	{
		cmd=data;
		data=strchr(cmd, ' ');
		if (!data)
			return;
		*data='\0';
		params=data+1;

		if (!strcmp(cmd, "PING"))
		{
			if (!params)
				return;
			this->socksend("PONG %s\r\n", &params[1]);
			#ifdef __IRC_DEBUG__
			DEBUG(net, 3, "[irc] Ping received, pong sent.");
			#endif
			fflush(dataout);
		}
		else
		{
			hostd_tmp.host=0;
			hostd_tmp.ident=0;
			hostd_tmp.nick=0;
			hostd_tmp.target=0;
			call_hook(cmd, params, &hostd_tmp);
		}
	}
}

void IRC::call_hook(char* irc_command, char* params, irc_reply_data* hostd)
{
	irc_command_hook* p;

	if (!hooks)
		return;

	p=hooks;
	while (p)
	{
		if (!strcmp(p->irc_command, irc_command))
		{
			(*(p->function))(params, hostd, this);
			p=0;
		}
		else
		{
			p=p->next;
		}
	}
}

int IRC::notice(char* target, char* message)
{
	if (!connected)
		return 1;
	this->socksend("NOTICE %s :%s\r\n", target, message);
	return fflush(dataout);
}

int IRC::notice(char* fmt, ...)
{
	va_list argp;
	char* target;
	
	if (!connected)
		return 1;

	va_start(argp, fmt);
	this->socksend("NOTICE %s :", fmt);
	this->socksend(va_arg(argp, char*), argp);
	va_end(argp);
	this->socksend("\r\n");
	return 0;
}

int IRC::socksend_raw(char * msg)
{
	return send(this->irc_socket, msg, strlen(msg), 0);
}

int IRC::socksend(char * fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	socksend_va(fmt, argp);
	va_end(argp);

	return 0;
}

int IRC::socksend_va(char * fmt, va_list argp)
{
	char buf[8192];
	vsnprintf(buf, sizeof(buf), fmt, argp); 
	return send(this->irc_socket, buf, strlen(buf), 0);
}

int IRC::privmsg(char* target, char* message)
{
	if (!connected)
		return 1;
	return this->socksend("PRIVMSG %s :%s\r\n", target, message);
}

int IRC::join(char* channel)
{
	if (!connected)
		return 1;
	return this->socksend("JOIN %s\r\n", channel);
}

int IRC::raw(char* data)
{
	if (!connected)
		return 1;
	this->socksend("%s\r\n", data);
	return 0;
}

int IRC::mode(char* channel, char* modes, char* targets)
{
	if (!connected)
		return 1;
	if (!targets)
		this->socksend("MODE %s %s\r\n", channel, modes);
	else
		this->socksend("MODE %s %s %s\r\n", channel, modes, targets);
	return 0;
}

int IRC::mode(char* modes)
{
	if (!connected)
		return 1;
	mode(cur_nick, modes, 0);
	return 0;
}

char* IRC::current_nick()
{
	return cur_nick;
}
