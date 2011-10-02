/** @file sa_companylist.hpp SA API implementation of SAIClientList */

#include "../../stdafx.h"
#include "../../network/network_base.h"
#include "../../network/network_internal.h"
#include "../../network/network_client.h"

#include "sai_clientlist.hpp"

SAIClientList::SAIClientList()
{
	const NetworkClientInfo *ci;
	FOR_ALL_CLIENT_INFOS(ci) {
		if (ci->client_id == CLIENT_ID_SERVER)
			continue;

		this->AddItem(ci->client_id);
	}
}