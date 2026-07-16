#pragma once
#include "../IMapBaker.h"

namespace MapBakers
{
	struct SBakeContext;

	class CVegetationBaker final : public JDKLevelMaps::IMapBaker
	{
	public:
		const char* GetId() const override;
		JDKLevelMaps::ELayerMapType GetMapType() const override;

		std::vector<uint8> Bake(const JDKLevelMaps::SBakeContext& context) override;

		JDKLevelMaps::SDebugColor GetDebugColor(uint8 value) const override;
	};
}