/** @file sai_town.hpp SA API implementation of SAITown */

#ifndef SA_TOWN_HPP
#define SA_TOWN_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../ai/api/ai_town.hpp"
#include "../../town.h"

class SAITown : public AIObject  {
public:
	static const char *GetClassName() { return "SAITown"; }

	/**
	* Get amount of food delivered last month
	*/
	static int32 GetLastMonthDeliveredFood(TownID town_id);

	/**
	* Get amount of water delivered last month
	*/
	static int32 GetLastMonthDeliveredWater(TownID town_id);

	/**
	* Get amount of goods delivered last month
	*/
	static int32 GetLastMonthDeliveredGoods(TownID town_id);

	/**
	* Get amount of valuables delivered last month
	*/
	static int32 GetLastMonthDeliveredValuables(TownID town_id);

	/**
	* Get amount of food delivered this month
	*/
	static int32 GetCurrentMonthDeliveredFood(TownID town_id);

	/**
	* Get amount of water delivered this month
	*/
	static int32 GetCurrentMonthDeliveredWater(TownID town_id);

	/**
	* Get amount of goods delivered this month
	*/
	static int32 GetCurrentMonthDeliveredGoods(TownID town_id);

	/**
	* Get amount of valuables delivered this month
	*/
	static int32 GetCurrentMonthDeliveredValuables(TownID town_id);

	/**
	* Get nr of months the fund buildings is yet active
	*/
	static int32 GetFundBuildingsMonths(TownID town_id);

	/**
	* Get the rate town is currently growing
	*/
	static int32 GetGrowthRate(TownID town_id);

	/*
	** Set universal town storage value
	*/
	static void SetStorage(TownID town, uint32 storage_index, uint32 value);

	/*
	** Get universal town storage value
	*/
	static uint32 GetStorage(TownID town, uint32 storage_index);

	/**
	* Return true if town is a city
	*/
	static bool IsCity(TownID town_id);
private:

};

#endif /* SA_TOWN_HPP */
