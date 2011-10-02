/** @file sai_companylist.hpp SA API implementation of SAICompanyList */

#ifndef SA_COMPANYLIST_HPP
#define SA_COMPANYLIST_HPP

#include "../../ai/api/ai_list.hpp"

class SAICompanyList : public AIList  {
public:
	static const char *GetClassName() { return "SAICompanyList"; }

	/*
	** List all companies
	*/
	SAICompanyList();

private:

};

#endif /* SA_COMPANYLIST_HPP */
