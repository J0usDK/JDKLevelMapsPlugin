#pragma once
#include <vector>
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps::FileSystem
{
	class CPathResolver;
}

namespace JDKLevelMaps::Baking
{
	class IMapBaker;
	struct SBakeContext;
	struct SBakeRunResult;

	class CBakePipeline
	{
	public:
		CBakePipeline(JDKLevelMaps::FileSystem::CPathResolver* pPathResolver);
		~CBakePipeline() = default;

		SBakeRunResult BakeMap(IMapBaker& pBaker, const SBakeContext& context);

	private:
		// If success contains path in the str field, otherwise the field contains error message
		struct SPathResult
		{
			bool success = false;
			std::string str;
		};

	private:
		// Returns the bool [true = tile is not empty; false = tile is empty]
		bool ExtractTileData(const std::vector<uint8>& flatData, uint32 tileX, uint32 tileY, const SBakeContext& context, uint32 numChannels, std::vector<uint8>& outTileBuffer) const;
		
		SBakeRunResult WriteToFile(const IMapBaker& pBaker, const SBakeContext& context, const char* path, const std::vector<uint8>& bakedData) const;

	private:
		FileSystem::CPathResolver* m_pPathResolver = nullptr;
	};
}