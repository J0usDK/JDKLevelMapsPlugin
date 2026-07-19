#pragma once
#include <string>
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps::Settings
{
	struct SVegetationBakerSettings
	{
		uint8 densityPerInstance = 20;

		std::string grassGroupName = "grass";
		std::string bushGroupName = "bushes";
		std::string treeGroupName = "trees";

		bool enableGrass = true;
		bool enableBush = true;
		bool enableTree = true;
	};
}