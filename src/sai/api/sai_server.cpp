/** @file sa_server.cpp SA API implementation of SAIServer */

#include "sai_server.hpp"
#include "sai_client.hpp"
#include "../../string_func.h"
#include "../../core/alloc_func.hpp"
#include "../../network/network_internal.h"
#include "../../network/network_base.h"
#include "../../network/network_func.h"
#include "../../console_func.h"
#include "../../company_func.h"
#include "../../company_base.h"
#include "../../command_type.h"
#include "../../command_func.h"
#include "../sai.hpp"
#include "../../irc_interface.hpp"

extern Company *DoStartupNewCompany(bool is_ai, CompanyID company = INVALID_COMPANY);

/* static */ void SAIServer::Say(const char* msg)
{
	if (!StrEmpty(msg))
		NetworkServerSendChat(NETWORK_ACTION_CHAT, DESTTYPE_BROADCAST, NULL, msg, CLIENT_ID_SERVER, NULL);
}

/* static */ void SAIServer::SayEx(bool notice_irc, const char* msg)
{
	if (!StrEmpty(msg))
		NetworkServerSendChat(NETWORK_ACTION_CHAT, DESTTYPE_BROADCAST, NULL, msg, CLIENT_ID_SERVER, NULL);

	if (notice_irc)
		IRCInterface::SendNotice(msg);
}

/* static */ void SAIServer::SayClient(SAIClient::ClientID client, const char* msg)
{
	if (!SAIClient::IsValid(client))
		return;

	if (!StrEmpty(msg))
		NetworkServerSendChat(NETWORK_ACTION_CHAT_CLIENT, DESTTYPE_CLIENT, client, msg, CLIENT_ID_SERVER, NULL);
}

/* static */ void SAIServer::SayCompany(SAICompany::CompanyID company, const char* msg)
{
	if (!SAICompany::IsValid(company))
		return;

	if (!StrEmpty(msg))
		NetworkServerSendChat(NETWORK_ACTION_CHAT_COMPANY, DESTTYPE_TEAM, company, msg, CLIENT_ID_SERVER, NULL);
}

/* static */ void SAIServer::ConsoleCmd(const char* command)
{
	::IConsoleCmdExec(command);
}

/* static */ SAICompany::CompanyID SAIServer::GetExecuteCommandCompany() {
	return ::SAI::GetStorage()->execute_commands_as;
}

/* static */ void SAIServer::SetExecuteCommandCompany(SAICompany::CompanyID company) {
	::SAI::GetStorage()->execute_commands_as = company;
}

/* static */ void SAIServer::ExecuteCommand(TileIndex tile, uint32 p1, uint32 p2, uint32 cmd, const char* text) {
	// execute based on what company is specified in SAIStorage

	const SAIStorage* storage = ::SAI::GetStorage();
	CompanyID company = OWNER_NONE;

	if (storage->execute_commands_as == ::SAICompany::OWNER_TOWN 
		|| storage->execute_commands_as == ::SAICompany::OWNER_NONE
		|| ::Company::IsValidID(storage->execute_commands_as)) {

		// execute as owner none or owner town
		company = (CompanyID)storage->execute_commands_as;
	}
	if (storage->execute_commands_as == ::SAICompany::COMPANY_SERVER) {
		// execute as server company if available
		const Company *c;
		FOR_ALL_COMPANIES(c) {
			if (c->is_server) {
				company = c->index;
				break;
			}
		}
	}

	SAIServer::ExecuteCommandEx(tile, p1, p2, cmd, text, company);
}


/* static */ void SAIServer::ExecuteCommandEx(TileIndex tile, uint32 p1, uint32 p2, uint32 cmd, const char* text, CompanyID company) {
	/* NetworkSendCommand needs _local_company to be set correctly, so
	 * adjust it, and put it back right after the function */
	::NetworkSendCommand(tile, p1, p2, cmd, NULL, text, company);
}

/* static */ SAICompany::CompanyID SAIServer::CreateServerCompany(const char* name, const char* president_name) {
	// server company is always the first company
	// this call is avalable only on _frame_counter = 0
	// otherwise if first company already exists, it's taken as server company instead

	if (_frame_counter > 0)
		return ::SAICompany::COMPANY_INVALID;
	
	Company *c;

	if (::Company::IsValidID(COMPANY_FIRST))
	{
		// already exists? takeover
		c = ::Company::Get(COMPANY_FIRST);
	} else {
		// create new company
		c = DoStartupNewCompany(false, COMPANY_FIRST);
	}
	c->is_protected = true;
	c->is_server = true;
	c->name = strdup(name);
	c->current_loan = 0;
	c->money = INT64_MAX;
	c->president_name = strdup(president_name);
	
	// generate impossible password hash
	strecpy(_network_company_states[c->index].password, "impossible", lastof(_network_company_states[c->index].password));
	NetworkServerUpdateCompanyPassworded(c->index, !StrEmpty(_network_company_states[c->index].password));

	return (::SAICompany::CompanyID)(CompanyID)c->index;

}

/* static */ SAIServer::CallbackFlags SAIServer::GetCallbackFlags() {
	return ::SAI::GetStorage()->command_callback_flags;
}

/* static */ void SAIServer::SetCallbackFlags(SAIServer::CallbackFlags flags) {
	::SAI::GetStorage()->command_callback_flags = flags;
}