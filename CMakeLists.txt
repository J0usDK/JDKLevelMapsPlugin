start_sources()

sources_platform(ALL)

add_sources("NoUberFile"
	SOURCE_GROUP "Root"
		"JDKLevelMapsPlugin.cpp"
		"JDKLevelMapsPlugin.h"
		"StdAfx.h"
		"StdAfx.cpp"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core\\\\Baking"
		"Core/Baking/BakeManager.h"
		"Core/Baking/BakeManager.cpp"
		"Core/Baking/BakePipeline.h"
		"Core/Baking/BakePipeline.cpp"
		"Core/Baking/BakeProgress.h"
		"Core/Baking/BakeRunResult.h"
		"Core/Baking/DebugPngExporter.h"
		"Core/Baking/DebugPngExporter.cpp"
		"Core/Baking/IMapBaker.h"
		"Core/Baking/LevelBakeContext.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core\\\\Baking\\\\Vegetation"
		"Core/Baking/Vegetation/EditorVegetationSource.h"
		"Core/Baking/Vegetation/EditorVegetationSource.cpp"
		"Core/Baking/Vegetation/VegetationBaker.h"
		"Core/Baking/Vegetation/VegetationBaker.cpp"
		"Core/Baking/Vegetation/VegetationCategory.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core\\\\FileSystem"
		"Core/FileSystem/PathResolver.h"
		"Core/FileSystem/PathResolver.cpp"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core\\\\LFS"
		"Core/LFS/LFSFacade.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Editor"
		"Editor/JDKLevelMapsEditor.h"
		"Editor/JDKLevelMapsEditor.cpp"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Editor\\\\Components"
		"Editor/Components/MapPreview.h"
		"Editor/Components/MapPreview.cpp"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Settings"
		"Settings/BakerSettings.h"
		"Settings/VegetationBakerSettings.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Shared"
		"Shared/MapHeader.h"
		"Shared/MapLayers.h"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Utils"
		"Utils/ConvertUtils.h"
)

end_sources()

CryEditorPlugin(JDKLevelMapsPlugin PCH "StdAfx.cpp" SOLUTION_FOLDER "Sandbox/Plugins")
target_include_directories(${THIS_PROJECT} PRIVATE ".")

target_link_libraries(${THIS_PROJECT} PRIVATE Sandbox)
target_include_directories(${THIS_PROJECT} PRIVATE "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
use_xt()