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

	enum CallbackFlags {
		BUILD_STATION		= 0x1,		///< Station build commands
		BUILD_UNMOVABLES	= 0x2,		///< Unmovables build commands
		BULDOZE_LANDSCAPE	= 0x4,		///< Landcaping and clear commands
		BUILD_TRANSPORT		= 0x8,		///< Building transport infrastructure (roads, rail, ...)
		OTHER				= 0x10,		///< Other commands
	};

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

	/*
	 * Get what company will be network commands send as
	 */
	static SAICompany::CompanyID GetExecuteCommandCompany();

	/*
	 * Set what company will be network commands send as
	 */
	static void SetExecuteCommandCompany(SAICompany::CompanyID);

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


	/*
	** Create server company (for citibuilder and similar goals)
	*/
	static void SetCallbackFlags(SAIServer::CallbackFlags flags);

	/*
	** Create server company (for citibuilder and similar goals)
	*/
	static SAIServer::CallbackFlags GetCallbackFlags();
private:

};

#endif /* SAI_SERVER_HPP */
