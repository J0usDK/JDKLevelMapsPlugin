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
	SOURCE_GROUP "Editor"
		"Editor/JDKLevelMapsEditor.h"
		"Editor/JDKLevelMapsEditor.cpp"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Core"
		"Core/Formats/LayerMapHeader.h"
		"Core/Baking/LevelBakeContext.h"
		"Core/Baking/BakeRunResult.h"
		"Core/Baking/IMapBaker.h"
		"Core/Baking/Vegetation/VegetationBaker.h"
		"Core/Baking/Vegetation/VegetationBaker.cpp"
		"Core/Baking/Vegetation/EditorVegetationSource.h"
		"Core/Baking/Vegetation/EditorVegetationSource.cpp"
		"Core/Baking/BakeManager.h"
		"Core/Baking/BakeManager.cpp"
		"Core/Baking/BakePipeline.h"
		"Core/Baking/BakePipeline.cpp"
		"Core/Baking/DebugPngExporter.h"
		"Core/Baking/DebugPngExporter.cpp"
)

end_sources()

CryEditorPlugin(JDKLevelMapsPlugin PCH "StdAfx.cpp" SOLUTION_FOLDER "Sandbox/Plugins")
target_include_directories(${THIS_PROJECT} PRIVATE ".")

target_link_libraries(${THIS_PROJECT} PRIVATE Sandbox)
target_include_directories(${THIS_PROJECT} PRIVATE "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
use_xt()