#pragma once
#include <string>

namespace JDKLevelMaps::Baking
{
	struct SBakeRunResult
	{
		bool success = false;
		std::string message;

		SBakeRunResult() {}
		SBakeRunResult(bool success, std::string msg) : success(success), message(msg) {}
	};
}