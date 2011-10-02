/** @file sa_industry.cpp SA API implementation of SAIIndustry */

#include "sai_industry.hpp"
#include "sai_server.hpp"
#include "../../tile_type.h"
#include "../../industry.h"
#include "../../command_type.h"
#include "../../core/random_func.hpp"

/* static */ int32 SAIIndustry::GetFounder(IndustryID industry_id)
{	
	if (!::Industry::IsValidID(industry_id)) return -1;
	Industry *ind = ::Industry::Get(industry_id);
	return ind->founder;
}

/* static */ int32 SAIIndustry::GetWidth(IndustryID industry_id)
{
	if (!::Industry::IsValidID(industry_id)) return -1;
	Industry *ind = ::Industry::Get(industry_id);
	return ind->location.w;
}

/* static */ int32 SAIIndustry::GetHeight(IndustryID industry_id)
{
	if (!::Industry::IsValidID(industry_id)) return -1;
	Industry *ind = ::Industry::Get(industry_id);
	return ind->location.h;
}

/* static */ void SAIIndustry::ProspectIndustry(IndustryType industry_type)
{
	uint32 seed = ::InteractiveRandom();
	SAIServer::ExecuteCommand(0, industry_type, seed, CMD_BUILD_INDUSTRY, NULL);
}

SAIIndustryList_PlayerFounded::SAIIndustryList_PlayerFounded()
{
	const Industry *i;

	FOR_ALL_INDUSTRIES(i) {
		if (i->founder != OWNER_NONE)
			this->AddItem(i->index);
	}
} 
