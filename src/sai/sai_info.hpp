#ifndef SAI_INFO
#define SAI_INFO

#include "../script/script_info.hpp"

class SAIFileInfo : public ScriptFileInfo {
public:
	/**
	 * Process the creation of a FileInfo object.
	 */
	static SQInteger Constructor(HSQUIRRELVM vm, SAIFileInfo *info);

protected:
	class SAIScanner *base;
};

class SAIInfo : public SAIFileInfo {
public:
	static const char *GetClassName() { return "SAIInfo"; }

	static SQInteger Constructor(HSQUIRRELVM vm);
};

#endif /* SAI_INFO */
