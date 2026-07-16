#include "StdAfx.h"
#include "DebugPngExporter.h"

#include <QImage>

#include "LevelBakeContext.h"
#include "IMapBaker.h"

bool JDKLevelMaps::ExportDebugPng(const char* filePath, const SBakeContext& header, const std::vector<uint8>& data, const IMapBaker& baker)
{
	const size_t expectedSize = header.gridWidth * header.gridHeight;
	if (header.gridWidth <= 0 || header.gridHeight <= 0 || data.size() != expectedSize)
		return false;
	
	QImage image(header.gridWidth, header.gridHeight, QImage::Format_RGB888);

	for (int32 y = 0; y < header.gridHeight; ++y)
	{
		uchar* pLine = image.scanLine(y);
		const uint8* pRowData = data.data() + y * header.gridWidth;

		for (int32 x = 0; x < header.gridWidth; ++x)
		{
			const SDebugColor color = baker.GetDebugColor(pRowData[x]);
			uchar* pPixel = pLine + x * 3;
			pPixel[0] = color.r;
			pPixel[1] = color.g;
			pPixel[2] = color.b;
		}
	}

	return image.save(filePath, "PNG", 100);
}