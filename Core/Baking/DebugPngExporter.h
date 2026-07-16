#pragma once
#include <string>
#include <vector>
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps
{
	class IMapBaker;
	struct SBakeContext;

	bool ExportDebugPng(const char* filePath, const SBakeContext& header, const std::vector<uint8>& data, const IMapBaker& baker);
}