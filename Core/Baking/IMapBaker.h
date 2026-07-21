#pragma once
#include <vector>
#include "Shared/LayerMapHeader.h"


namespace JDKLevelMaps::Baking
{
	struct SBakeContext;

	struct SDebugColor
	{
		uint8 r, g, b;
		SDebugColor() {}
		SDebugColor(uint8 r, uint8 g, uint8 b) : r(r), g(g), b(b) {}
	};

	class IMapBaker
	{
	public:
		virtual ~IMapBaker() = default;

		// Returns the baker's identificator
		virtual const char* GetId() const = 0;
		virtual ELayerMapType GetMapType() const = 0;
		virtual uint32 GetChannelCount() const { return 1; }

		virtual std::vector<uint8> Bake(const SBakeContext& context) = 0;

		// Returns color of the map's cell
		virtual SDebugColor GetDebugColor(const uint8* pCellData) const = 0;
	};
}