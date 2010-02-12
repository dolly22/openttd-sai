/** @file sai_company.hpp SA API implementation of SAICompany */

#ifndef SA_COMPANY_HPP
#define SA_COMPANY_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include "../../economy_type.h"

class SAICompany : public SimpleCountedObject  {
public:
	static const char *GetClassName() { return "SAICompany"; }

	/** Different constants related to CompanyID. */
	enum CompanyID {
		COMPANY_INVALID = -1, //!< An invalid company.

		OWNER_TOWN				= 0x0F,
		OWNER_NONE				= 0x10,

		/* fake companies */
		COMPANY_SERVER			= 252,
		COMPANY_INACTIVE_CLIENT = 253, ///< The client is joining
		COMPANY_NEW_COMPANY     = 254, ///< The client wants a new company
		COMPANY_SPECTATOR       = 255, ///< The client is spectating
	};

	/*
	** Is company valid
	*/
	static bool IsValid(CompanyID company);

	/*
	** Get company name
	*/
	static char* GetName(CompanyID company);

	/*
	** Get company inagurated year
	*/
	static int InauguratedYear(CompanyID company);

	/*
	** Is company suspended
	*/
	static bool IsSuspended(CompanyID company);

	/*
	** Months this company has been empty
	*/
	static int MonthsEmpty(CompanyID company);

	/*
	** Company bank account balance
	*/
	static Money GetMoney(CompanyID company);

	/*
	** Current economy entry company income
	*/
	static Money GetCEIncome(CompanyID company);

	/*
	** Current economy entry company expenses
	*/
	static Money GetCEExpenses(CompanyID company);

	/*
	** Current economy entry company deliveder cargo
	*/
	static int32 GetCEDeliveredCargo(CompanyID company);

	/*
	** Current economy entry company performance
	*/
	static int32 GetCEPerformance(CompanyID company);

	/*
	** Current economy entry company value
	*/
	static Money GetCECompanyValue(CompanyID company);

	/*
	** Set company suspended flag
	*/
	static void SetSuspended(CompanyID company, bool suspended);

	/*
	** Return current company value
	*/
	static Money CurrentCV(CompanyID company);

	/*
	** Return current normalized company value
	*/
	static Money CurrentNCV(CompanyID company);

	/*
	** Return current value of delivered cargo (last 12months)
	*/
	static uint32 CurrentDeliveredCargo(CompanyID company);

	/*
	** Return current company income (last 12months)
	*/
	static Money CurrentIncome(CompanyID company);

	/*
	** Set universal company storage value
	*/
	static void SetStorage(CompanyID company, uint32 storage_index, uint32 value);

	/*
	** Get universal company storage value
	*/
	static uint32 GetStorage(CompanyID company, uint32 storage_index);

	/*
	** Is company protected from autoclean
	*/
	static bool IsProtected(CompanyID company);

	/*
	** Set company protected flag
	*/
	static void SetProtected(CompanyID company, bool prot);

	/*
	** Is a server company
	*/
	static bool IsServer(CompanyID company);

	/*
	** Get company HQ size
	*/
	static uint32 GetHQSize(CompanyID company);
private:

};

DECLARE_POSTFIX_INCREMENT(SAICompany::CompanyID);

#endif /* SA_COMPANY_HPP */
