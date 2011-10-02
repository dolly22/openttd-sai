/** @file sa_tile.cpp SA API implementation of SAISign */

#include "sai_sign.hpp"
#include "sai_server.hpp"
#include "table/strings.h"
#include "../../command_func.h"
#include "../../core/alloc_func.hpp"
#include "../../signs_base.h"
#include "../../string_func.h"
#include "../../strings_func.h"
#include "../../tile_map.h"
#include "../../tile_type.h"
#include "../../company_base.h"
#include "../../ai/api/ai_sign.hpp"

/* static */ bool SAISign::IsValidSignID(SignID sign_id)
{
	return ::Sign::IsValidID(sign_id);
}

/* static */ bool SAISign::IsProtected(SignID sign_id)
{
	if (!::Sign::IsValidID(sign_id))
		return false;
	
	return ::Sign::Get(sign_id)->is_protected;
}

/* static */ void SAISign::SetProtected(SignID sign_id, bool prot)
{
	if (!Sign::IsValidID(sign_id))
		return;

	::Sign::Get(sign_id)->is_protected = prot;
}

/* static */ bool SAISign::BuildSignCompany(SAICompany::CompanyID company, TileIndex location, uint32 data1, uint32 data2, const char *text)
{
	if (!::IsValidTile(location) || !::Company::IsValidID((::CompanyID)company) || !(::strlen(text) < MAX_LENGTH_SIGN_NAME_CHARS))
		return false;

	::SAIServer::ExecuteCommandEx(location, data1, data2, CMD_PLACE_SIGN, text, (::CompanyID)company);
	return true;
}

/* static */ bool SAISign::BuildSign(TileIndex location, uint32 data1, uint32 data2, const char *text)
{
	if (!::IsValidTile(location) || !(::strlen(text) < MAX_LENGTH_SIGN_NAME_CHARS))
		return false;

	::SAIServer::ExecuteCommand(location, data1, data2, CMD_PLACE_SIGN, text);
	return true;
}

/* static */ bool SAISign::SetName(SignID sign_id, const char *text)
{
	if (!Sign::IsValidID(sign_id))
		return false;

	::SAIServer::ExecuteCommand(0, sign_id, 0, CMD_RENAME_SIGN, text);
	return true;
}

/* static */ bool SAISign::SetNameCompany(SAICompany::CompanyID company, SignID sign_id, const char *text)
{
	if (!Sign::IsValidID(sign_id) || !::Company::IsValidID((::CompanyID)company) )
		return false;

	::SAIServer::ExecuteCommandEx(0, sign_id, 0, CMD_RENAME_SIGN, text, (::CompanyID)company);
	return true;
}


/* static */ char *SAISign::GetName(SignID sign_id)
{
	if (!Sign::IsValidID(sign_id))
		return NULL;

	static const int len = 64;
	char *sign_name = MallocT<char>(len);

	::SetDParam(0, sign_id);
	::GetString(sign_name, STR_SIGN_NAME, &sign_name[len - 1]);

	return sign_name;
}


/* static */ bool SAISign::RemoveSign(SignID sign_id)
{
	if (!Sign::IsValidID(sign_id))
		return false;

	::SAIServer::ExecuteCommand(0, sign_id, 0, CMD_RENAME_SIGN, "");
	return true;
}

/* static */ int32 SAISign::GetOwner(SignID sign_id)
{
	if (!Sign::IsValidID(sign_id))
		return false;

	return ::Sign::Get(sign_id)->owner;
}


SAISignList::SAISignList()
{
	Sign *s;
	FOR_ALL_SIGNS(s) {
		if (::Sign::IsValidID(s->index)) this->AddItem(s->index);
	}
}

