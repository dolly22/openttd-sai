/** @file sai_server.hpp SA API implementation of SAIServer */

#ifndef SAI_SERVER_HPP
#define SAI_SERVER_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../network/network_type.h"
#include "sai_client.hpp"
#include "sai_company.hpp"

class SAIServer : public SimpleCountedObject  {
public:
	static const char *GetClassName() { return "SAIServer"; }

	/*
	** Say to all 
	*/
	static void Say(const char* msg);

	/*
	** Say to client by index
	*/
	static void SayClient(SAIClient::ClientID client, const char* msg);

	/*
	** Say to company by index
	*/
	static void SayCompany(SAICompany::CompanyID company, const char* msg);

	/*
	** Execute console command
	*/
	static void ConsoleCmd(const char* command);

	/**
	* Execute command
	**/
	static void ExecuteCommand(TileIndex tile, uint32 p1, uint32 p2, uint32 cmd, const char* text);

#ifndef EXPORT_SKIP
	/**
	* Execute command as company
	**/
	static void ExecuteCommandEx(TileIndex tile, uint32 p1, uint32 p2, uint32 cmd, const char* text, CompanyID company);

#endif /* EXPORT_SKIP */

private:

};

#endif /* SAI_SERVER_HPP */
