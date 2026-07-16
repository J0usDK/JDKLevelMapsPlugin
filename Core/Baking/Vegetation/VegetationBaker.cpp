#include "StdAfx.h"
#include "VegetationBaker.h"

#include <Cry3DEngine/I3DEngine.h>
#include <Cry3DEngine/IRenderNode.h>
#include <Cry3DEngine/ISurfaceType.h>

#include "../LevelBakeContext.h"

namespace
{
	constexpr uint8 kDensityPerInstance = 20;
}

std::vector<uint8> MapBakers::CVegetationBaker::Bake(const JDKLevelMaps::SBakeContext& context)
{
	const int32 totalCells = context.gridWidth * context.gridHeight;
	std::vector<uint8> mapData(totalCells, 0);

	AABB bakeBox;
	bakeBox.min = Vec3(context.originX, context.originY, -8000.0f);
	bakeBox.max = Vec3(context.originX + (context.gridWidth * context.cellSize), context.originY + (context.gridHeight * context.cellSize), 8000.0f);

	uint32 objectsCount = gEnv->p3DEngine->GetObjectsByTypeInBox(eERType_Vegetation, bakeBox, nullptr);
	if (objectsCount == 0)
		return mapData;

	std::vector<IRenderNode*> objects(objectsCount);
	gEnv->p3DEngine->GetObjectsByTypeInBox(eERType_Vegetation, bakeBox, objects.data());

	for (auto pObject : objects)
	{
		for (int i = 0; i < pObject->GetMaterial()->GetSubMtlCount(); ++i)
			CryLogAlways(pObject->GetMaterial()->GetSubMtl(i)->GetSurfaceType()->GetName());
		CryLogAlways("========================================");

		Vec3 pos = pObject->GetPos();
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