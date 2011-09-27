#ifndef SAI_STORAGE_HPP
#define SAI_STORAGE_HPP

#include "../ai/ai_storage.hpp"
#include "api/sai_company.hpp"

class SAIStorage : public AIStorage {
	friend class SAIServer;

private:
	SAICompany::CompanyID execute_commands_as;

public:
	SAIStorage() : 
		AIStorage(),
		execute_commands_as(SAICompany::OWNER_NONE)
	{};
};

#endif /* SAI_STORAGE_HPP */