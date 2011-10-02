/** @file sa_gamesettings.cpp SA API implementation of SAIGameSettings */

#include "sai_gamesettings.hpp"
#include "../../settings_internal.h"
#include "../../network/network.h"
#include "../../command_type.h"
#include "../../command_func.h"

/* static */ bool SAIGameSettings::IsValid(const char* setting)
{
	uint i;
	const SettingDesc *sd = GetSettingFromName(setting, &i);
	return sd != NULL;
}

/* static */ bool SAIGameSettings::GetBoolValue(SettingType type, const char* setting)
{
	uint i;
	const void *ptr;

	const SettingDesc *sd = GetSettingFromName(setting, &i);

	if (sd == NULL)
		return false;

	ptr = GetVariableAddress(resolveSettingByType(type), &sd->save);

	if (sd->desc.cmd != SDT_BOOLX)
		return false;

	return (*(bool*)ptr == 1);
}

/* static */ int32 SAIGameSettings::GetIntValue(SettingType type, const char* setting)
{
	uint i;
	const void *ptr;

	const SettingDesc *sd = GetSettingFromName(setting, &i);

	if (sd == NULL)
		return 0;

	ptr = GetVariableAddress(resolveSettingByType(type), &sd->save);

	if (sd->desc.cmd == SDT_STRING)
		return 0;

	return (int32)ReadValue(ptr, sd->save.conv);
}

/* static */ const char* SAIGameSettings::GetStringValue(SettingType type, const char* setting)
{
	uint i;
	const void *ptr;

	const SettingDesc *sd = GetSettingFromName(setting, &i);

	if (sd == NULL)
		return NULL;

	ptr = GetVariableAddress(resolveSettingByType(type), &sd->save);

	if (sd->desc.cmd != SDT_STRING)
		return NULL;

	return (const char *)ptr;
}

/* static */ bool SAIGameSettings::SetValue(SettingType type, const char* setting, int32 value)
{
	uint i;

	const SettingDesc *sd = GetSettingFromName(setting, &i);

	if (sd == NULL)
		return false;

	GameSettings* settings = resolveSettingByType(type);

	if (type == SAIGameSettings::GS_NEWGAME || sd->save.conv & SLF_NETWORK_NO) {
		void *var = GetVariableAddress(settings, &sd->save);
		Write_ValidateSetting(var, sd, value);

		if (type != SAIGameSettings::GS_NEWGAME) {
			void *var2 = GetVariableAddress(&_settings_newgame, &sd->save);
			Write_ValidateSetting(var2, sd, value);
		}
		if (sd->desc.proc != NULL) sd->desc.proc((int32)ReadValue(var, sd->save.conv));
		return true;
	}

	/* send non-company-based settings over the network */
	if (!_networking || (_networking && _network_server)) {
		return DoCommandP(0, i, value, CMD_CHANGE_SETTING);
	}

	return false;
}

/* static */ bool SAIGameSettings::SetStringValue(SettingType type, const char* setting, const char* value)
{
	uint i;

	const SettingDesc *sd = GetSettingFromName(setting, &i);

	if (sd == NULL)
		return false;

	return false;

	//TODO: not implemented yet, will be needed?
	//if (sd->desc.cmd == SDT_STRING) {
	//	success = SetSettingValue(index, value);
	//} else {
	//	uint32 val;
	//	extern bool GetArgumentInteger(uint32 *value, const char *arg);
	//	success = GetArgumentInteger(&val, value);
	//	if (success) success = SetSettingValue(index, val);
	//}
}



// get type...
/* static */ GameSettings* SAIGameSettings::resolveSettingByType(SettingType type)
{
	switch (type)
	{
		case SAIGameSettings::GS_NEWGAME:
			return &_settings_newgame;
			break;
		case SAIGameSettings::GS_CURRENT_GAME:
		default:
			return &_settings_game;
	}
}