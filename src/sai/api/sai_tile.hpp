/** @file sai_tile.hpp SA API implementation of SAITile */

#ifndef SA_TILE_HPP
#define SA_TILE_HPP

#include "../../stdafx.h"
#include "../../ai/api/ai_object.hpp"
#include "../../tile_type.h"
#include "../../tile_map.h"
#include "sai_company.hpp"

class SAITile : public AIObject  {
public:
	static const char *GetClassName() { return "SAITile"; }

	/* tropic zone tile ids */
	enum TropicZone {
		TROPICZONE_NORMAL     = 0,      ///< Normal tropiczone
		TROPICZONE_DESERT     = 1,      ///< Tile is desert
		TROPICZONE_RAINFOREST = 2,      ///< Rainforest tile
		TROPICZONE_UNKNKOWN   = 255,    ///< Cannot tell
	};

	/*
	** Get tropic type
	*/
	static TropicZone GetTropicZone(TileIndex tile);

	/**
	** Get amount of competitors stations arount tile (rectangle)
	**/
	static uint32 GetAmountOfCompetitorStationsAround(TileIndex tile, uint32 width, uint32 height, SAICompany::CompanyID company);

	/**
	** TileX() without checking valid tile
	**/
	static int32 GetRawTileX(TileIndex t);

	/**
	** TileY() without checking valid tile
	**/
	static int32 GetRawTileY(TileIndex t);

	/**
	* Test if tile is an industry tile
	* @param tile The tile to check.
	*/
	static bool IsIndustryTile(TileIndex tile);

	/**
	* Get index of industry build on tile
	* @param tile The tile to check.
	*/
	static int GetIndustryIndexForTile(TileIndex tile);

private:

};

#endif /* SA_TILE_HPP */
