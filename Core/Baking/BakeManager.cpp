#include "StdAfx.h"
#include "BakeManager.h"

#include <algorithm>
#include <cstring>

#include "BakePipeline.h"
#include "Vegetation/VegetationBaker.h"
#include "LevelBakeContext.h"
#include "BakeRunResult.h"
#include "../FileSystem/PathResolver.h"
#include "../../Settings/BakerSettings.h"

JDKLevelMaps::Baking::CBakeManager::CBakeManager(FileSystem::CPathResolver* pPathResolver, const Settings::SBakerSettings* pBakerSettings)
{
	m_pPathResolver = pPathResolver;
	m_pBakerSettings = pBakerSettings;
	m_pBakePipeline = std::make_unique<CBakePipeline>(pPathResolver);

	m_bakers.reserve(1);
	m_bakers.push_back(std::make_unique<MapBakers::CVegetationBaker>(&m_pBakerSettings->vegSettings));
}

JDKLevelMaps::Baking::CBakeManager::~CBakeManager() = default;

std::vector<const JDKLevelMaps::Baking::IMapBaker*> JDKLevelMaps::Baking::CBakeManager::GetAvaliableBakers() const
{
	std::vector<const IMapBaker*> outBakers;
	outBakers.reserve(m_bakers.size());

	for (const auto& pBaker : m_bakers)
		outBakers.push_back(pBaker.get());

	return outBakers;
}

const JDKLevelMaps::Baking::IMapBaker* JDKLevelMaps::Baking::CBakeManager::GetBaker(EMapType bakerType) const
{
	auto it = std::find_if(m_bakers.begin(), m_bakers.end(), [bakerType](const auto& pBaker) {
		return pBaker->GetMapType() == bakerType;
	});

	return it != m_bakers.end() ? it->get() : nullptr;
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakeManager::RunBake(EMapType mapType)
{
	auto it = std::find_if(m_bakers.begin(), m_bakers.end(), [mapType](const auto& pBaker) {
		return pBaker->GetMapType() == mapType;
	});
	
	if (it == m_bakers.end())
	{
		SBakeRunResult result;
		result.message = "Baker not found";
		return result;
	}

	SBakeContext context = JDKLevelMaps::Baking::ComputeLevelBakeContext(m_pBakerSettings->cellSize);
	return m_pBakePipeline->BakeMap(*it->get(), context);
}