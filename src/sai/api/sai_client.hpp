/** @file sai_client.hpp SA API implementation of SAIClient */

#ifndef SAI_CLIENT_HPP
#define SAI_CLIENT_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../network/network_type.h"
#include "../../ai/api/ai_company.hpp"

class SAIClient : public SimpleCountedObject  {
public:
	static const char *GetClassName() { return "SAIClient"; }

	/** Different constants related to CompanyID. */
	enum ClientID {
		INVALID_CLIENT_ID = 0, ///< Client is not part of anything
		CLIENT_ID_SERVER  = 1, ///< Servers always have this ID
		CLIENT_ID_FIRST   = 2, ///< The first client ID
	};

	/*
	** Check if client with this id exists
	*/
	static bool IsValid(SAIClient::ClientID client);

	/*
	** Check if client is admin
	*/
	static bool IsAdmin(SAIClient::ClientID client_id);

	/*
	** Set admin flag for client
	*/
	static void SetAdmin(SAIClient::ClientID client_id, bool is_admin);

	/*
	** Check if client is moderator
	*/
	static bool IsModerator(SAIClient::ClientID client_id);

	/*
	** Set moderator flag for client
	*/
	static void SetModerator(SAIClient::ClientID client_id, bool is_moderator);

	/*
	** Get name of the client
	*/
	static char* GetName(SAIClient::ClientID client_id);
	
	/*
	** Get company the client plays for
	*/
	static AICompany::CompanyID GetCompany(SAIClient::ClientID client_id);

	/* 
	** Get client IP as string
	*/
	static const char* GetAddress(SAIClient::ClientID client_id);

	/* 
	** Get client IP hash as string
	*/
	static const char* GetAnonymizedAddress(SAIClient::ClientID client_id);

	/*
	** Get the date client joined
	*/
	static uint32 GetJoinedDate(SAIClient::ClientID client_id);

	/*
	** Disconnect client with statu code
	*/
	static void Disconnect(SAIClient::ClientID client_id, uint32 status);

	/*
	** check if hostname is resolved
	*/
	static bool IsHostNameResolved(SAIClient::ClientID client_id);

	/*
	** Get host name
	*/
	static char* GetHostName(SAIClient::ClientID client_id);

	/*
	** Get anonymized host name
	*/
	static char* GetAnonymizedHostName(SAIClient::ClientID client_id);

private:

};

#endif /* SAI_CLIENT_HPP */
