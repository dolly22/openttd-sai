/** @file sa_tile.cpp SA API implementation of SAITile */

#include "sai_tile.hpp"
#include "../../landscape_type.h"
#include "../../tile_type.h"
#include "../../strings_func.h"
#include "../../station_map.h"
#include "../../station_base.h"
#include "../../industry_map.h"

/* static */ SAITile::TropicZone SAITile::GetTropicZone(TileIndex tile) {
	if (!::IsValidTile(tile) || _settings_game.game_creation.landscape != LT_TROPIC) 
		return SAITile::TROPICZONE_UNKNKOWN;

	return (TropicZone)::GetTropicZone(tile);
}

/* static */ uint32 SAITile::GetAmountOfCompetitorStationsAround(TileIndex tile, uint32 width, uint32 height, SAICompany::CompanyID company)
{
	if (!::IsValidTile(tile))
		return 0;

	StationList stations;
	TileArea ta(tile, width, height);
	::FindStationsAroundTiles(ta, &stations);

	int foundStations = 0;
	for (Station **st_iter = stations.Begin(); st_iter != stations.End(); ++st_iter) {
		Station *st = *st_iter;
		if (st->owner != company)
			foundStations++;
	}
	return (uint32)foundStations;
}

/* static */ int32 SAITile::GetRawTileX(TileIndex t)
{
	return ::TileX(t);
}

/* static */ int32 SAITile::GetRawTileY(TileIndex t)
{
	return ::TileY(t);
}


/* static */ bool SAITile::IsIndustryTile(TileIndex tile)
{
	if (!::IsValidTile(tile))
		return false;

	return (::IsTileType(tile, MP_INDUSTRY));
}

/* static */ int SAITile::GetIndustryIndexForTile(TileIndex tile)
{
	if (!IsIndustryTile(tile))
		return -1;

	return ::GetIndustryIndex(tile);
}