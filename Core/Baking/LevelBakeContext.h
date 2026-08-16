#pragma once
#include <cmath>
#include <CryCore/BaseTypes.h>
#include <Cry3DEngine/I3DEngine.h>

namespace JDKLevelMaps::Baking
{
	struct SBakeContext
	{
		uint32 tileSize;
		int32 gridWidth;
		int32 gridHeight;
		float cellSize;
		float originX;
		float originY;
	};

	inline int GetLevelTerrainSize()
	{
		return gEnv->p3DEngine->GetTerrainSize();
	}

	inline SBakeContext ComputeLevelBakeContext(float cellSize, uint32 tileSize)
	{
		CRY_ASSERT(cellSize > 0.0f);
		if (cellSize < 0.1f)
			cellSize = 0.1f;
		if (tileSize == 0)
			tileSize = 1;

		int terrainSize = GetLevelTerrainSize();
		int32 gridSize = static_cast<int32>(std::round(terrainSize / cellSize));

		SBakeContext context;
		context.cellSize = cellSize;
		context.tileSize = tileSize;
		context.originX = 0.0f;
		context.originY = 0.0f;
		context.gridWidth = gridSize;
		context.gridHeight = gridSize;

		return context;
	}
}