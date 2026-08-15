#include "StdAfx.h"
#include "VegetationBaker.h"

#include "../LevelBakeContext.h"
#include "EditorVegetationSource.h"
#include "../../../Settings/VegetationBakerSettings.h"

JDKLevelMaps::MapBakers::CVegetationBaker::CVegetationBaker(const Settings::SVegetationBakerSettings* pSettings)
{
	m_pSettings = pSettings;
}

std::vector<uint8> JDKLevelMaps::MapBakers::CVegetationBaker::Bake(const JDKLevelMaps::Baking::SBakeContext& context)
{
	const size_t numChannels = GetChannelCount();
	const size_t totalCells = static_cast<size_t>(context.gridWidth) * static_cast<size_t>(context.gridHeight);
	std::vector<uint8> mapData(totalCells * numChannels, 0);

	auto objects = JDKLevelMaps::JDKEditorSource::QueryVegetationInstances(
		context.originX, context.originY,
		context.originX + (context.gridWidth * context.cellSize),
		context.originY + (context.gridHeight * context.cellSize),
		m_pSettings);

	for (auto object : objects)
	{
		float relX = (object.pos.x - context.originX) / context.cellSize;
		float relY = (object.pos.y - context.originY) / context.cellSize;

		if (relX < 0.0f || relX >= static_cast<float>(context.gridWidth) ||
			relY < 0.0f || relY >= static_cast<float>(context.gridHeight))
			continue;

		int32 gridX = static_cast<int32>(relX);
		int32 gridY = static_cast<int32>(relY);

		if (gridX < 0 || gridX >= context.gridWidth || gridY < 0 || gridY >= context.gridHeight)
			continue;

		int32 channelOffset = ResolveGroup(object.layer);
		if (channelOffset < 0)
			continue;

		size_t index = ((static_cast<size_t>(gridY) * context.gridWidth + gridX) * numChannels) + channelOffset;
		mapData[index] = static_cast<uint8>(std::min(mapData[index] + m_pSettings->densityPerInstance, 255));
	}

	return mapData;
}

JDKLevelMaps::Baking::SDebugColor JDKLevelMaps::MapBakers::CVegetationBaker::GetDebugColor(const uint8* pCellData) const
{
	if (!pCellData)
		return { 0, 0, 0 };

	uint8 r = pCellData[0];
	uint8 g = pCellData[1];
	uint8 b = pCellData[2];

	auto calcIntensity = [](uint8 val) -> uint8 {
		if (val == 0) return 0;
		return static_cast<uint8>(50.0f + (val / 255.0f) * 205.0f);
	};
	return { calcIntensity(r), calcIntensity(g), calcIntensity(b) };
}

int32 JDKLevelMaps::MapBakers::CVegetationBaker::ResolveGroup(MapLayers::EVegetationLayers group) const
{
	switch (group)
	{
		case MapLayers::EVegetationLayers::Tree:
			return m_pSettings->enableTree ? JDKLevelMaps::MapLayers::ToChannelIndex(group) : -1;
		case MapLayers::EVegetationLayers::Grass:
			return m_pSettings->enableGrass ? JDKLevelMaps::MapLayers::ToChannelIndex(group) : -1;
		case MapLayers::EVegetationLayers::Bush:
			return m_pSettings->enableBush ? JDKLevelMaps::MapLayers::ToChannelIndex(group) : -1;
		default:
			return -1;
	}
}

const char* JDKLevelMaps::MapBakers::CVegetationBaker::GetId() const { return "VegetationDensity"; }
JDKLevelMaps::EMapType JDKLevelMaps::MapBakers::CVegetationBaker::GetMapType() const { return JDKLevelMaps::EMapType::VegetationDensity; }
uint32 JDKLevelMaps::MapBakers::CVegetationBaker::GetChannelCount() const { return JDKLevelMaps::MapLayers::kVegetationChannelCount; }