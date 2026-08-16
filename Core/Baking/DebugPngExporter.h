#pragma once
#include <string>
#include <vector>
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps::Baking
{
	class IMapBaker;
	struct SBakeContext;
	struct SBakeProgress;

	bool ExportDebugPng(const char* filePath, const SBakeContext& header, const std::vector<uint8>& data, const IMapBaker& baker, std::shared_ptr<SBakeProgress> pProgress = nullptr);
}