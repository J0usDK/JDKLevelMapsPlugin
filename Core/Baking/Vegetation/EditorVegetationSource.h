#pragma once
#include <vector>
#include <string>
#include <CryMath/Cry_Vector3.h>

#include "Shared/MapLayers.h"

namespace JDKLevelMaps::Settings
{
	struct SVegetationBakerSettings;
}

namespace JDKLevelMaps::JDKEditorSource
{
	struct SVegetationInstanceData
	{
		Vec3 pos;
		MapLayers::EVegetationLayers layer;

		SVegetationInstanceData() : layer(MapLayers::EVegetationLayers::Unknown) {}
		SVegetationInstanceData(Vec3 pos, MapLayers::EVegetationLayers layer) : pos(pos), layer(layer) {}
	};

	std::vector<SVegetationInstanceData> QueryVegetationInstances(float x1, float y1, float x2, float y2, const Settings::SVegetationBakerSettings* pSettings);
}