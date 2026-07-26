
#ifndef IC4_C_INITLIBRARY_H_INC_
#define IC4_C_INITLIBRARY_H_INC_

#include "ic4core_export.h"

#include <stddef.h>

/**
 * @defgroup library Core Library Functions
 *
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#ifdef IC4_C_IN_NAMESPACE
namespace ic4
{
namespace c_interface
{
#endif
#endif

	/**
	 * @brief Defines the possible library log levels
	 */
	enum IC4_LOG_LEVEL
	{
		IC4_LOG_OFF,	///< Disable logging
		IC4_LOG_ERROR,	///< Log only errors
		IC4_LOG_WARN,	///< Log warnings and above
		IC4_LOG_INFO,	///< Log info and above
		IC4_LOG_DEBUG,	///< Log debug and above
		IC4_LOG_TRACE	///< Log trace and above
	};

	/**
	 * @brief Defines the possible log targets
	 */
	enum IC4_LOG_TARGET_FLAGS
	{
		IC4_LOGTARGET_DISABLE	= 0,	///< Disable logging
		IC4_LOGTARGET_STDOUT	= 1,	///< Log to stdout
		IC4_LOGTARGET_STDERR	= 2,	///< Log to stderr
		IC4_LOGTARGET_FILE		= 4,	///< Log to a file specified by @ref IC4_INIT_CONFIG::log_file
		IC4_LOGTARGET_WINDEBUG	= 8,	///< Log using @c OutputDebugString (Windows only)
	};

	/**
	 * @brief The library initialization config structure
	 * 
	 * Passed to @ref ic4_init_library when initializing the IC Imaging Control 4 C Library.
	 */
	struct IC4_INIT_CONFIG
	{
		/**
		 * @brief Specifies the library API log level.
		 * 
		 * This log level controls whether to log errors for failed library API function calls.
		 */
		enum IC4_LOG_LEVEL api_log_level;

		/**
		 * @brief Specifies the internal library log level.
		 * 
		 * This log level controls internal library logging.
		 */
		enum IC4_LOG_LEVEL internal_log_level;

		/**
		 * @brief Selects the targets for logging.
		 * 
		 * This is a bitwise combination of @ref IC4_LOG_TARGET_FLAGS values.
		 */
		enum IC4_LOG_TARGET_FLAGS log_targets;

		/**
		 * @brief Specifies the log file to use if @a log_targets has @ref IC4_LOGTARGET_FILE set.
		 */
		const char* log_file;

		/**
		 * @brief Reserved. Must be 0.
		 */
		uint64_t reserved0;
	};

	/**
	 * @brief Initializes the IC Imaging Control 4 C Library
	 * 
	 * ic4_init_library must be called before any other library function.
	 * 
	 * @param init_config A structure configuring library settings, e.g. the log level.
	 * 
	 * @return @c true on success, otherwise @c false.
	 */
	IC4CORE_API bool ic4_init_library(const struct IC4_INIT_CONFIG* init_config);
	
	/**
	 * @brief Un-initializes the library.
	 * 
	 * Every successful call to @c ic4_init_library should be balanced by a matching call to @a ic4_exit_library before unloading the library DLL.
	 */
	IC4CORE_API void ic4_exit_library();

#ifndef IC4_DOXYGEN_INVISIBLE
	IC4CORE_API size_t ic4_dbg_count_objects(const char* type_name);
#endif

#ifdef __cplusplus
#ifdef IC4_C_IN_NAMESPACE
}
}
#endif
}
#endif

/**
 * @}
 */

#endif // IC4_C_INITLIBRARY_H_INC_