#ifndef IRC_INTERFACE_H
#define IRC_INTERFACE_H

#include "irc.hpp"
#include "thread/thread.h"

#define MAX_IRC_NICK_LENGTH 100			// maximum irc nick length
#define MAX_IRC_MESSAGE_LENGTH 1024		// maximum irc message length
#define IRC_BUFFER_SIZE 25				// in/out command/message buffer size

// out queue IRC -> ttd
typedef struct {
	bool is_public;
	char nick[MAX_IRC_NICK_LENGTH];
	char msg[MAX_IRC_MESSAGE_LENGTH];
} irc_out_queue_item;

// in queue ttd -> IRC
typedef struct {
	char msg[MAX_IRC_MESSAGE_LENGTH];
} irc_in_queue_item;

class IRCInterface {
private:
	static IRC _conn;
	static ThreadObject *_connection_thread;
	static ThreadObject *_producer_thread;

	static bool onchannel;

	// out queue implementation
	static irc_out_queue_item _out_queue[IRC_BUFFER_SIZE];
	static int _out_queue_reader;
	static int _out_queue_writer;

	static irc_in_queue_item _in_queue[IRC_BUFFER_SIZE];
	static int _in_queue_reader;
	static int _in_queue_writer;

public:
	static void Initialize();
	static void Terminate();

	static void SendMsg(const char *msg);
	static void SendMsg(const char *dest, const char *msg);
	static void SendMsgFmt(const char *fmt, ...);

	static void SendNotice(const char *msg);
	static void SendNotice(const char *dest, const char *msg);

	static void OutQueueWrite(const irc_out_queue_item *item);
	static void ProcessQueuedMessage();

	static void SendRaw(const char *fmt, ...);
	static void SendRaw(const char * fmt, va_list argp);

	static void OpSelf();
	static void JoinChannel();
	static void OnChannelLost();

private:
	static void IrcServerConnectionThread(void *context);
	static void IrcServerProducerThread(void *context);

	static bool InQueueEmpty();
	static bool InQueueRead(irc_in_queue_item *item);
	static void InQueueWrite(const irc_in_queue_item *item);

	static bool OutQueueEmpty();
	static bool OutQueueRead(irc_out_queue_item *item);

};

#endif
