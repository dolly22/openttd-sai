/** @file sai_irc.cpp SA API implementation of SAIIrc */

#include "../../stdafx.h"
#include "../../string_func.h"
#include "../../irc_interface.hpp"
#include "sai_irc.hpp"
#include "../sai.hpp"

/* static */ void SAIIrc::Message(const char* msg)
{
	if (!StrEmpty(msg))
		IRCInterface::SendMsg(msg);
}

/* static */ void SAIIrc::MessageNick(const char* destination, const char* msg)
{
	if (!StrEmpty(msg))
		IRCInterface::SendMsg(destination, msg);
}

/* static */ void SAIIrc::Notice(const char* msg)
{
	if (!StrEmpty(msg))
		IRCInterface::SendNotice(msg);
}

/* static */ void SAIIrc::NoticeNick(const char* destination, const char* msg)
{
	if (!StrEmpty(msg))
		IRCInterface::SendNotice(destination, msg);
}

/* static */ void SAIIrc::RawCommand(const char* msg)
{
	if (!StrEmpty(msg))
		IRCInterface::SendRaw(msg);
}

