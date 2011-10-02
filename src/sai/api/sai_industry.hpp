/** @file sai_industry.hpp SA API implementation of SAIIndustry */

#ifndef SA_INDUSTRY_HPP
#define SA_INDUSTRY_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../strings_type.h"
#include "../../settings_type.h"
#include "../../ai/api/ai_list.hpp" 

class SAIIndustry : public AIObject  {
public:
	static const char *GetClassName() { return "SAIIndustry"; }

	/**
	** Get industry founder
	**/
	static int32 GetFounder(IndustryID industry_id);

	/**
	** Get industry width
	**/
	static int32 GetWidth(IndustryID industry_id);

	/**
	** Get industry height
	**/
	static int32 GetHeight(IndustryID industry_id);

	/**
	** Prospect industry do not wait for command return
	**/
	static void ProspectIndustry(IndustryType industry_type);
};


/**
 * Creates a list of industries that were founded by player
 */
class SAIIndustryList_PlayerFounded : public AIList {
public:
	static const char *GetClassName() { return "SAIIndustryList_PlayerFounded"; }

	SAIIndustryList_PlayerFounded();
};


#endif /* SA_INDUSTRY_HPP */
