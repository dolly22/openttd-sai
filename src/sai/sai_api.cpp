#include <squirrel.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>
#include "../stdafx.h"
#include "../debug.h"
#include "../script/squirrel.hpp"
#include "../script/squirrel_class.hpp"
#include "../script/squirrel_helper.hpp"
#include "../script/squirrel_helper_type.hpp"

#include "sai.hpp"

#define DEFINE_SCRIPT_FILES

// Include selected AI API
#include "../ai/api/ai_company.hpp.sqh"
#include "../ai/api/ai_list.hpp.sqh"
#include "../ai/api/ai_tile.hpp.sqh"
#include "../ai/api/ai_tilelist.hpp.sqh"
#include "../ai/api/ai_town.hpp.sqh"
#include "../ai/api/ai_townlist.hpp.sqh"
#include "../ai/api/ai_date.hpp.sqh"
#include "../ai/api/ai_base.hpp.sqh"
#include "../ai/api/ai_map.hpp.sqh"
#include "../ai/api/ai_sign.hpp.sqh"
#include "../ai/api/ai_signlist.hpp.sqh"
#include "../ai/api/ai_basestation.hpp.sqh"
#include "../ai/api/ai_station.hpp.sqh"
#include "../ai/api/ai_cargo.hpp.sqh"
#include "../ai/api/ai_industry.hpp.sqh"
#include "../ai/api/ai_industrylist.hpp.sqh"
#include "../ai/api/ai_industrytype.hpp.sqh"

// Include API classes
#include "api/sai_clientlist.hpp.sq"
#include "api/sai_companylist.hpp.sq"
#include "api/sai_company.hpp.sq"
#include "api/sai_client.hpp.sq"
#include "api/sai_tile.hpp.sq"
#include "api/sai_sign.hpp.sq"
#include "api/sai_industry.hpp.sq"
#include "api/sai_town.hpp.sq"
#include "api/sai_server.hpp.sq"
#include "api/sai_gamesettings.hpp.sq"
#include "api/sai_irc.hpp.sq"

// SAI Extensions
#include "../saiext/sqlite3/sqlite_library.hpp"

#undef DEFINE_SCRIPT_FILES

void Register_SAI_API(Squirrel* engine)
{
	// basic squirrel library support
	sqstd_register_mathlib(engine->GetVM());
	sqstd_register_stringlib(engine->GetVM());
	sqstd_register_systemlib(engine->GetVM());

	SQAIList_Register(engine);
	SQAICompany_Register(engine);
	SQAIDate_Register(engine);
	SQAITile_Register(engine);
	SQAITown_Register(engine);
	SQAITownList_Register(engine);
	SQAISign_Register(engine);
	SQAISignList_Register(engine);
	SQAIBaseStation_Register(engine);
	SQAIStation_Register(engine);
	SQAICargo_Register(engine);
	SQAIIndustry_Register(engine);
	SQAIIndustryType_Register(engine);
	SQAIIndustryList_Register(engine);
	SQAIIndustryList_CargoAccepting_Register(engine);
	SQAIIndustryList_CargoProducing_Register(engine);
	SQAITileList_Register(engine);
	SQAITileList_IndustryAccepting_Register(engine);
	SQAITileList_IndustryProducing_Register(engine);
	SQAITileList_StationType_Register(engine);
	SQAITown_Register(engine);
	SQAIBase_Register(engine);
	SQAIMap_Register(engine);

	SQSAIServer_Register(engine);
	SQSAIClient_Register(engine);
	SQSAICompany_Register(engine);
	SQSAITown_Register(engine);
	SQSAIGameSettings_Register(engine);
	SQSAIClientList_Register(engine);
	SQSAICompanyList_Register(engine);
	SQSAIIndustry_Register(engine);
	SQSAISign_Register(engine);
	SQSAISignList_Register(engine);
	SQSAITile_Register(engine);
	SQSAIIndustryList_PlayerFounded_Register(engine);

	SQSqliteLibrary_Register(engine);
	SQSAIIrc_Register(engine);
}