
#ifndef IC4_C_VERSION_H_INC_
#define IC4_C_VERSION_H_INC_

#pragma once

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
     * @brief Contains retrievable version descriptions
     */
    enum IC4_VERSION_INFO_FLAGS
    {
        IC4_VERSION_INFO_DEFAULT = 0x0,     ///< Give the most useful information
        IC4_VERSION_INFO_ALL = 0x1,         ///< Give as much information as possible
        IC4_VERSION_INFO_IC4 = 0x2,         ///< Information about IC4 core libraries
        IC4_VERSION_INFO_DRIVER = 0x4,      ///< Information about TIS GenTL providers
        IC4_VERSION_INFO_PLUGINS = 0x8,     ///< Information about IC4 plugins
    };

    /**
     * @brief Retrieve version information description string
     *
     * @param[out] str         Pointer to a character array to receive an error message.
     * @param[in,out] size     Size of str buffer
     * @param[in] flags        What version information to retrieve
     *
     * @return \c true on success
     */
    IC4CORE_API bool ic4_get_version_info(char* str, size_t* size, enum IC4_VERSION_INFO_FLAGS flags);

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

#endif // IC4_C_VERSION_H_INC_
