#include "StdAfx.h"
#include "BakePipeline.h"

#include <future>

#include "IMapBaker.h"
#include "LevelBakeContext.h"

#include "BakeProgress.h"
#include "BakeRunResult.h"
#include "DebugPngExporter.h"
#include "../FileSystem/PathResolver.h"
#include "../LFS/LFSFacade.h"

namespace
{
	struct ScopedCryFile
	{
		FILE* p = nullptr;
		
		ScopedCryFile(FILE* pFile) : p(pFile) {}
		~ScopedCryFile() { close(); }

		ScopedCryFile(const ScopedCryFile&) = delete;
		ScopedCryFile& operator=(const ScopedCryFile&) = delete;

		operator FILE* () const { return p; }

		void close()
		{
			if (p)
			{
				gEnv->pCryPak->FClose(p);
				p = nullptr;
			}
		}
	};

	inline bool PixelHasData(const std::vector<uint8>& flatData, size_t pixelIndex, uint32 numChannels)
	{
		for (uint32 c = 0; c < numChannels; ++c)
		{
			if (flatData[pixelIndex + c] > 0)
				return true;
		}
		return false;
	}

	void ExtractTileDataHelper(
		const std::vector<uint8>& flatData,
		uint32 tileX, uint32 tileY,
		const JDKLevelMaps::Baking::SBakeContext& context,
		uint32 numChannels,
		std::vector<uint8>& outTileBuffer)
	{
		uint32 maxLy = std::min(context.tileSize, context.gridHeight - (tileY * context.tileSize));

		const size_t bytesPerRow = static_cast<size_t>(maxLy) * numChannels;
		const size_t localStride = static_cast<size_t>(context.tileSize) * numChannels;
		const size_t globalStride = static_cast<size_t>(context.gridWidth) * numChannels;

		size_t globalStart = ((static_cast<size_t>(tileY) * context.tileSize * context.gridWidth) + (tileX * context.tileSize)) * numChannels;
		size_t localStart = 0;

		for (uint32 ly = 0; ly < maxLy; ++ly)
		{
			std::memcpy(&outTileBuffer[localStart], &flatData[globalStart], bytesPerRow);
			globalStart += globalStride;
			localStart += localStride;
		}
	}

	template<typename TTileEntry>
	JDKLevelMaps::Baking::SBakeRunResult WriteTiles(
		ScopedCryFile& pFile,
		const char* path,
		JDKLevelMaps::SMapHeader& header,
		const JDKLevelMaps::Baking::SBakeContext& context,
		const std::vector<uint8>& bakedData,
		uint32 numChannels,
		uint32 tileCountX,
		uint32 tileCountY,
		const std::vector<uint8>& tileHasData,
		std::shared_ptr<JDKLevelMaps::Baking::SBakeProgress> pProgress)
	{
		if (pProgress)
			pProgress->currentStage.store(JDKLevelMaps::Baking::EBakeStage::WritingTiles);

		JDKLevelMaps::Baking::SBakeRunResult result;
		auto HandleIOError = [&pFile, path, &result](const std::string& errorMsg) -> JDKLevelMaps::Baking::SBakeRunResult
		{
			pFile.close();
			gEnv->pCryPak->RemoveFile(path);
			result.success = false;
			result.message = errorMsg;
			return result;
		};

		if (gEnv->pCryPak->FWrite(&header, sizeof(JDKLevelMaps::SMapHeader), 1, pFile) != 1)
			return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot write map's header");

		size_t totalTiles = static_cast<size_t>(tileCountX) * tileCountY;
		std::vector<TTileEntry> directory;

		try
		{
			directory.resize(totalTiles);
		}
		catch (const std::bad_alloc&)
		{
			return HandleIOError("[JDKLevelMaps] Out of Memory: Failed to allocate memory for the tile directory metadata");
		}

		uint64 directoryOffset = JDKLevelMaps::FileSystem::CLFSFacade::FTell(pFile);
		if (gEnv->pCryPak->FWrite(directory.data(), sizeof(TTileEntry), directory.size(), pFile) != directory.size())
			return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot reserve directory space (Out of disk space?)");

		std::vector<uint8> tileBuffer;
		try
		{
			tileBuffer.resize(static_cast<size_t>(context.tileSize) * context.tileSize * numChannels);
		}
		catch (const std::bad_alloc&)
		{
			return HandleIOError("[JDKLevelMaps] Out of Memory: Failed to allocate memory for tile buffer during file writing.");
		}

		uint32 tx = 0;
		uint32 ty = 0;

		for (size_t tileIndex = 0; tileIndex < totalTiles; ++tileIndex)
		{
			if (pProgress && (tileIndex & 0xFF) == 0)
				pProgress->writeProgress.store(0.2f + (static_cast<float>(tileIndex) / totalTiles) * 0.6f);

			if (tileHasData[tileIndex])
			{
				std::fill(tileBuffer.begin(), tileBuffer.end(), 0);

				ExtractTileDataHelper(bakedData, tx, ty, context, numChannels, tileBuffer);
				uint64 currentOffset = JDKLevelMaps::FileSystem::CLFSFacade::FTell(pFile);

				directory[tileIndex].fileOffset = static_cast<decltype(TTileEntry::fileOffset)>(currentOffset);
				directory[tileIndex].byteSize = static_cast<decltype(TTileEntry::byteSize)>(tileBuffer.size());

				if (gEnv->pCryPak->FWrite(tileBuffer.data(), 1, tileBuffer.size(), pFile) != tileBuffer.size())
					return HandleIOError("[JDKLevelMaps] Disk I/O Error: Failed to write tile data at index: " + std::to_string(tileIndex));
			}
			else
			{
				directory[tileIndex].fileOffset = 0;
				directory[tileIndex].byteSize = 0;
			}

			if (++tx == tileCountX)
			{
				tx = 0;
				++ty;
			}
		}

		if (pProgress)
			pProgress->writeProgress.store(1.0f);

		JDKLevelMaps::FileSystem::CLFSFacade::FSeek(pFile, directoryOffset, SEEK_SET);

		const size_t chunkSize = (1024 * 1024) / sizeof(TTileEntry);
		size_t totalWritten = 0;
		const size_t totalElements = directory.size();

		while (totalWritten < totalElements)
		{
			size_t elementsToWrite = std::min(chunkSize, totalElements - totalWritten);
			if (gEnv->pCryPak->FWrite(directory.data() + totalWritten, sizeof(TTileEntry), elementsToWrite, pFile) != elementsToWrite)
				return HandleIOError("[JDKLevelMaps] Disk I/O Error: Cannot finilize directory metadata");

			totalWritten += elementsToWrite;
			if (pProgress)
			{
				float flushProgress = static_cast<float>(totalWritten) / totalElements;
				pProgress->writeProgress.store(0.8f + (flushProgress * 0.2f));
			}
		}

		pFile.close();

		if (pProgress)
			pProgress->writeProgress.store(1.0f);

		return { true, "Saved to " + std::string(path) };
	}
}

JDKLevelMaps::Baking::CBakePipeline::CBakePipeline(FileSystem::CPathResolver* pPathResolver)
{
	m_pPathResolver = pPathResolver;
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakePipeline::BakeMap(IMapBaker& pBaker, const SBakeContext& context, std::shared_ptr<SBakeProgress> pProgress)
{
	const uint32 numChannels = pBaker.GetChannelCount();
	const uint32 tileCountX = (static_cast<uint32>(context.gridWidth) + context.tileSize - 1) / context.tileSize;
	const uint32 tileCountY = (static_cast<uint32>(context.gridHeight) + context.tileSize - 1) / context.tileSize;
	const uint64 totalTiles = static_cast<uint64>(tileCountX) * tileCountY;

	const uint64 headerSize = sizeof(SMapHeader);
	const uint64 directorySize = totalTiles * sizeof(STileEntry64);
	const uint64 maxTileDataSize = totalTiles * static_cast<uint64>(context.tileSize) * context.tileSize * numChannels;

	const uint64 totalSize = headerSize + directorySize + maxTileDataSize;
	if (totalSize > FileSystem::CLFSFacade::GetMaxFileSize())
		return { false, "The size of map will exceed max file's size" };

	std::vector<uint8> bakedData;
	try
	{
		if (pProgress)
			pProgress->currentStage.store(EBakeStage::ExtractingData);
		bakedData = pBaker.Bake(context);
	}
	catch (const std::bad_alloc&)
	{
		return { false, "[JDKLevelMaps] Out of Memory: Failed to allocate memory for the map." };
	}

	std::string mapPath;
	if (auto resultPath = m_pPathResolver->GetMapPath(pBaker.GetId()))
		mapPath = resultPath.value();
	else
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot get map's path" };

	std::string imagePath;
	if (auto resultPath = m_pPathResolver->GetImagePath(pBaker.GetId()))
		imagePath = resultPath.value();
	else
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot get image's path" };

	std::future<SBakeRunResult> writeTask = std::async(std::launch::async, [&]()
	{
		return WriteToFile(pBaker, context, mapPath.c_str(), bakedData, pProgress);
	});

	std::future<bool> imageTask = std::async(std::launch::async, [&]()
	{
		return ExportDebugPng(imagePath.c_str(), context, bakedData, pBaker, pProgress);
	});


	SBakeRunResult mapResult = writeTask.get();
	bool imageResult = imageTask.get();
	if (!mapResult.success)
		return mapResult;
	return { imageResult, imageResult ? "" : "[JDKLevelMaps] Disk I/O Error: Error during saving debug image" };
}

JDKLevelMaps::Baking::SBakeRunResult JDKLevelMaps::Baking::CBakePipeline::WriteToFile (
	const IMapBaker& pBaker,
	const SBakeContext& context,
	const char* path,
	const std::vector<uint8>& bakedData,
	std::shared_ptr<SBakeProgress> pProgress) const
{
	const uint32 numChannels = pBaker.GetChannelCount();
	const uint32 tileCountX = (static_cast<uint32>(context.gridWidth) + context.tileSize - 1) / context.tileSize;
	const uint32 tileCountY = (static_cast<uint32>(context.gridHeight) + context.tileSize - 1) / context.tileSize;
	const uint64 totalTiles = static_cast<uint64>(tileCountX) * tileCountY;

	if (pProgress)
		pProgress->currentStage.store(EBakeStage::PrePassAnalysis);

	std::vector<uint8> tileHasData(totalTiles, 0);
	uint64 nonEmptyTileCount = 0;

	// Pre pass to compute real file size
	for (int32 y = 0; y < context.gridHeight; ++y)
	{
		if (pProgress)
			pProgress->writeProgress.store((static_cast<float>(y) / context.gridHeight) * 0.2f);

		uint32 ty = y / context.tileSize;
		size_t rowTileOffset = static_cast<size_t>(ty) * tileCountX;
		size_t globalRowIndex = static_cast<size_t>(y) * context.gridWidth * numChannels;

		uint tx = 0;
		uint32 xInTile = 0;

		for (int32 x = 0; x < context.gridWidth; ++x)
		{
			size_t tileIndex = rowTileOffset + tx;

			if (!tileHasData[tileIndex] && PixelHasData(bakedData, globalRowIndex + (x * numChannels), numChannels))
			{
				tileHasData[tileIndex] = 1;
				nonEmptyTileCount++;
			}

			if (++xInTile == context.tileSize)
			{
				tx++;
				xInTile = 0;
			}
		}
	}

	const uint64 fullTileByteSize = static_cast<uint64>(context.tileSize) * context.tileSize * numChannels;
	const uint64 actualSize32 = sizeof(SMapHeader) +
		(totalTiles * sizeof(STileEntry32)) +
		(nonEmptyTileCount * fullTileByteSize);

	bool useCompact = (actualSize32 <= 4294967295ULL);
	SMapHeader header;
	header.mapType = pBaker.GetMapType();
	header.entryFormat = useCompact ? ETileEntryFormat::Compact_32 : ETileEntryFormat::Standard_64;
	header.gridWidth = context.gridWidth;
	header.gridHeight = context.gridHeight;
	header.cellSize = context.cellSize;
	header.originX = context.originX;
	header.originY = context.originY;
	header.tileSize = context.tileSize;
	header.tileCountX = tileCountX;
	header.tileCountY = tileCountY;

	ScopedCryFile pFile(gEnv->pCryPak->FOpen(path, "wb"));
	if (!pFile)
		return { false, "[JDKLevelMaps] Disk I/O Error: Cannot open map file for writting" };

	if (useCompact)
		return WriteTiles<STileEntry32>(pFile, path, header, context, bakedData, numChannels, tileCountX, tileCountY, tileHasData, pProgress);
	else
		return WriteTiles<STileEntry64>(pFile, path, header, context, bakedData, numChannels, tileCountX, tileCountY, tileHasData, pProgress);
}