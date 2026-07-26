
#ifndef IC4_C_ALLOCATOR_H_INC_
#define IC4_C_ALLOCATOR_H_INC_

#include "ic4core_export.h"

#include "C_ImageBuffer.h"
#include "C_ImageType.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup imgbuf Image Buffers
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
	 * Contains function pointers used to configure a custom allocator for image buffers
	 */
	struct IC4_ALLOCATOR_CALLBACKS
	{
		/**
		 * @brief Notifies the user that the allocator will not receive any additional callback function calls.
		 *
		 * @anchor allocator_release
		 *
		 * Any resources attached to the \c context parameter can be released.
		 *
		 * @param[in] context   The \c allocator_context parameter that was passed to the sink creation function.
		 *
		 * @note
		 * The \c release callback function is executed on the thread that destroys the sink using the final call to #ic4_sink_unref().
		 */
		void (*release)(void* context);

		/**
		 * @brief Requests the allocator to allocate a memory buffer.
		 *
		 * @param[in] context       The \c allocator_context parameter that was passed to the sink creation function
		 * @param[in] buffer_size   The size of the requested memory buffer
		 * @param[in] alignment     Requests memory to be allocated on a specific alignment boundary.\n
		 *                          This value is always a power of 2.
		 * @param[out] buffer_ptr   A pointer to a pointer that receives the address of the newly-allocated buffer
		 * @param[out] user_data    User data to attach to the buffer. The user data is passed to @ref free_buffer when
		 *                          the allocated memory is freed again.
		 *
		 * @return \c true on success. If the allocation could not be performed, return \c false.
		 */
		bool (*allocate_buffer)(void* context, size_t buffer_size, size_t alignment, void** buffer_ptr, void** user_data);

		/**
		 * @brief Requests the allocator to free a previously-allocated memory buffer.
		 *
		 * @param[in] context       The \c allocator_context parameter that was passed to the sink creation function
		 * @param[in] buffer_ptr    Pointer to the memory buffer to be freed
		 * @param[in] user_data     The user data that was returned from @ref allocate_buffer when the memory buffer was allocated
		 */
		void (*free_buffer)(void* context, void* buffer_ptr, void* user_data);
	};
	
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

#endif //IC4_C_ALLOCATOR_H_INC_