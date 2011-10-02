#include "../stdafx.h"

#include <squirrel.h>
#include "../script/squirrel.hpp"
#include "../script/squirrel_helper.hpp"
#include "sai_info.hpp"
#include "sai_scanner.hpp"


/* static */ SQInteger SAIFileInfo::Constructor(HSQUIRRELVM vm, SAIFileInfo *info)
{
	SQInteger res = ScriptFileInfo::Constructor(vm, info);
	if (res != 0) return res;
	info->base = ((SAIScanner *)Squirrel::GetGlobalPointer(vm));

	return 0;
}

/* static */ SQInteger SAIInfo::Constructor(HSQUIRRELVM vm)
{
	/* Get the AIInfo */
	SQUserPointer instance = NULL;
	if (SQ_FAILED(sq_getinstanceup(vm, 2, &instance, 0)) || instance == NULL) return sq_throwerror(vm, _SC("Pass an instance of a child class of SAIInfo to RegisterSAI"));
	SAIInfo *info = (SAIInfo *)instance;

	SQInteger res = SAIFileInfo::Constructor(vm, info);
	if (res != 0) return res;

	/* Remove the link to the real instance, else it might get deleted by RegisterSAI() */
	sq_setinstanceup(vm, 2, NULL);
	/* Register the AI to the base system */
	info->base->RegisterSAI(info);
	return 0;
}