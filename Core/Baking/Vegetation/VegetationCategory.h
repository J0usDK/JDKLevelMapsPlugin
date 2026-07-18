#pragma once
#include <string>
#include <CryCore/BaseTypes.h>

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

	inline EVegetationCategory ClassifyGroup(const char* groupName)
	{
		if (!groupName)
			return EVegetationCategory::Unknown;

		if (strcmp(groupName, "grass") == 0)
			return EVegetationCategory::Grass;
		else if (strcmp(groupName, "bushes") == 0)
			return EVegetationCategory::Bush;
		else if (strcmp(groupName, "trees") == 0)
			return EVegetationCategory::Tree;

		return EVegetationCategory::Unknown;
	}
}