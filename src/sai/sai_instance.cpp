#include "../stdafx.h"
#include "../debug.h"
#include "../saveload/saveload.h"
#include "../gui.h"
#include "../company_func.h"

#include <stdio.h>
#include <stdarg.h>
#include <squirrel.h>
#include "../script/squirrel.hpp"
#include "../script/squirrel_helper.hpp"
#include "../script/squirrel_class.hpp"
#include "sai_info.hpp"
#include "sai_instance.hpp"
#include "sai_controller.hpp"

#include "../ai/ai_instance.hpp"
#include "../ai/ai_storage.hpp"
#include "../ai/api/ai_object.hpp"
#include "api/sai_controller.hpp.sq"

void Register_SAI_API(Squirrel* engine);

static void PrintFunc(bool error_msg, const SQChar *message)
{
	/* Convert to OpenTTD internal capable string */
	SAIController::Print(error_msg, SQ2OTTD(message));
}

SAIInstance::SAIInstance() :
	controller(NULL),
	engine(NULL),
	instance(NULL),
	is_dead(false)
{
	this->controller = new SAIController();
	this->engine     = new Squirrel();
	this->engine->SetPrintFunction(&PrintFunc);

	/* Register the SAIController */
	SQSAIController_Register(this->engine);

	Register_SAI_API(this->engine);
}

SAIInstance::~SAIInstance()
{
	if (instance != NULL) this->engine->ReleaseObject(this->instance);
	if (engine != NULL) delete this->engine;
	delete this->controller;
	free(this->instance);
}

bool SAIInstance::Initialize(SAIInfo *info)
{
	try {
		// never allow AI do commands for SAI (use own handling)
		AIObject::SetAllowDoCommand(false);

		/* Load and execute the script for this AI */
		const char *main_script = info->GetMainScript();

		if (!this->engine->LoadScript(main_script) || this->engine->IsSuspended()) {
			//if (this->engine->IsSuspended()) AILog::Error("This SAI took too long to load script. SAI is not started.");
			this->Died();
			return false;
		}

		/* Create the main-class */
		this->instance = MallocT<SQObject>(1);
		if (!this->engine->CreateClassInstance(info->GetInstanceName(), this->controller, this->instance)) {
			this->instance = NULL;
			this->Died();
			return false;
		}

		// initialize constructors
		if (this->engine->MethodExists(*this->instance, "constructor")) {
			if (!this->engine->CallMethod(*this->instance, "constructor", -1) || this->engine->IsSuspended()) {
				DEBUG(ai, 1, "[sai] SAIController constructor call failed");
				this->Died();
				return false;
			}
		}

		// start processing
		if (this->engine->MethodExists(*this->instance, "Start")) {
			if (!this->engine->CallMethod(*this->instance, "Start", -1) || this->engine->IsSuspended()) {
				DEBUG(ai, 1, "[sai] SAIController Start() method failed");
				this->Died();
				return false;
			}
		}

		return true;
	} catch (AI_FatalError e) {
		this->is_dead = true;
		this->engine->ThrowError(e.GetErrorMessage());
		this->engine->ResumeError();
		this->Died();
	}	
	return false;
}

void SAIInstance::Died()
{
	DEBUG(ai, 0, "[sai] Server AI died.");
	this->is_dead = true;

	if (this->instance != NULL) this->engine->ReleaseObject(this->instance);
	delete this->engine;
	this->instance = NULL;
	this->engine = NULL;
}


bool SAIInstance::CallMethod(const char* method_name, HSQOBJECT *res, const char* arg_format, ...)
{
	va_list vl;
	va_start(vl, arg_format);

	bool result = true;
	try {
		result = CallMethod(method_name, res, arg_format, vl);
	} catch (AI_VMSuspend) {
	}
	va_end(vl);

	return result;
}

bool SAIInstance::CallMethod(const char* method_name, HSQOBJECT *res, const char* arg_format, va_list vl)
{
	CompanyID old = _current_company;
	_current_company = OWNER_NONE;

	bool result = true;
	try {
		result = this->engine->CallMethod(*this->instance, method_name, res, -1, arg_format, vl);
	} catch (AI_VMSuspend) {
	}
	_current_company = old;

	return result;
}

bool SAIInstance::MethodExists(const char* method_name)
{
	// we are not so strict as in ai here...
	this->engine->ResetCrashed();

	if (!this->engine->MethodExists(*this->instance, method_name))
	{
		DEBUG(ai, 3, "[sai] SAIController does not contain '%s' callback implementation.", method_name);
		return false;
	}
	return true;
}

bool SAIInstance::GetInteger(HSQOBJECT object, int *res)
{
	if (object._type != OT_INTEGER)
		return false;

	*res = this->engine->ObjectToInteger(&object);
	return true;
}