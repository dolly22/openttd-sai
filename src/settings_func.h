/* $Id: settings_func.h 20689 2010-08-29 15:58:43Z alberth $ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file settings_func.h Functions related to setting/changing the settings. */

#ifndef SETTINGS_FUNC_H
#define SETTINGS_FUNC_H

#include "core/smallvec_type.hpp"
#include "company_type.h"

void IConsoleSetSetting(const char *name, const char *value, bool force_newgame = false);
void IConsoleSetSetting(const char *name, int32 value);
void IConsoleGetSetting(const char *name, bool force_newgame = false);
void IConsoleListSettings(const char *prefilter);

void LoadFromConfig();
void SaveToConfig();
void CheckConfig();

/* Functions to load and save NewGRF settings to a separate
 * configuration file, used for presets. */
typedef AutoFreeSmallVector<char *, 4> GRFPresetList;

void GetGRFPresetList(GRFPresetList *list);
struct GRFConfig *LoadGRFPresetFromConfig(const char *config_name);
void SaveGRFPresetToConfig(const char *config_name, struct GRFConfig *config);
void DeleteGRFPresetFromConfig(const char *config_name);

uint GetCompanySettingIndex(const char *name);
void SetDefaultCompanySettings(CompanyID cid);

#if defined(ENABLE_NETWORK)
void SyncCompanySettings();
#else /* ENABLE_NETWORK */
static inline void SyncCompanySettings() {}
#endif /* ENABLE_NETWORK */

#endif /* SETTINGS_FUNC_H */
