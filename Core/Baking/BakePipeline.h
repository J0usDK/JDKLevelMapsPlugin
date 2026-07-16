#pragma once
#include <vector>
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps
{
	class IMapBaker;
	struct SBakeContext;
	struct SBakeRunResult;

	class CBakePipeline
	{
	public:
		SBakeRunResult BakeMap(IMapBaker& pBaker, const JDKLevelMaps::SBakeContext& context);

	private:
		// If success contains path in the str field, otherwise the field contains error message
		struct SPathResult
		{
			bool success = false;
			std::string str;
		};

	private:
		SPathResult PrepareAndGetPath(const char* filename);
		SBakeRunResult WriteToFile(const IMapBaker& pBaker, const JDKLevelMaps::SBakeContext& context, const char* path, const std::vector<uint8>& bakedData);
	};
}