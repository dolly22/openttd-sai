/** @file sai_object.cpp SA API implementation of SAIObject */

#include <squirrel.h>
#include "../../stdafx.h"
#include "../../script/squirrel.hpp"
#include "sai_object.hpp"
#include "../sai.hpp"

/* static */ void SAIObject::ReportError(const char* msg) {
	SAIInstance *instance = ::SAI::GetInstance();
	assert(instance != NULL);
	assert(::SAI::IsStarted());

	throw sq_throwerror(instance->engine->GetVM(), OTTD2SQ(msg)); 
}
