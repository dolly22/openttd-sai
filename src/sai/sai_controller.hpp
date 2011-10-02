#ifndef SAI_CONTROLLER_HPP
#define SAI_CONTROLLER_HPP

#include "../core/string_compare_type.hpp"
#include <map>

/**
 * The Controller, the class each SAI should extend. It creates the SAI, makes
 *  sure the logic kicks in correctly, and that GetTick() has a valid value.
 */
class SAIController {
	friend class SAIScanner;
	friend class SAIInstance;

public:
	static const char *GetClassName() { return "SAIController"; }

	/**
	 * Initializer of the SAIController.
	 */
	SAIController();

	/**
	 * Destructor of the SAIController.
	 */
	~SAIController();

	/**
	 * This function is called to start your SAI. Your SAI starts here. If you
	 *   return from this function, your SAI dies, so make sure that doesn't
	 *   happen.
	 * @note Cannot be called from within your SAI.
	 */
	void Start();

	/**
	 * When Squirrel triggers a print, this function is called.
	 *  Squirrel calls this when 'print' is used, or when the script made an error.
	 * @param error_msg If true, it is a Squirrel error message.
	 * @param message The message Squirrel logged.
	 * @note Use AILog.Info/Warning/Error instead of 'print'.
	 */
	static void Print(bool error_msg, const char *message);

private:
};

#endif /* SAI_CONTROLLER_HPP */
