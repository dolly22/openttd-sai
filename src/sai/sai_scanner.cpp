#include "../stdafx.h"
#include "../debug.h"
#include "../fileio_func.h"

#include <squirrel.h>
#include "../script/squirrel.hpp"
#include "../script/squirrel_helper.hpp"
#include "../script/squirrel_class.hpp"
#include "../settings_type.h"
#include "sai_info.hpp"
#include "sai_scanner.hpp"


void SAIScanner::ScanAIDir()
{
	this->ScanScriptDir("info.nut", SAI_DIR);
}

SAIScanner::SAIScanner() :
	ScriptScanner()
{
	DefSQClass <SAIInfo> SQSAIInfo("SAIInfo");
	SQSAIInfo.PreRegister(engine);
	SQSAIInfo.AddConstructor<void (SAIInfo::*)(), 1>(engine, "x");
	//SQSAController.DefSQMethod(engine, &SAController::SetCallbackFlags, "SetCallbackFlags", 2, ".i");
	//SQSAController.DefSQMethod(engine, &SAController::GetCallbackFlags, "GetCallbackFlags", 1, ".");	
	SQSAIInfo.PostRegister(engine);
	
	this->engine->AddMethod("RegisterSAI", &SAIInfo::Constructor, 2, "tx");

	this->ScanAIDir();
}

SAIScanner::~SAIScanner()
{
	SAIInfoList::iterator it = this->info_list.begin();
	for (; it != this->info_list.end(); it++) {
		free((void *)(*it).first);
		delete (*it).second;
	}
	it = this->info_single_list.begin();
	for (; it != this->info_single_list.end(); it++) {
		free((void *)(*it).first);
	}
}


void SAIScanner::RegisterSAI(SAIInfo *info)
{
	char sai_name[1024];
	snprintf(sai_name, sizeof(sai_name), "%s.%d", info->GetName(), info->GetVersion());
	strtolower(sai_name);

	/* Check if GetShortName follows the rules */
	if (strlen(info->GetShortName()) != 4) {
		DEBUG(ai, 0, "[sai] The SAI '%s' returned a string from GetShortName() which is not four characaters. Unable to load the SAI.", info->GetName());
		delete info;
		return;
	}

	if (this->info_list.find(sai_name) != this->info_list.end()) {
		/* This AI was already registered */
#ifdef WIN32
		/* Windows doesn't care about the case */
		if (strcasecmp(this->info_list[sai_name]->GetMainScript(), info->GetMainScript()) == 0) {
#else
		if (strcmp(this->info_list[sai_name]->GetMainScript(), info->GetMainScript()) == 0) {
#endif
			delete info;
			return;
		}

		DEBUG(ai, 0, "Registering two SAIs with the same name and version");
		DEBUG(ai, 0, "  1: %s", this->info_list[sai_name]->GetMainScript());
		DEBUG(ai, 0, "  2: %s", info->GetMainScript());
		DEBUG(ai, 0, "The first is taking precedence.");

		delete info;
		return;
	}
	
	// print some info to console...
	DEBUG(ai, 0, "[sai] registering SAI: %s", info->GetName());

	this->info_list[strdup(sai_name)] = info;

	/* Add the AI to the 'unique' AI list, where only the highest version of the
	 *  AI is registered. */
	snprintf(sai_name, sizeof(sai_name), "%s", info->GetName());
	strtolower(sai_name);
	if (this->info_single_list.find(sai_name) == this->info_single_list.end()) {
		this->info_single_list[strdup(sai_name)] = info;
	} else if (this->info_single_list[sai_name]->GetVersion() < info->GetVersion()) {
		this->info_single_list[sai_name] = info;
	}
}

SAIInfo *SAIScanner::SelectSAI()
{
	char sai_name[1024];
	ttd_strlcpy(sai_name,  _settings_client.network.sai_name, sizeof(sai_name));
	strtolower(sai_name);

	if (!StrEmpty(sai_name))
	{
		// try to load server ai by configuration name
		if (this->info_single_list.find(sai_name) != this->info_single_list.end()) return this->info_single_list[sai_name];
		DEBUG(ai, 0, "[sai] cannot find SAI specified in config file: %s.", sai_name);
	}
	else
	{
		DEBUG(ai, 0, "[sai] no SAI specified in configuration, using first available.");
	}

	for (SAIInfoList::const_iterator it = this->info_single_list.begin(); it != this->info_single_list.end(); it++) {
		return it->second;
	}

	DEBUG(ai, 0, "[sai] no suitable SAI found.");
	return NULL;
}
