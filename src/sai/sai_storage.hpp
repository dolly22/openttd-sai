#ifndef SAI_STORAGE_HPP
#define SAI_STORAGE_HPP

#include "../ai/ai_storage.hpp"
#include "api/sai_company.hpp"
#include "api/sai_server.hpp"

class SAIStorage : public AIStorage {
	friend class SAIServer;
	friend class SAI;

private:
	SAICompany::CompanyID execute_commands_as;
	SAIServer::CallbackFlags command_callback_flags;

public:
	SAIStorage() : 
		AIStorage(),
		execute_commands_as(SAICompany::OWNER_NONE),
		command_callback_flags((SAIServer::CallbackFlags)0)
	{
	};
};

#endif /* SAI_STORAGE_HPP */