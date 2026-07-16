#pragma once
#include <vector>
#include <memory>

namespace JDKLevelMaps
{
	class IMapBaker;
	class CBakePipeline;
	enum class ELayerMapType : uint16;
	struct SBakeRunResult;

	class CBakeManager
	{
	public:
		CBakeManager();
		~CBakeManager();

		std::vector<const IMapBaker*> GetAvaliableBakers() const;

		SBakeRunResult RunBake(ELayerMapType mapType, float cellSize);

	private:
		std::unique_ptr<CBakePipeline> m_pBakePipeline;

		std::vector<std::unique_ptr<IMapBaker>> m_bakers;
	};
}