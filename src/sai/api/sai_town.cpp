#include "sai_town.hpp"


/* static */ int32 SAITown::GetLastMonthDeliveredFood(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->act_food;
}

/* static */ int32 SAITown::GetLastMonthDeliveredWater(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->act_water;
}

/* static */ int32 SAITown::GetLastMonthDeliveredGoods(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->act_goods;
}

/* static */ int32 SAITown::GetLastMonthDeliveredValuables(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->act_valuables;
}

/* static */ int32 SAITown::GetCurrentMonthDeliveredFood(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->new_act_food;
}

/* static */ int32 SAITown::GetCurrentMonthDeliveredWater(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->new_act_water;
}

/* static */ int32 SAITown::GetCurrentMonthDeliveredGoods(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->new_act_goods;
}

/* static */ int32 SAITown::GetCurrentMonthDeliveredValuables(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->new_act_valuables;
}

/* static */ int32 SAITown::GetFundBuildingsMonths(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->fund_buildings_months;
}


/* static */ int32 SAITown::GetGrowthRate(TownID town_id)
{
	if (!::Town::IsValidID(town_id)) return -1;

	const Town *t = ::Town::Get(town_id);
	return t->growth_rate;
}

/* static */ void SAITown::SetStorage(TownID town, uint32 storage_index, uint32 value) {
	if (!::Town::IsValidID(town))
		return;

	if (storage_index > SAI_TOWN_STORAGE_SIZE)
		return;

	::Town::Get(town)->sa_storage[storage_index] = value;
}

/* static */ uint32 SAITown::GetStorage(TownID town, uint32 storage_index) {
	if (!::Town::IsValidID(town))
		return 0;

	if (storage_index > SAI_TOWN_STORAGE_SIZE)
		return 0;

	return ::Town::Get(town)->sa_storage[storage_index];
}


/* static */ bool SAITown::IsCity(TownID town) {
	if (!::Town::IsValidID(town))
		return 0;

	return ::Town::Get(town)->larger_town;
}