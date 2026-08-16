#pragma once
#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps
{
	constexpr uint8 kLayerMapVersion = 3;
	constexpr uint32 kLayerMapMagic = 'JDKM';

	enum class EMapType : uint8
	{
		VegetationDensity = 0
	};

	enum class ETileEntryFormat : uint8
	{
		Compact_32 = 0,	// offset: uint32, size: uint32 (8 bytes)
		Standard_64 = 1	// offset: uint64, size: uint64 (16 bytes)
	};

#pragma pack(push, 1)
	struct STileEntry32
	{
		uint32 fileOffset = 0;
		uint32 byteSize = 0;
	};

	struct STileEntry64
	{
		uint64 fileOffset = 0;
		uint64 byteSize = 0;
	};

	struct SMapHeader
	{
		uint32 magic = kLayerMapMagic;
		uint8 version = kLayerMapVersion;
		EMapType mapType = EMapType::VegetationDensity;
		ETileEntryFormat entryFormat = ETileEntryFormat::Compact_32;
		uint8 reserved = 0; // currently not used, added for alignment

		int32 gridWidth = 0;
		int32 gridHeight = 0;
		float cellSize = 0.0f;	//meters per cell
		float originX = 0.0f;
		float originY = 0.0f;

		uint32 tileSize = 0;	//size in cells
		uint32 tileCountX = 0;
		uint32 tileCountY = 0;
	};
#pragma pack(pop)

	static_assert(sizeof(STileEntry32) == 8,
		"The size of STileEntry32 has been changed. Keep the in-game reader up to date.");

	static_assert(sizeof(STileEntry64) == 16,
		"The size of STileEntry32 has been changed. Keep the in-game reader up to date.");

	static_assert(sizeof(SMapHeader) == 40,
		"The size of SMapHeader has been changed. Keep the in-game reader up to date.");
}