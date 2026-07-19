#pragma once
#include <string>
#include <optional>

namespace JDKLevelMaps::FileSystem
{
	class CPathResolver
	{
	public:
		CPathResolver();
		~CPathResolver() = default;

		const std::optional<std::string> GetImagePath(const char* bakerId) const;
		const std::optional<std::string> GetMapPath(const char* bakerId) const;

	private:
		std::string m_sDefaultPath = "";
		bool m_bInitialized = false;

		const std::string m_sMapExtension = ".jdkm";
		const std::string m_sImageExtension = ".png";
	};
}