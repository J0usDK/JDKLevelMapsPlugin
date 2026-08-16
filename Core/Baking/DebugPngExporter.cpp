#include "StdAfx.h"
#include "DebugPngExporter.h"

#include <QImage>

#include "LevelBakeContext.h"
#include "BakeProgress.h"
#include "IMapBaker.h"

bool JDKLevelMaps::Baking::ExportDebugPng(const char* filePath, const SBakeContext& header, const std::vector<uint8>& data, const IMapBaker& baker, std::shared_ptr<SBakeProgress> pProgress)
{
	const uint32 channels = baker.GetChannelCount();
	const size_t expectedSize = header.gridWidth * header.gridHeight * channels;
	if (header.gridWidth <= 0 || header.gridHeight <= 0 || data.size() != expectedSize)
		return false;
	
	QImage image(header.gridWidth, header.gridHeight, QImage::Format_RGB888);
	if (image.isNull())
	{
		CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_ERROR, "[JDKLevelMaps] Out of Memory: Failed to allocate QImage for debug export.");
	}

	for (int32 y = 0; y < header.gridHeight; ++y)
	{
		if (pProgress && (y & 0xF) == 0)
			pProgress->imageProgress.store((static_cast<float>(y) / header.gridHeight) * 0.1f);

		uchar* pLine = image.scanLine(y);
		const uint8* pRowData = data.data() + (y * header.gridWidth * channels);

		for (int32 x = 0; x < header.gridWidth; ++x)
		{
			const SDebugColor color = baker.GetDebugColor(&pRowData[x * channels]);
			uchar* pPixel = pLine + x * 3;
			pPixel[0] = color.r;
			pPixel[1] = color.g;
			pPixel[2] = color.b;
		}
	}

	bool saved = image.save(filePath, "PNG", 100);

	if (pProgress)
		pProgress->imageProgress.store(1.0f);

	return saved;
}