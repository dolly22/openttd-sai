/** @file sai_irc.hpp SA API implementation of SAIIrc */

#ifndef SAI_IRC_HPP
#define SAI_IRC_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"

class SAIIrc : public SimpleCountedObject  {
public:
	static const char *GetClassName() { return "SAIIrc"; }

	/*
	** Say to whole irc channel
	*/
	static void Message(const char* msg);

	/*
	** Say to destination on irc
	*/
	static void MessageNick(const char* destination, const char* msg);

	/*
	** Notice to irc
	*/
	static void Notice(const char* msg);

	/*
	** Notice to destination on irc
	*/
	static void NoticeNick(const char* destination, const char* msg);

	/**
	* Irc raw command
	*/
	static void RawCommand(const char* msg);

private:

};

#endif /* SAI_IRC_HPP */
