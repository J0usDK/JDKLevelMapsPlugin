#pragma once
#include "../IMapBaker.h"
#include "VegetationCategory.h"

namespace JDKLevelMaps::Settings
{
	struct SVegetationBakerSettings;
}

namespace JDKLevelMaps::MapBakers
{
	struct SBakeContext;

	class CVegetationBaker final : public JDKLevelMaps::Baking::IMapBaker
	{
	public:
		CVegetationBaker(const Settings::SVegetationBakerSettings* pSettings);

		const char* GetId() const override;
		JDKLevelMaps::EMapType GetMapType() const override;
		uint32 GetChannelCount() const override;

		std::vector<uint8> Bake(const JDKLevelMaps::Baking::SBakeContext& context) override;

		JDKLevelMaps::Baking::SDebugColor GetDebugColor(const uint8* pCellData) const override;

	private:
		int32 ResolveGroup(MapLayers::EVegetationLayers group) const;

	private:
		const Settings::SVegetationBakerSettings* m_pSettings = nullptr;
	};
}