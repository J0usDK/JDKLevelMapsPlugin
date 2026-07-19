#include "StdAfx.h"
#include "PathResolver.h"

#include <Cry3DEngine/I3DEngine.h>

#include "../Formats/LayerMapHeader.h"

JDKLevelMaps::FileSystem::CPathResolver::CPathResolver()
{
	m_sDefaultPath = gEnv->p3DEngine->GetLevelFilePath("JDKLevelMaps");

	if (gEnv->pCryPak->MakeDir(m_sDefaultPath.c_str()))
		m_bInitialized = true;
	else
		CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_ERROR, ("Can't create plugin directory: " + m_sDefaultPath).c_str());

	m_sDefaultPath += "/";
}

const std::optional<std::string> JDKLevelMaps::FileSystem::CPathResolver::GetMapPath(const char* bakerId) const
{
	if (!m_bInitialized)
		return std::nullopt;

	return m_sDefaultPath + bakerId + m_sMapExtension;
}

const std::optional<std::string> JDKLevelMaps::FileSystem::CPathResolver::GetImagePath(const char* bakerId) const
{
	if (!m_bInitialized)
		return std::nullopt;

	return m_sDefaultPath + bakerId + m_sImageExtension;
}