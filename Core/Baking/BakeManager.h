#pragma once
#include <vector>
#include <memory>

namespace JDKLevelMaps
{
	enum class ELayerMapType : uint16;
}

namespace JDKLevelMaps::FileSystem
{
	class CPathResolver;
}

namespace JDKLevelMaps::Baking
{
	class IMapBaker;
	class CBakePipeline;
	struct SBakeRunResult;

	class CBakeManager
	{
	public:
		CBakeManager(FileSystem::CPathResolver* pPathResolver);
		~CBakeManager();

		std::vector<const IMapBaker*> GetAvaliableBakers() const;
		const IMapBaker* GetBaker(ELayerMapType bakerType) const;

		SBakeRunResult RunBake(ELayerMapType mapType, float cellSize);

	private:
		FileSystem::CPathResolver* m_pPathResolver = nullptr;
		std::unique_ptr<CBakePipeline> m_pBakePipeline = nullptr;

		std::vector<std::unique_ptr<IMapBaker>> m_bakers;
	};
}