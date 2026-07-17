#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_Editor
#undef  RWI_NAME_TAG
#define RWI_NAME_TAG "RayWorldIntersection(JDKLevelMaps)"
#undef  PWI_NAME_TAG
#define PWI_NAME_TAG "PrimitiveWorldIntersection(JDKLevelMaps)"
#include <CryCore/Platform/platform.h>

#define CRY_USE_XT
#define CRY_USE_ATL
#define CRY_SUPPRESS_CRYENGINE_WINDOWS_FUNCTION_RENAMING
#include <CryCore/Platform/CryAtlMfc.h>

#include <CryCore/Project/ProjectDefines.h>

#include "IPlugin.h"
#include "EditorDefs.h"

// Do not remove - linker error (comment from EditorQt::StdAfx.h)
#include "Util/Variable.h"