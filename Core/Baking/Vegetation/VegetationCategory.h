#pragma once
#include <string>
#include <CryCore/BaseTypes.h>

#include "../../../Settings/VegetationBakerSettings.h"

namespace JDKLevelMaps::Categories::Vegetation
{
	enum class EVegetationCategory : uint8
	{
		Unknown = 0,
		Grass,
		Bush,
		Tree,

		Count
	};

	inline EVegetationCategory ClassifyGroup(const char* groupName, const Settings::SVegetationBakerSettings* pSettings)
	{
		if (!groupName)
			return EVegetationCategory::Unknown;

		if (strcmp(groupName, pSettings->grassGroupName.c_str()) == 0)
			return EVegetationCategory::Grass;
		else if (strcmp(groupName, pSettings->bushGroupName.c_str()) == 0)
			return EVegetationCategory::Bush;
		else if (strcmp(groupName, pSettings->treeGroupName.c_str()) == 0)
			return EVegetationCategory::Tree;

		return EVegetationCategory::Unknown;
	}
}