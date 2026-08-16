#pragma once
#include <CryCore/BaseTypes.h>

#if defined(__has_include)
	#if __has_include(<CrySystem/File/JDKPakLFS.h>)
		#include <CrySystem/File/JDKPakLFS.h>
	#endif
#endif

namespace JDKLevelMaps::FileSystem
{
	class CLFSFacade
	{
	public:
		static constexpr uint64 GetMaxFileSize()
		{
#if defined(JDK_CRYPAK_LFS_PATCH)
			return JDK_MAX_PAK_FILE_SIZE;
#else
			return 2147483648ull;
#endif
		}

		static constexpr bool IsPatched()
		{
#if defined(JDK_CRYPAK_LFS_PATCH)
			return true;
#else
			return false;
#endif
		}

		static uint64 FTell(FILE* pFile)
		{
			if (!pFile || !gEnv || !gEnv->pCryPak)
				return 0;

#if defined(JDK_CRYPAK_LFS_PATCH)
			return static_cast<uint64>(gEnv->pCryPak->FTell64(pFile));
#else
			return static_cast<uint64>(gEnv->pCryPak->FTell(pFile))
#endif
		}

		static size_t FSeek(FILE* pFile, uint64 offset, int mode)
		{
			if (!pFile || !gEnv || !gEnv->pCryPak)
				return 0;

#if defined(JDK_CRYPAK_LFS_PATCH)
			return gEnv->pCryPak->FSeek64(pFile, static_cast<int64>(offset), mode);
#else
			return gEnv->pCryPak->FSeek(pFile, static_cast<long>(offset), mode);
#endif
		}
	};
}