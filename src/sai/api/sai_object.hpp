/* $Id$ */
/** @file sai_advancedobject.hpp Advanced SAI object, on which all objects depend. */

#ifndef SAI_ADVANCED_OBJECT_HPP
#define SAI_ADVANCED_OBJECT_HPP

#include "../../stdafx.h"
#include "../../misc/countedptr.hpp"
#include <squirrel.h>

class SAIObject : public SimpleCountedObject {
public:
	static void ReportError(const char* msg);
};

#endif /* SAI_ADVANCED_OBJECT_HPP */
