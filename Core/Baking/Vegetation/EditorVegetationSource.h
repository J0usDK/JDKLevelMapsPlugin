#pragma once
#include <vector>
#include <string>
#include <CryMath/Cry_Vector3.h>

namespace JDKLevelMaps::JDKEditorSource
{
	struct SVegetationInstanceData
	{
		Vec3 pos;
		std::string group;

		SVegetationInstanceData() {}
		SVegetationInstanceData(Vec3 pos, const char* group) : pos(pos), group(group) {}
	};

	std::vector<SVegetationInstanceData> QueryVegetationInstances(float x1, float y1, float x2, float y2);
}