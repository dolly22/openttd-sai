/** @file sai_sign.hpp SA API implementation of SAISign */

#ifndef SA_SIGN_HPP
#define SA_SIGN_HPP

#include "../../stdafx.h"
#include "../../ai/api/ai_list.hpp"
#include "../../misc/countedptr.hpp"
#include "../../company_func.h"
#include "../../company_type.h"
#include "../../tile_type.h"
#include "../../signs_base.h"
#include "sai_company.hpp"

class SAISign : public AIObject {
public:
	static const char *GetClassName() { return "SAISign"; }

	/*
	** Test if signid is valid
	*/
	static bool IsValidSignID(SignID sign_id);

	/*
	** Is company protected from autoclean
	*/
	static bool IsProtected(SignID sign_id);

	/*
	** Set company protected flag
	*/
	static void SetProtected(SignID sign_id, bool prot);

	/*
	** Build sign as specific company
	**/
	static bool BuildSignCompany(SAICompany::CompanyID company, TileIndex location, uint32 data1, uint32 data2, const char *text);

	/*
	** Build sign as owner none
	**/
	static bool BuildSign(TileIndex location, uint32 data1, uint32 data2, const char *text);

	/*
	** Set sign text
	**/
	static bool SetName(SignID sign_id, const char *text);

	/*
	** Set sign text
	**/
	static bool SetNameCompany(SAICompany::CompanyID company, SignID sign_id, const char *text);


	/*
	** Get sign text
	**/
	static char* GetName(SignID sign_id);

	/*
	** Remove sign
	**/
	static bool RemoveSign(SignID sign_id);

	/*
	** Get sign owner
	*/
	static int32 GetOwner(SignID sign_id);
private:

};


/**
 * Create a list of signs your company has created.
 * @ingroup AIList
 */
class SAISignList : public AIList {
public:
	static const char *GetClassName() { return "SAISignList"; }
	SAISignList();
};


#endif /* SA_SIGN_HPP */
