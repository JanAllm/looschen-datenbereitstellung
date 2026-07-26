#ifndef IC4_VERSION_H_INC_
#define IC4_VERSION_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Version.h"

#include "Error.h"

#include <string>

/**
 * @brief ic4 namespace
 */
namespace ic4
{

	/**
	 * @brief Contains retrievable version descriptions
	 */
	enum class VersionInfoFlags
	{
		Default = c_interface::IC4_VERSION_INFO_DEFAULT,    ///< Give the most useful information
		All = c_interface::IC4_VERSION_INFO_ALL,            ///< Give as many information as possible
		IC4 = c_interface::IC4_VERSION_INFO_IC4,            ///< Information about IC4 libraries
		Driver = c_interface::IC4_VERSION_INFO_DRIVER,      ///< Information about TIS GenTL provider
		Plugins = c_interface::IC4_VERSION_INFO_PLUGINS,    ///< Information about IC4 plugins, like GStreamer or OpenGL
	};

	/**
	 * @brief Retrieve IC4 version information
	 *
	 * @param[in] flags		What version information to retrieve
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return std::string - string containing version information about IC4
	 */
	inline std::string getVersionInfo(VersionInfoFlags flags = VersionInfoFlags::Default, Error& err = Error::Default())
	{
		size_t required_size = 2048;
		std::string result;
		result.resize(required_size);

		if (!ic4::c_interface::ic4_get_version_info(&result[0], &required_size, static_cast<c_interface::IC4_VERSION_INFO_FLAGS> (flags)))
		{
			result.resize(required_size + 1);
			if (!ic4::c_interface::ic4_get_version_info(&result[0], &required_size, static_cast<c_interface::IC4_VERSION_INFO_FLAGS> (flags)))
			{
				result.clear();
				return detail::updateFromLastErrorReturn(err, result);
			}
		}
		if (required_size > 0) {
			result.resize(required_size - 1);
		}
		return detail::clearReturn(err, result);
	}
}

#endif // IC4_VERSION_H_INC_
