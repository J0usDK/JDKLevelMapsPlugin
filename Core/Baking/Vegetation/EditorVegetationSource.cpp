#include "StdAfx.h"
#include "EditorVegetationSource.h"

#include "VegetationCategory.h"

#include "IEditorImpl.h"
#include "Vegetation/VegetationMap.h"
#include "Vegetation/VegetationObject.h"

namespace JDKLevelMaps::JDKEditorSource
{
	std::vector<SVegetationInstanceData> QueryVegetationInstances(float x1, float y1, float x2, float y2, const Settings::SVegetationBakerSettings* pSettings)
	{
		std::vector<SVegetationInstanceData> result;

		CEditorImpl* pEditorImpl = static_cast<CEditorImpl*>(GetIEditor());
		if (!pEditorImpl)
			return result;

		CVegetationMap* pVegetationMap = pEditorImpl->GetVegetationMap();
		if (!pVegetationMap)
			return result;

		std::vector<CVegetationInstance*> instances;
		pVegetationMap->GetObjectInstances(x1, y1, x2, y2, instances);

		result.reserve(instances.size());
		for (auto pInstance : instances)
		{
			if (!pInstance || !pInstance->object)
				continue;

			MapLayers::EVegetationLayers layer = Categories::Vegetation::ClassifyGroup(pInstance->object->GetGroup(), pSettings);

			if (layer != MapLayers::EVegetationLayers::Unknown)
				result.emplace_back(pInstance->pos, layer);
		}
		return result;
	}
}