#ifdef ENABLE_NETWORK

#include "../stdafx.h"
#include "../debug.h"
#include "../strings_func.h"
#include "network_internal.h"
#include "../vehicle_base.h"
#include "../vehiclelist.h"
#include "../date_func.h"
#include "network_server.h"
#include "network_udp.h"
#include "network_base.h"
#include "../console_func.h"
#include "../command_func.h"
#include "../saveload/saveload.h"
#include "../station_base.h"
#include "../genworld.h"
#include "../fileio_func.h"
#include "../string_func.h"
#include "../company_base.h"
#include "../company_func.h"
#include "../company_gui.h"
#include "../settings_type.h"
#include "../object_type.h"
#include "../window_func.h"
#include "../sai/sai.hpp"
#include "../signs_base.h"
#include "../signs_func.h"

#include "network_checks.h"
#include "table/strings.h"


// handles altering signs...
bool HandleSignAltering(const NetworkClientInfo *ci, CommandPacket cp) {	
    if ((cp.cmd & 0xFF) == CMD_RENAME_SIGN) {
		if (::Sign::IsValidID(cp.p1) && ci->client_id != CLIENT_ID_SERVER) {
			Sign *si = ::Sign::Get(cp.p1);
			if (si->owner != ci->client_playas || si->is_protected) {
				SAI::InvokeCallback("OnSignAlterForbid", "i", ci->client_id);
				return true;
			}
		}
    }
    return false;
}

// disable advertising and road reconstruction
bool HandleDisabledTownCommands(const NetworkClientInfo *ci, CommandPacket cp)
{	
	// advertising campaigns (large stays enabled) and road reconstruction
    if ((cp.cmd & 0xFF) == CMD_DO_TOWN_ACTION && (cp.p2 == 0 || cp.p2 == 1 || cp.p2 == 3)) {
		NetworkServerSendChat(NETWORK_ACTION_CHAT_CLIENT, DESTTYPE_CLIENT, ci->client_id, "The city council did not approve this action.", CLIENT_ID_SERVER);
		return true;
    }
    return false;
}

// Company cannot execute any command when silenced
bool HandleCompanySuspended(const NetworkClientInfo *ci, CommandPacket cp)
{	
	if ((cp.cmd & 0xFF) != CMD_COMPANY_CTRL && (cp.cmd & 0xFF) != CMD_SET_AUTOREPLACE && ci->client_playas != COMPANY_SPECTATOR && ::Company::IsValidID((CompanyID)ci->client_playas))
	{
		// check silenced flag
		Company *c = ::Company::Get((CompanyID)ci->client_playas);
		if (c->is_suspended) {
			NetworkServerSendChat(NETWORK_ACTION_CHAT_CLIENT, DESTTYPE_CLIENT, ci->client_id, "You can't execute any commands while suspended.", CLIENT_ID_SERVER);
			return true;
		}
	}
	return false;
}

// Company cannot execute any command when silenced
bool HandleUnpasswordedBuild(const NetworkClientInfo *ci, CommandPacket cp)
{	
	if ((cp.cmd & 0xFF) != CMD_COMPANY_CTRL && (cp.cmd & 0xFF) != CMD_SET_AUTOREPLACE && ci->client_playas != COMPANY_SPECTATOR && ::Company::IsValidID((CompanyID)ci->client_playas) )
	{
		if (StrEmpty(_network_company_states[ci->client_playas].password))
		{
			NetworkServerSendChat(NETWORK_ACTION_CHAT_CLIENT, DESTTYPE_CLIENT, ci->client_id, "Company is locked (you cannot build) until you set a password", CLIENT_ID_SERVER);
			return true;
		}
	}
	return false;
}

bool HandleExtendedHouseDestroying(const NetworkClientInfo *ci, CommandPacket cp)
{
	int callback_result;
	uint32 base_command = cp.cmd & 0xFF;

	// addional houses destroy handling
	if (base_command == CMD_LANDSCAPE_CLEAR && ::GetTileType(cp.tile) == MP_HOUSE) {
		if (SAI::InvokeIntegerCallback("OnHouseDestroying", &callback_result, "iiii", ci->client_id, cp.tile, cp.p1, 1))
			return (callback_result != 0);
	} else if (base_command == CMD_CLEAR_AREA) {
		// compute number of houses that will be deleted
		if (cp.p1 >= MapSize()) return false;

		/* make sure sx,sy are smaller than ex,ey */
		int ex = TileX(cp.tile);
		int ey = TileY(cp.tile);
		int sx = TileX(cp.p1);
		int sy = TileY(cp.p1); 
		if (ex < sx) Swap(ex, sx);
		if (ey < sy) Swap(ey, sy);
		int houses = 0;

		for (int x = sx; x <= ex; ++x) {
			for (int y = sy; y <= ey; ++y) {
				if (::GetTileType(TileXY(x, y)) == MP_HOUSE)
					houses++;
			}
		};

		if (houses > 0 && SAI::InvokeIntegerCallback("OnHouseDestroying", &callback_result, "iiii", ci->client_id, cp.tile, cp.p1, houses))
			return (callback_result != 0);
	}
	return false;
}

bool HandleCommandCallbacks(const NetworkClientInfo *ci, CommandPacket cp)
{
	int callback_result;
	uint32 base_command = cp.cmd & 0xFF;
	uint32 callback_flags = SAI::GetCallbackFlags();

	// station build commands callbacks enabled
	if ((callback_flags & ::SAIServer::BUILD_STATION) != 0) {
		if (base_command == CMD_BUILD_RAIL_STATION) {
			//uint32 p1 = GetCurrentRailType() | (platform_length << 16) | (num_platforms << 8);
			//if (direction == RAILTRACK_NW_SE) p1 |= 1 << 4;
			//EnforcePrecondition(false, direction == RAILTRACK_NW_SE || direction == RAILTRACK_NE_SW);

			int length = cp.p1 >> 16 & 0xFF;
			int platforms = cp.p1 >> 8 & 0xFF;
			int direction = cp.p1 >> 4 & 0x1;
			TileIndex tile2;

			if (direction != 0) {
				// along y axis
				tile2 = TILE_ADDXY(cp.tile, platforms - 1, length - 1);
			} else {
				// along x axis
				tile2 = TILE_ADDXY(cp.tile, length - 1, platforms - 1);
			}

			//SAHook OnRailroadStationBuilding
			if (SAI::InvokeIntegerCallback("OnRailroadStationBuilding", &callback_result, "iii", ci->client_id, cp.tile, tile2)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_ROAD_STOP) {
			//SAHook OnRoadStopBuilding
			if (SAI::InvokeIntegerCallback("OnRoadStopBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_DOCK) {
			//SAHook OnDockBuilding
			if (SAI::InvokeIntegerCallback("OnDockBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_AIRPORT) {
			//SAHook OnAirportBuilding
			if (SAI::InvokeIntegerCallback("OnAirportBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}		
	}

	// industry and unmovables build callbacks enabled
	if ((callback_flags & ::SAIServer::BUILD_UNMOVABLES) != 0) {
		if (base_command == CMD_BUILD_INDUSTRY) {
			// not interested in prospecting industries
			if (cp.tile == 0)
				return false;

			//SAHook OnBuildIndustry
			if (SAI::InvokeIntegerCallback("OnBuildingIndustry", &callback_result, "iii", ci->client_id, cp.tile, GB(cp.p1, 0, 16))) {
				return (callback_result != 0);
			}
		}	

		if (base_command == CMD_BUILD_OBJECT && cp.p1 == OBJECT_HQ) {
			//SAHook OnCompanyHQBuilding
			if (SAI::InvokeIntegerCallback("OnCompanyHQBuilding", &callback_result, "iii", ci->client_id, cp.tile, TILE_ADDXY(cp.tile, 1, 1))) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_PLACE_SIGN) {
			//SAHook OnSignBuilding
			if (SAI::InvokeIntegerCallback("OnSignBuilding", &callback_result, "iii", ci->client_playas, ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
	}

	// landscaping and clear commands callbacks enabled
	if ((callback_flags & ::SAIServer::BULDOZE_LANDSCAPE) != 0) {	
		if (base_command == CMD_LANDSCAPE_CLEAR || base_command == CMD_CLEAR_AREA) {
			//SAHook OnTileClearing
			if (SAI::InvokeIntegerCallback("OnTileClearing", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				if (callback_result != 0)
					return true;
			}

			// extended house destroying handling
			return HandleExtendedHouseDestroying(ci, cp);
		}
		if (base_command == CMD_TERRAFORM_LAND) {
			//SAHook OnTerraforming
			if (SAI::InvokeIntegerCallback("OnTerraforming", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_LEVEL_LAND) {
			//SAHook OnLandLeveling
			if (SAI::InvokeIntegerCallback("OnLandLeveling", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				return (callback_result != 0);
			}
		}
	}

	// building commands callbacks enabled
	if ((callback_flags & ::SAIServer::BULDOZE_LANDSCAPE) != 0) {	
		if (base_command == CMD_BUILD_RAILROAD_TRACK) {
			//SAHook OnRailroadTrackBuilding
			if (SAI::InvokeIntegerCallback("OnRailroadTrackBuilding", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_SINGLE_RAIL) {
			//SAHook OnRailroadTrackBuilding
			if (SAI::InvokeIntegerCallback("OnRailroadBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_BRIDGE) {
			//SAHook OnBridgeBuilding
			if (SAI::InvokeIntegerCallback("OnBridgeBuilding", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_TRAIN_DEPOT) {
			//SAHook OnTrainDepotBuilding
			if (SAI::InvokeIntegerCallback("OnTrainDepotBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_OBJECT && cp.p1 == OBJECT_OWNED_LAND) {
			//SAHook OnPurchasingLand
			if (SAI::InvokeIntegerCallback("OnPurchasingLand", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_LONG_ROAD) {
			//SAHook OnLongRoadBuilding
			if (SAI::InvokeIntegerCallback("OnLongRoadBuilding", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_ROAD) {
			//SAHook OnRoadBuilding
			if (SAI::InvokeIntegerCallback("OnRoadBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_ROAD_DEPOT) {
			//SAHook OnRoadDepotBuilding
			if (SAI::InvokeIntegerCallback("OnRoadDepotBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}
		if (base_command == CMD_BUILD_SHIP_DEPOT) {
			//SAHook OnShipDepotBuilding
			if (SAI::InvokeIntegerCallback("OnShipDepotBuilding", &callback_result, "ii", ci->client_id, cp.tile)) {
				return (callback_result != 0);
			}
		}	
		if (base_command == CMD_BUILD_CANAL) {
			//SAHook OnCanalBuilding
			if (SAI::InvokeIntegerCallback("OnCanalBuilding", &callback_result, "iii", ci->client_id, cp.tile, cp.p1)) {
				return (callback_result != 0);
			}
		}	
	}

	// other commands
	if ((callback_flags & ::SAIServer::OTHER) != 0) {
			//SAHook OnTownCommand
			if (SAI::InvokeIntegerCallback("OnTownCommand", &callback_result, "iii", ci->client_id, cp.tile, cp.p2)) {
				return (callback_result != 0);
			}
	}
	return false;
}


// Check if this command is not disabled and check other cheat protections
bool PrecheckNetworkCommands(NetworkClientSocket *cs, CommandPacket cp)
{
	NetworkClientInfo *ci = cs->GetInfo();

	if (!ci->is_admin && !ci->is_moderator)
	{
		if (HandleCompanySuspended(ci, cp))
			return true;
		if (HandleSignAltering(ci, cp))
			return true;		
	}

	if (HandleUnpasswordedBuild(ci, cp))
		return true;
	if (HandleDisabledTownCommands(ci, cp))
		return true;
	if (HandleCommandCallbacks(ci, cp))
		return true;

	return false;
}

#endif
