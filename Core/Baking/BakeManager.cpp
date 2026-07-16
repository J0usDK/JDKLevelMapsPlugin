#include "StdAfx.h"
#include "BakeManager.h"

#include <algorithm>
#include <cstring>

#include "BakePipeline.h"
#include "Vegetation/VegetationBaker.h"
#include "LevelBakeContext.h"
#include "BakeRunResult.h"

JDKLevelMaps::CBakeManager::CBakeManager()
{
	m_pBakePipeline = std::make_unique<CBakePipeline>();
	m_bakers.reserve(1);
	m_bakers.push_back(std::make_unique<MapBakers::CVegetationBaker>());
}

JDKLevelMaps::CBakeManager::~CBakeManager() = default;

std::vector<const JDKLevelMaps::IMapBaker*> JDKLevelMaps::CBakeManager::GetAvaliableBakers() const
{
	std::vector<const JDKLevelMaps::IMapBaker*> outBakers;
	outBakers.reserve(m_bakers.size());

	for (const auto& pBaker : m_bakers)
		outBakers.push_back(pBaker.get());

	return outBakers;
}

JDKLevelMaps::SBakeRunResult JDKLevelMaps::CBakeManager::RunBake(JDKLevelMaps::ELayerMapType mapType, float cellSize)
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

	SBakeContext context = JDKLevelMaps::ComputeLevelBakeContext(cellSize);
	return m_pBakePipeline->BakeMap(*it->get(), context);
}