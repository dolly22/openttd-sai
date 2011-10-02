#ifndef SAI_SCANNER_HPP
#define SAI_SCANNER_HPP

#include <squirrel.h>
#include "../script/script_scanner.hpp"
#include "sai.hpp"

class SAIScanner : public ScriptScanner {
public:
	SAIScanner();
	~SAIScanner();

	/**
	 * Scan the SAI dir for scripts.
	 */
	void ScanAIDir();

	/**
	 * Register an SAI to be put in the available list.
	 */
	void RegisterSAI(class SAIInfo *info);

	/**
	 * Get configured server ai instance
	 */
	class SAIInfo *SelectSAI();
private:

	SAIInfoList info_list;
	SAIInfoList info_single_list;
};

#endif /* SAI_SCANNER_HPP */