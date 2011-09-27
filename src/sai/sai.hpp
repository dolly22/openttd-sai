#ifndef SAI_HPP
#define SAI_HPP

#include "../core/string_compare_type.hpp"
#include <map>

#include "sai_storage.hpp"
#include "sai_instance.hpp"


typedef std::map<const char *, class SAIInfo *, StringCompare> SAIInfoList;

class SAI {
public:

	/**
	 * Initialize the Server AI system.
	 */
	static void Initialize();


	/**
	 * UnInitialize the Server AI system.
	 */
	static void Uninitialize();


	static bool IsStarted();

	/**
	 * Start Server AI.
	 */
	static void Start();

	static SAIInstance *GetInstance();

	static SAIStorage *GetStorage();

	/**
	 * Execute command issued from some AI commands
	 */
	static void ExecuteAICommand(TileIndex tile, uint32 p1, uint32 p2, uint cmd, const char *text);

	/**
	 * Invoke callback function with no return value
	 */		
	static bool InvokeCallback(const char* name);

	/**
	 * Invoke callback function with no return value and input params
	 */		
	static bool InvokeCallback(const char* method_name, const char* arg_format, ...);

	/**
	 * Invoke callback function with integer return value
	 */		
	static bool InvokeIntegerCallback(const char* name, int *res);

	/**
	 * Invoke callback function with integer return value and variable number of arguments
	 */		
	static bool InvokeIntegerCallback(const char* name, int *res, const char* arg_format, ...);

	/**
	 * Test if method exists
	 */		
	static bool CallbackExists(const char* name);

private:
	static class SAIScanner *sai_scanner;
	static class SAIInstance *sai_instance;
	static class SAIStorage *storage;
};

#endif /* SAI_HPP */