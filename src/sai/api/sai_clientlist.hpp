/** @file sai_clientlist.hpp SA API implementation of SAIClientList */

#ifndef SAI_CLIENTLIST_HPP
#define SAI_CLIENTLIST_HPP

#include "../../ai/api/ai_list.hpp"
#include "../../ai/api/ai_company.hpp"

class SAIClientList : public AIList  {
public:
	static const char *GetClassName() { return "SAIClientList"; }

	/*
	** List all clients
	*/
	SAIClientList();

private:

};

#endif /* SAI_CLIENTLIST_HPP */
