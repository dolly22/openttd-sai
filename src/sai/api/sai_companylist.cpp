/** @file sa_companylist.hpp SA API implementation of SAICompanyList */

#include "../../stdafx.h"
#include "../../core/bitmath_func.hpp"
#include "../../company_base.h"
#include "../../company_func.h"
#include "sai_companylist.hpp"

SAICompanyList::SAICompanyList()
{
	const Company *c;
	FOR_ALL_COMPANIES(c) {
		if (Company::IsHumanID(c->index)) {
			this->AddItem(c->index);
		}
	}
}