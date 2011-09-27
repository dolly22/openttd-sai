#include <stdio.h>
#include <stdarg.h>
#include <squirrel.h>
#include "../stdafx.h"
#include "../core/bitmath_func.hpp"
#include "../company_base.h"
#include "../company_func.h"
#include "../debug.h"
#include "../network/network.h"
#include "sai.hpp"
#include "sai_scanner.hpp"
#include "sai_instance.hpp"
#include "sai_info.hpp"
#include "api/sai_server.hpp"

/* static */ SAIInstance *SAI::sai_instance = NULL;
/* static */ SAIScanner *SAI::sai_scanner = NULL;
/* static */ SAIStorage *SAI::storage = NULL;

/* static */ void SAI::Initialize()
{
	if (SAI::sai_scanner != NULL) SAI::Uninitialize();

	if (SAI::sai_scanner == NULL) 
	{
		SAI::storage = new SAIStorage();
		SAI::sai_scanner = new SAIScanner();
	}
}

/* static */ void SAI::Uninitialize()
{
	if (SAI::sai_instance != NULL)
	{
		delete SAI::sai_instance;
		SAI::sai_instance = NULL;
	}
	if (SAI::sai_scanner != NULL)
	{
		delete SAI::sai_scanner;
		SAI::sai_scanner = NULL;
	}
	if (SAI::storage != NULL)
	{
		delete SAI::storage;
		SAI::storage = NULL;
	}
}

/* static */ bool SAI::IsStarted()
{
	if (SAI::sai_instance != NULL && !SAI::sai_instance->is_dead)
		return true;
	return false;
}


/* static */ void SAI::Start()
{
	/* Clients shouldn't start AIs */
	if (_networking && !_network_server) return;

	SAIInfo *info = SAI::sai_scanner->SelectSAI();
	if (info != NULL)
	{
		// try to start this SAI
		DEBUG(ai, 0, "[sai] starting SAI: %s", info->GetName());
		sai_instance = new SAIInstance();

		if (!sai_instance->Initialize(info)) {
			DEBUG(ai, 0, "[sai] the SAI did not initialize correctly.");
			delete sai_instance;
			sai_instance = NULL;
		}
	}
	return;
}

/* static */ SAIInstance *SAI::GetInstance()
{
	return sai_instance;
}

/* static */ SAIStorage *SAI::GetStorage()
{
	return storage;
}


/* static */ bool SAI::InvokeCallback(const char* method_name)
{
	return SAI::InvokeCallback(method_name, NULL, NULL);
}

/* static */ bool SAI::InvokeCallback(const char* method_name, const char* arg_format, ...)
{
	if (!CallbackExists(method_name))
		return false;

	va_list va;
	va_start(va, arg_format);

	bool result = SAI::sai_instance->CallMethod(method_name, NULL, arg_format, va);
	va_end(va);

	return result;
}

/* static */ bool SAI::InvokeIntegerCallback(const char* method_name, int *res)
{
	return SAI::InvokeIntegerCallback(method_name, res, NULL, NULL);
}

/* static */ bool SAI::InvokeIntegerCallback(const char* method_name, int *res, const char* arg_format, ...)
{
	HSQOBJECT sqresult;

	if (!CallbackExists(method_name))
		return false;

	va_list va;
	va_start(va, arg_format);

	bool result = SAI::sai_instance->CallMethod(method_name, &sqresult, arg_format, va);
	va_end(va);

	return SAI::sai_instance->GetInteger(sqresult, res);
}

/* static */ bool SAI::CallbackExists(const char* method_name)
{
	if (!IsStarted())
		return false;

	return SAI::sai_instance->MethodExists(method_name);
}

/* static */ void SAI::ExecuteAICommand(TileIndex tile, uint32 p1, uint32 p2, uint cmd, const char *text)
{
	::SAIServer::ExecuteCommand(tile, p1, p2, cmd, text);
}