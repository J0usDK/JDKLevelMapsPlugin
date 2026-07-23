#pragma once
#include <vector>
#include <memory>

namespace JDKLevelMaps
{
	enum class EMapType : uint16;
}

namespace JDKLevelMaps::Settings
{
	struct SBakerSettings;
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
		CBakeManager(FileSystem::CPathResolver* pPathResolver, const Settings::SBakerSettings* pBakerSettings);
		~CBakeManager();

		std::vector<const IMapBaker*> GetAvaliableBakers() const;
		const IMapBaker* GetBaker(EMapType bakerType) const;

		SBakeRunResult RunBake(EMapType mapType);

	private:
		const Settings::SBakerSettings* m_pBakerSettings = nullptr;
		FileSystem::CPathResolver* m_pPathResolver = nullptr;
		std::unique_ptr<CBakePipeline> m_pBakePipeline = nullptr;

		std::vector<std::unique_ptr<IMapBaker>> m_bakers;
	};
}