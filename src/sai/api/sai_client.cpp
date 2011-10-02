#include "sai_client.hpp"
#include "../../string_func.h"
#include "../../core/alloc_func.hpp"
#include "../../network/network_base.h"
#include "../../network/network_func.h"
#include "../../network/network_server.h"
#include "../../network/network.h"
#include "../../ai/api/ai_company.hpp"

/* static */ bool SAIClient::IsValid(SAIClient::ClientID client_id)
{
	if (NetworkClientInfo::GetByClientID((::ClientID)client_id) != NULL)
		return true;

	return false;
}

/* static */ bool SAIClient::IsAdmin(SAIClient::ClientID client_id)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return false;

	return ci->is_admin;
}

/* static */ void SAIClient::SetAdmin(SAIClient::ClientID client_id, bool is_admin)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return;

	ci->is_admin = is_admin;
}

/* static */ bool SAIClient::IsModerator(SAIClient::ClientID client_id)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return false;

	return ci->is_moderator;
}

/* static */ void SAIClient::SetModerator(SAIClient::ClientID client_id, bool is_moderator)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return;

	ci->is_moderator = is_moderator;
}

/* static */ char* SAIClient::GetName(SAIClient::ClientID client_id)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return NULL;
	
	char *client_name = MallocT<char>(NETWORK_CLIENT_NAME_LENGTH);

	if (StrEmpty(ci->client_name)) {
		snprintf(client_name, NETWORK_CLIENT_NAME_LENGTH-1, "Client #%4d", ci->client_id);
	} else {
		ttd_strlcpy(client_name, ci->client_name, NETWORK_CLIENT_NAME_LENGTH-1);
	}
	return client_name;
}

/* static */ AICompany::CompanyID SAIClient::GetCompany(SAIClient::ClientID client_id)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return AICompany::COMPANY_INVALID;

	return (AICompany::CompanyID)ci->client_playas;
}

/* static */ const char* SAIClient::GetAddress(SAIClient::ClientID client_id)
{
	NetworkClientSocket *cs;

	cs = NetworkClientSocket::GetByClientID((::ClientID)client_id);
	if (cs == NULL)
		return NULL;

	return cs->client_address.GetHostname();
}

/* static */ const char* SAIClient::GetAnonymizedAddress(SAIClient::ClientID client_id)
{
	NetworkClientSocket *cs;

	cs = NetworkClientSocket::GetByClientID((::ClientID)client_id);
	if (cs == NULL)
		return NULL;

	return cs->client_address.GetAnonymizedAddressAsString();
}


/* static */ uint32 SAIClient::GetJoinedDate(SAIClient::ClientID client_id)
{
	NetworkClientInfo *ci;

	ci = NetworkClientInfo::GetByClientID((::ClientID)client_id);		
	if (ci == NULL)
		return NULL;

	return ci->join_date;
}


/* static */ void SAIClient::Disconnect(SAIClient::ClientID client_id, uint32 status)
{
    if (client_id == CLIENT_ID_SERVER) return;
	NetworkClientSocket::GetByClientID((::ClientID)client_id)->SendError((::NetworkErrorCode)status);
}

