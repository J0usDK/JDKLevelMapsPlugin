start_sources()

sources_platform(ALL)
add_sources("NoUberFile"
	SOURCE_GROUP "Root"
		"JDKLevelMapsPlugin.cpp"
		"JDKLevelMapsPlugin.h"
		"StdAfx.h"
		"StdAfx.cpp"
)

end_sources()

CryEditorPlugin(JDKLevelMapsPlugin PCH "StdAfx.cpp" SOLUTION_FOLDER "Sandbox/Plugins")

