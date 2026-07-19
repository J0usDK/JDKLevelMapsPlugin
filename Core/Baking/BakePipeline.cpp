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
		return SBakeRunResult(false, "Cannot get map's path");

	SBakeRunResult mapResult = WriteToFile(pBaker, context, path.c_str(), bakedData);
	if (!mapResult.success)
		return mapResult;

	if (auto resultPath = m_pPathResolver->GetImagePath(pBaker.GetId()))
		path = resultPath.value();
	else
		return SBakeRunResult(false, "Cannot get image's path");

	bool imageResult = ExportDebugPng(path.c_str(), context, bakedData, pBaker);
	return { imageResult, imageResult ? "" : "Error saving debug image" };
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakePipeline::WriteToFile (
	const IMapBaker& pBaker,
	const SBakeContext& context,
	const char* path,
	const std::vector<uint8>& bakedData)
{
	SBakeRunResult result;

	SLayerMapHeader header;
	header.mapType = pBaker.GetMapType();
	header.gridWidth = context.gridWidth;
	header.gridHeight = context.gridHeight;
	header.cellSize = context.cellSize;
	header.originX = context.originX;
	header.originY = context.originY;

	auto pFile = gEnv->pCryPak->FOpen(path, "wb");
	if (pFile == NULL)
	{
		result.message = "Can't open map";
		return result;
	}

	size_t writtenSize = 0;
	writtenSize = gEnv->pCryPak->FWrite(&header, sizeof(SLayerMapHeader), 1, pFile);
	if (writtenSize != 1)
	{
		result.message = "Can't write map's header";
		gEnv->pCryPak->FClose(pFile);
		gEnv->pCryPak->RemoveFile(path);
		return result;
	}

	writtenSize = 0;
	writtenSize = gEnv->pCryPak->FWrite(bakedData.data(), 1, bakedData.size(), pFile);
	if (writtenSize != bakedData.size())
	{
		result.message = "Can't write map's data";
		gEnv->pCryPak->FClose(pFile);
		gEnv->pCryPak->RemoveFile(path);
		return result;
	}

	result.success = true;
	result.message = "Saved to " + std::string(path);
	gEnv->pCryPak->FClose(pFile);
	return result;
}