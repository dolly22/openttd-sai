/** @file sa_company.hpp SA API implementation of SAICompany */

#include "sai_company.hpp"
#include "../../company_base.h"
#include "../../tile_map.h"
#include "../../ai/api/ai_company.hpp"
#include "../../network/network_func.h"

/* static */ bool SAICompany::IsValid(CompanyID company)
{
	return ::Company::IsValidID(company);
}

/* static */ char* SAICompany::GetName(CompanyID company)
{
	if (!IsValid(company))
	return NULL;

	return AICompany::GetName((AICompany::CompanyID)company);
}

/* static */ int SAICompany::InauguratedYear(SAICompany::CompanyID company)
{
	if (!IsValid(company))
		return -1;

	return ::Company::Get(company)->inaugurated_year;
}

/* static */ bool SAICompany::IsSuspended(CompanyID company)
{
	if (!IsValid(company))
		return false;

	return ::Company::Get(company)->is_suspended;
}

/* static */ int SAICompany::MonthsEmpty(CompanyID company)
{
	if (!IsValid(company))
		return -1;

	return _network_company_states[company].months_empty;
}

/* static */ Money SAICompany::GetMoney(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->money;
}

/* static */ Money SAICompany::GetCEIncome(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->cur_economy.income;
}

/* static */ Money SAICompany::GetCEExpenses(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->cur_economy.expenses;
}

/* static */ int32 SAICompany::GetCEDeliveredCargo(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->cur_economy.delivered_cargo;
}

/* static */ int32 SAICompany::GetCEPerformance(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->cur_economy.performance_history;
}

/* static */ Money SAICompany::GetCECompanyValue(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	return ::Company::Get(company)->cur_economy.company_value;
}


/* static */ Money SAICompany::CurrentCV(CompanyID company)
{
	if (!IsValid(company))
		return 0;
	return ::CalculateCompanyValue(::Company::Get(company));
}

/* static */ Money SAICompany::CurrentNCV(CompanyID company)
{
	if (!IsValid(company))
		return 0;	

	return ::CalculateCompanyNCV(::Company::Get(company));
}

/* static */ uint32 SAICompany::CurrentDeliveredCargo(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	Company* c = ::Company::Get(company);

	// Copy from economy.cpp - UpdateCompanyRatingAndValue()
	const CompanyEconomyEntry *cee;
	int numec;
	uint32 total_delivered = 0;

	numec = min(c->num_valid_stat_ent, 4);
	if (numec != 0) {
		cee = c->old_economy;
		total_delivered = 0;
		do {
			total_delivered += cee->delivered_cargo;
		} while (++cee,--numec);
	}

	return total_delivered;
}

/* static */ Money SAICompany::CurrentIncome(CompanyID company)
{
	if (!IsValid(company))
		return 0;

	Company* c = ::Company::Get(company);

	// Copy from economy.cpp - UpdateCompanyRatingAndValue()
	const CompanyEconomyEntry *cee;
	int numec;
	Money total_income = 0;

	numec = min(c->num_valid_stat_ent, 4);
	if (numec != 0) {
		cee = c->old_economy;
		total_income = 0;
		do {
			total_income += cee->income;
		} while (++cee,--numec);
	}

	return total_income;
}



/* static */ void SAICompany::SetSuspended(CompanyID company, bool suspended)
{
	if (!IsValid(company))
		return;

	::Company::Get(company)->is_suspended = suspended;
}

/* static */ void SAICompany::SetStorage(CompanyID company, uint32 storage_index, uint32 value) {
	if (!IsValid(company))
		return;

	if (storage_index > SAI_COMPANY_STORAGE_SIZE)
		return;

	::Company::Get(company)->sa_storage[storage_index] = value;
}

/* static */ uint32 SAICompany::GetStorage(CompanyID company, uint32 storage_index) {
	if (!IsValid(company))
		return 0;

	if (storage_index > SAI_COMPANY_STORAGE_SIZE)
		return 0;

	return ::Company::Get(company)->sa_storage[storage_index];
}

/* static */ bool SAICompany::IsProtected(CompanyID company)
{
	if (!IsValid(company))
		return false;

	return ::Company::Get(company)->is_protected;
}

/* static */ void SAICompany::SetProtected(CompanyID company, bool prot)
{
	if (!IsValid(company))
		return;

	::Company::Get(company)->is_protected = prot;
}

/* static */ uint32 SAICompany::GetHQSize(CompanyID company) {
	if (!IsValid(company))
		return 0;

	TileIndex hqTile = ::Company::Get(company)->location_of_HQ;	
	if (hqTile == 0)
		return 0;

	return GetAnimationFrame(hqTile);
}