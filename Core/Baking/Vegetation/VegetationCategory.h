#pragma once
#include <string>
#include <CryCore/BaseTypes.h>

#include "Settings/VegetationBakerSettings.h"
#include "Shared/MapLayers.h"

namespace JDKLevelMaps::Categories::Vegetation
{
	inline MapLayers::EVegetationLayers ClassifyGroup(const char* groupName, const Settings::SVegetationBakerSettings* pSettings)
	{
		if (!groupName)
			return MapLayers::EVegetationLayers::Unknown;

		if (strcmp(groupName, pSettings->grassGroupName.c_str()) == 0)
			return MapLayers::EVegetationLayers::Grass;
		else if (strcmp(groupName, pSettings->bushGroupName.c_str()) == 0)
			return MapLayers::EVegetationLayers::Bush;
		else if (strcmp(groupName, pSettings->treeGroupName.c_str()) == 0)
			return MapLayers::EVegetationLayers::Tree;

		return MapLayers::EVegetationLayers::Unknown;
	}
}