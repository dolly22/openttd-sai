#ifndef SAI_INSTANCE_HPP
#define SAI_INSTANCE_HPP

#include <squirrel.h>

class SAIInstance {
	friend class SAI;
	friend class SAIObject;

public:
	SAIInstance();
	~SAIInstance();

	/**
	 * Load and initialize this SAI engine instance
	 */		
	bool Initialize(class SAIInfo *info);

	void Died();

	/**
	 * Invoke method
	 */		
	bool CallMethod(const char* method_name, HSQOBJECT *res, const char* arg_format, ...);

	/**
	 * Invoke method
	 */		
	bool CallMethod(const char* method_name, HSQOBJECT *res, const char* arg_format, va_list vl);

	/**
	 * Test if method exists
	 */		
	bool MethodExists(const char* name);

	/**
	 * Get integer representation of object
	 */	
	bool GetInteger(HSQOBJECT object, int *res);

private:
	class SAIController *controller;
	class Squirrel *engine;
	SQObject *instance;

	bool is_dead;

};

#endif /* SAI_INSTANCE_HPP */