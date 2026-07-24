#include "StdAfx.h"
#include "BakePipeline.h"

#include "IMapBaker.h"
#include "LevelBakeContext.h"

#include "BakeRunResult.h"
#include "DebugPngExporter.h"
#include "../FileSystem/PathResolver.h"

JDKLevelMaps::Baking::CBakePipeline::CBakePipeline(FileSystem::CPathResolver* pPathResolver)
{
	m_pPathResolver = pPathResolver;
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakePipeline::BakeMap(IMapBaker& pBaker, const SBakeContext& context)
{
	std::vector<uint8> bakedData = pBaker.Bake(context);
	std::string path;

	if (auto resultPath = m_pPathResolver->GetMapPath(pBaker.GetId()))
		path = resultPath.value();
	else
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot get map's path" };

	SBakeRunResult mapResult = WriteToFile(pBaker, context, path.c_str(), bakedData);
	if (!mapResult.success)
		return mapResult;

	if (auto resultPath = m_pPathResolver->GetImagePath(pBaker.GetId()))
		path = resultPath.value();
	else
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot get image's path" };

	bool imageResult = ExportDebugPng(path.c_str(), context, bakedData, pBaker);
	return { imageResult, imageResult ? "" : "[JDKLevelMaps] Disk I/O Error: Error during saving debug image" };
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakePipeline::WriteToFile (
	const IMapBaker& pBaker,
	const SBakeContext& context,
	const char* path,
	const std::vector<uint8>& bakedData) const
{
	SBakeRunResult result;
	const uint32 numChannels = pBaker.GetChannelCount();
	const uint32 tileCountX = (context.gridWidth + context.tileSize - 1) / context.tileSize;
	const uint32 tileCountY = (context.gridHeight + context.tileSize - 1) / context.tileSize;

	auto pFile = gEnv->pCryPak->FOpen(path, "wb");
	if (!pFile)
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot open map file for writting" };

	auto HandleIOError = [&pFile, &path, &result](const std::string& errorMsg) -> SBakeRunResult
	{
		gEnv->pCryPak->FClose(pFile);
		gEnv->pCryPak->RemoveFile(path);
		result.success = false;
		result.message = errorMsg;
		return result;
	};

	SMapHeader header;
	header.mapType = pBaker.GetMapType();
	header.gridWidth = context.gridWidth;
	header.gridHeight = context.gridHeight;
	header.cellSize = context.cellSize;
	header.originX = context.originX;
	header.originY = context.originY;
	header.tileSize = context.tileSize;
	header.tileCountX = tileCountX;
	header.tileCountY = tileCountY;

	if (gEnv->pCryPak->FWrite(&header, sizeof(SMapHeader), 1, pFile) != 1)
		return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot write map's header");

	std::vector<STileEntry> directory(tileCountX * tileCountY);
	size_t directoryOffset = gEnv->pCryPak->FTell(pFile);

	if (gEnv->pCryPak->FWrite(directory.data(), sizeof(STileEntry), directory.size(), pFile) != directory.size())
		return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot reserve directory space (Out of disk space?)");

	for (uint32 ty = 0; ty < tileCountY; ++ty)
	{
		for (uint32 tx = 0; tx < tileCountX; ++tx)
		{
			uint32 tileIndex = ty * tileCountX + tx;
			std::vector<uint8> tileBuffer(context.tileSize * context.tileSize * numChannels);

			bool isEmpty = ExtractTileData(bakedData, tx, ty, context, numChannels, tileBuffer);
			if (!isEmpty)
			{
				directory[tileIndex].fileOffset = gEnv->pCryPak->FTell(pFile);
				directory[tileIndex].byteSize = tileBuffer.size();
				directory[tileIndex].flags = 0;

				if (gEnv->pCryPak->FWrite(tileBuffer.data(), 1, tileBuffer.size(), pFile) != tileBuffer.size())
					return HandleIOError("[JDKLevelMaps] Disk I/O Error: Failed to write tile data at index: " + std::to_string(tileIndex));
			}
			else
				directory[tileIndex] = { 0, 0, 1 };
		}
	}

	gEnv->pCryPak->FSeek(pFile, directoryOffset, SEEK_SET);

	if (gEnv->pCryPak->FWrite(directory.data(), sizeof(STileEntry), directory.size(), pFile) != directory.size())
		return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot finilize directory metadata");

	gEnv->pCryPak->FClose(pFile);

	return { true, "Saved to " + std::string(path) };
}

bool JDKLevelMaps::Baking::CBakePipeline::ExtractTileData(
	const std::vector<uint8>& flatData,
	uint32 tileX, uint32 tileY,
	const SBakeContext& context,
	uint32 numChannels,
	std::vector<uint8>& outTileBuffer) const
{
	uint32 maxLy = std::min(context.tileSize, context.gridHeight - (tileY * context.tileSize));
	uint32 maxLx = std::min(context.tileSize, context.gridWidth - (tileX * context.tileSize));
	bool isEmpty = true;

	for (uint32 ly = 0; ly < maxLy; ++ly)
	{
		for (uint32 lx = 0; lx < maxLx; ++lx)
		{
			uint32 globalX = (tileX * context.tileSize) + lx;
			uint32 globalY = (tileY * context.tileSize) + ly;
			uint32 globalIndex = ((globalY * context.gridWidth) + globalX) * numChannels;
			uint32 localIndex = ((ly * context.tileSize) + lx) * numChannels;

			for (uint32 c = 0; c < numChannels; ++c)
			{
				uint8 val = flatData[globalIndex + c];
				outTileBuffer[localIndex + c] = val;
				if (val > 0)
					isEmpty = false;
			}
		}
	}
	return isEmpty;
}