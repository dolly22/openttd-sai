/** @file sai_gamesettings.hpp SA API implementation of SAIGameSettings */

#ifndef SA_GAMESETTINGS_HPP
#define SA_GAMESETTINGS_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../strings_type.h"
#include "../../settings_type.h"

class SAIGameSettings : public SimpleCountedObject  {
public:
	static const char *GetClassName() { return "SAIGameSettings"; }

	enum SettingType {
		GS_NEWGAME,			///< Game settings
		GS_CURRENT_GAME,    ///< Newgame settings
	};	

	/*
	** Test if setting is valid
	*/
	static bool IsValid(const char *setting);

	/*
	** Get bool value from specified setting
	*/
	static bool GetBoolValue(SettingType type, const char* setting);

	/*
	** Get int value from specified setting
	*/
	static int32 GetIntValue(SettingType type, const char* setting);

	/*
	** Get string value from specified setting
	*/
	static const char* GetStringValue(SettingType type, const char* setting);

	/*
	** Set int32 value for specified setting
	*/
	static bool SetValue(SettingType type, const char* setting, int32 value);

	/*
	** Set string value for specified setting
	*/
	static bool SetStringValue(SettingType type, const char* setting, const char* value);
private:
	/*
	** resolve concrete settings reference
	*/
	static GameSettings* resolveSettingByType(SettingType type);

};

#endif /* SA_GAMESETTINGS_HPP */
