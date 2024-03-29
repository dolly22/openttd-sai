/* $Id: void_cmd.cpp 20419 2010-08-09 06:48:52Z frosch $ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file void_cmd.cpp Handling of void tiles. */

#include "stdafx.h"
#include "tile_cmd.h"
#include "command_func.h"
#include "viewport_func.h"

#include "table/strings.h"
#include "table/sprites.h"

static void DrawTile_Void(TileInfo *ti)
{
	DrawGroundSprite(SPR_SHADOW_CELL, PAL_NONE);
}


static uint GetSlopeZ_Void(TileIndex tile, uint x, uint y)
{
	return TilePixelHeight(tile);
}

static Foundation GetFoundation_Void(TileIndex tile, Slope tileh)
{
	return FOUNDATION_NONE;
}

static CommandCost ClearTile_Void(TileIndex tile, DoCommandFlag flags)
{
	return_cmd_error(STR_ERROR_OFF_EDGE_OF_MAP);
}


static void GetTileDesc_Void(TileIndex tile, TileDesc *td)
{
	td->str = STR_EMPTY;
	td->owner[0] = OWNER_NONE;
}

static void TileLoop_Void(TileIndex tile)
{
	/* not used */
}

static void ChangeTileOwner_Void(TileIndex tile, Owner old_owner, Owner new_owner)
{
	/* not used */
}

static TrackStatus GetTileTrackStatus_Void(TileIndex tile, TransportType mode, uint sub_mode, DiagDirection side)
{
	return 0;
}

static CommandCost TerraformTile_Void(TileIndex tile, DoCommandFlag flags, uint z_new, Slope tileh_new)
{
	return_cmd_error(STR_ERROR_OFF_EDGE_OF_MAP);
}

extern const TileTypeProcs _tile_type_void_procs = {
	DrawTile_Void,            // draw_tile_proc
	GetSlopeZ_Void,           // get_slope_z_proc
	ClearTile_Void,           // clear_tile_proc
	NULL,                     // add_accepted_cargo_proc
	GetTileDesc_Void,         // get_tile_desc_proc
	GetTileTrackStatus_Void,  // get_tile_track_status_proc
	NULL,                     // click_tile_proc
	NULL,                     // animate_tile_proc
	TileLoop_Void,            // tile_loop_clear
	ChangeTileOwner_Void,     // change_tile_owner_clear
	NULL,                     // add_produced_cargo_proc
	NULL,                     // vehicle_enter_tile_proc
	GetFoundation_Void,       // get_foundation_proc
	TerraformTile_Void,       // terraform_tile_proc
};
