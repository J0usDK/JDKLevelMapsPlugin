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

end_sources()

CryEditorPlugin(JDKLevelMapsPlugin PCH "StdAfx.cpp" SOLUTION_FOLDER "Sandbox/Plugins")
target_include_directories(${THIS_PROJECT} PRIVATE ".")
