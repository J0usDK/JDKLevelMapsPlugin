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
	};
}