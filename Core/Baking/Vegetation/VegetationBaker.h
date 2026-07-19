#pragma once
#include "../IMapBaker.h"
#include "VegetationCategory.h"

namespace JDKLevelMaps::MapBakers
{
	struct SBakeContext;

	class CVegetationBaker final : public JDKLevelMaps::Baking::IMapBaker
	{
	public:
		const char* GetId() const override;
		JDKLevelMaps::ELayerMapType GetMapType() const override;
		uint32 GetChannelCount() const override;

		std::vector<uint8> Bake(const JDKLevelMaps::Baking::SBakeContext& context) override;

		JDKLevelMaps::Baking::SDebugColor GetDebugColor(const uint8* pCellData) const override;

	private:
		int32 ResolveGroup(JDKLevelMaps::Categories::Vegetation::EVegetationCategory group) const;
	};
}