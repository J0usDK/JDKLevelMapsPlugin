#pragma once
#include "VegetationBakerSettings.h"

namespace JDKLevelMaps::Settings
{
	struct SBakerSettings
	{
		float cellSize = 1.0f;
		uint16 tileSize = 64;
		SVegetationBakerSettings vegSettings;
	};
}