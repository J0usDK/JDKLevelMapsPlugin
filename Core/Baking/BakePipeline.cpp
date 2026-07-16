#include "StdAfx.h"
#include "BakePipeline.h"

#include "IMapBaker.h"
#include "LevelBakeContext.h"
#include "BakeRunResult.h"
#include "../Formats/LayerMapHeader.h"
#include "DebugPngExporter.h"

JDKLevelMaps::SBakeRunResult JDKLevelMaps::CBakePipeline::BakeMap(IMapBaker& pBaker, const JDKLevelMaps::SBakeContext& context)
{
	std::vector<uint8> bakedData = pBaker.Bake(context);

	SPathResult pathResult = PrepareAndGetPath((std::string(pBaker.GetId()) + ".jdkm").c_str());
	if (!pathResult.success)
		return { false, pathResult.str };

	SBakeRunResult mapResult = WriteToFile(pBaker, context, pathResult.str.c_str(), bakedData);
	if (!mapResult.success)
		return mapResult;

	pathResult = PrepareAndGetPath((std::string(pBaker.GetId()) + ".png").c_str());
	if (!pathResult.success)
		return { false, pathResult.str };

	bool imageResult = JDKLevelMaps::ExportDebugPng(pathResult.str.c_str(), context, bakedData, pBaker);
	return { imageResult, imageResult ? "" : "Error saving debug image" };
}

JDKLevelMaps::SBakeRunResult JDKLevelMaps::CBakePipeline::WriteToFile (
	const IMapBaker& pBaker,
	const JDKLevelMaps::SBakeContext& context,
	const char* path,
	const std::vector<uint8>& bakedData)
{
	SBakeRunResult result;

	JDKLevelMaps::SLayerMapHeader header;
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

JDKLevelMaps::CBakePipeline::SPathResult JDKLevelMaps::CBakePipeline::PrepareAndGetPath(const char* filename)
{
	if (!filename)
		return { false, "Missing file name" };

	SPathResult result;

	result.str = gEnv->p3DEngine->GetLevelFilePath("JDKLevelMaps");
	if (!gEnv->pCryPak->MakeDir(result.str.c_str()))
		return { false, "Cannot create output directory: " + result.str };

	result.str += "/" + std::string(filename);
	result.success = true;
	return result;
}