#include "StdAfx.h"
#include "VegetationBaker.h"

#include "../LevelBakeContext.h"
#include "EditorVegetationSource.h"

namespace
{
	constexpr uint8 kDensityPerInstance = 20;
}

std::vector<uint8> MapBakers::CVegetationBaker::Bake(const JDKLevelMaps::SBakeContext& context)
{
	const int32 totalCells = context.gridWidth * context.gridHeight;
	std::vector<uint8> mapData(totalCells, 0);

	auto objects = JDKLevelMaps::JDKEditorSource::QueryVegetationInstances(
		context.originX, context.originY,
		context.originX + (context.gridWidth * context.cellSize),
		context.originY + (context.gridHeight * context.cellSize));

	for (auto object : objects)
	{
		Vec3 pos = object.pos;
		int32 gridX = static_cast<int32>((pos.x - context.originX) / context.cellSize);
		int32 gridY = static_cast<int32>((pos.y - context.originY) / context.cellSize);

		if (gridX < 0 || gridX >= context.gridWidth || gridY < 0 || gridY >= context.gridHeight)
			continue;

		int32 index = (gridY * context.gridWidth) + gridX;

		mapData[index] = static_cast<uint8>(std::min(mapData[index] + kDensityPerInstance, 255));
	}

	return mapData;
}

JDKLevelMaps::SDebugColor MapBakers::CVegetationBaker::GetDebugColor(uint8 value) const
{
	if (value == 0)
		return { 100, 100, 100 };

	const float intensity = 50.0f + (value / 255.0f) * 205.0f;
	return { 0, static_cast<uint8>(intensity), 0};
}

const char* MapBakers::CVegetationBaker::GetId() const { return "VegetationDensity"; }
JDKLevelMaps::ELayerMapType MapBakers::CVegetationBaker::GetMapType() const { return JDKLevelMaps::ELayerMapType::VegetationDensity; }