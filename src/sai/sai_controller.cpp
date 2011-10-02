#include "../stdafx.h"
#include "../string_func.h"
#include "../company_base.h"
#include "../rev.h"
#include "../console_func.h"
#include "../network/network.h"
#include "../network/network_func.h"

#include "sai_controller.hpp"
#include "sai_instance.hpp"

/* static */ void SAIController::Print(bool error_msg, const char *message)
{
	if (_network_dedicated) {
		fprintf(stdout, "sai: %s", message);
		fflush(stdout);
		IConsoleWriteToLogFile(message);
	}
}

SAIController::SAIController()
{
}

SAIController::~SAIController()
{
}

