#pragma once
#include <CryCore/BaseTypes.h>
#include <Cry3DEngine/I3DEngine.h>

namespace JDKLevelMaps
{
	struct SBakeContext
	{
		int32 gridWidth;
		int32 gridHeight;
		float cellSize;
		float originX;
		float originY;
	};

	inline SBakeContext ComputeLevelBakeContext(float cellSize)
	{
		CRY_ASSERT(cellSize > 0.0f);
		if (cellSize <= 0.0f)
			cellSize = 1.0f;

		SBakeContext context;
		context.cellSize = cellSize;
		context.originX = 0.0f;
		context.originY = 0.0f;

		int terrainSize = gEnv->p3DEngine->GetTerrainSize();
		int32 gridSize = static_cast<int32>(terrainSize / cellSize);
		context.gridWidth = gridSize;
		context.gridHeight = gridSize;

		return context;
	}
}