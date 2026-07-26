
#ifndef IC4_INITLIBRARY_H_INC_
#define IC4_INITLIBRARY_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_InitLibrary.h"

#include "Error.h"

/**
 * @brief ic4 namespace
 */
namespace ic4
{
	/**
	 * @enum LogLevel
	 * @brief Specifies the possible library log levels
	 */
	enum class LogLevel
	{
		Off = c_interface::IC4_LOG_OFF,			///< Disable logging
		Error = c_interface::IC4_LOG_ERROR,		///< Log only errors
		Warning = c_interface::IC4_LOG_WARN,	///< Log warnings and above
		Info = c_interface::IC4_LOG_INFO,		///< Log info and above
		Debug = c_interface::IC4_LOG_DEBUG,		///< Log debug and above
		Trace = c_interface::IC4_LOG_TRACE		///< Log trace and above
	};

	/**
	 * @enum LogTarget
	 * @brief Defines the possible library log targets
	 */
	enum class LogTarget
	{
		StdOut = c_interface::IC4_LOGTARGET_STDOUT,		///< Log to stdout
		StdErr = c_interface::IC4_LOGTARGET_STDERR,		///< Log to stderr
		File = c_interface::IC4_LOGTARGET_FILE,			///< Log to a file
		WinDebug = c_interface::IC4_LOGTARGET_WINDEBUG,	///< Log using OutputDebugString (Windows only)
	};

	/**
	 * @brief The library initialization config structure
	 * 
	 * Passed to @ref initLibrary() when initializing the IC Imaging Control 4 C++ Class Library.
	 */
	struct InitLibraryConfig
	{
		/**
		 * @brief Selects the behavor of the default error handler #ic4::Error::Default(). See @ref technical_article_error_handling for details.
		 */
		ErrorHandlerBehavior defaultErrorHandlerBehavior = ErrorHandlerBehavior::Ignore;

		/**
		 * @brief Specifies the log level for the library API.
		 */
		LogLevel apiLogLevel = LogLevel::Off;

		/**
		 * @brief Specifies the log level for the internal library functions.
		 */
		LogLevel internalLogLevel = LogLevel::Off;

		/**
		 * @brief Selects the targets for logging.
		 *
		 * This is a bitwise combination of @ref LogTarget values.
		 */
		LogTarget logTargets = {};

		/**
		 * @brief Specifies the log file to use if @a logTargets has @ref LogTarget::File set.
		 */
		std::string logFilePath;

		/**
		 * @brief Reserved. Must be nullptr.
		 */
		uint64_t reserved0 = 0;
	};

	/**
	 * @brief This function must be used to initialize the library.
	 * @param[in] config	Configures global library settings such as default error handler and logging.
	 *
	 * @return @c true on success, otherwise @c false.
	*/
	inline bool	initLibrary(const InitLibraryConfig& config = {})
	{
		detail::configureDefaultErrorHandlerBehavior(config.defaultErrorHandlerBehavior);

		c_interface::IC4_INIT_CONFIG init_config =
		{
			(c_interface::IC4_LOG_LEVEL)config.apiLogLevel,						// .api_log_level =
			(c_interface::IC4_LOG_LEVEL)config.internalLogLevel,				// .library_log_level =
			(c_interface::IC4_LOG_TARGET_FLAGS)config.logTargets,				// .log_targets =
			config.logFilePath.empty() ? nullptr : config.logFilePath.c_str(),	// .log_file =
			config.reserved0,													// .reserved0 = 
		};

		return ic4::c_interface::ic4_init_library(&init_config);
	}

	/** This method should be called by the application, when the library should shut down.
	 *
	 * If this function is not called, the library may seem to leak several objects and bytes, because the dump running
	 * objects call may happen before the library has been told to unload itself.
	 *
	 * After calling exitLibrary no operation on the library is valid and so its effect is undefined.
	 */
	inline void	exitLibrary()
	{
		return ic4::c_interface::ic4_exit_library();
	}

#ifndef IC4_DOXYGEN_INVISIBLE
	inline size_t dbgCountObjects()
	{
		return ic4::c_interface::ic4_dbg_count_objects(nullptr);
	}
#endif

#ifndef IC4_DOXYGEN_INVISIBLE
	inline LogTarget operator|(LogTarget a, LogTarget b) noexcept
	{
		return static_cast<LogTarget>(static_cast<int>(a) | static_cast<int>(b));
	}
	inline LogTarget& operator|=(LogTarget& a, LogTarget b) noexcept
	{
		a = a | b;
		return a;
	}
#endif
}

#endif //IC4_INITLIBRARY_H_INC_
