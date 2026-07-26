
#ifndef IC4_C_BUFFERPOOL_H_INC_
#define IC4_C_BUFFERPOOL_H_INC_

#include "ic4core_export.h"

#include "C_ImageBuffer.h"
#include "C_ImageType.h"
#include "C_Allocator.h"

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
	 * @struct IC4_BUFFER_POOL
	 * @brief The buffer pool allows allocating additional image buffers for use by the program.
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_BUFFER_POOL*.
	 * 
	 * Most programs will only use image buffers provided by one of the sink types.
	 * However, some programs require additional buffers, for example to use as destination for image processing.
	 * 
	 * To create additional buffers, first create a buffer pool using #ic4_bufferpool_create().
	 * Then, use #ic4_bufferpool_get_buffer() to create a new image buffer with a specified image type.
	 * Allocation options can be specified to customizer image buffer's memory alignment, pitch and total buffer size.
	 * 
	 * When the image buffer is no longer required, call #ic4_imagebuffer_unref on it. The buffer will then be returned to the buffer pool.
	 * 
	 * The buffer pool has configurable caching behavior. By default, the buffer pool will cache one image buffer and return it the next
	 * time a matching image buffer is requested.
	 * 
	 * Buffer pool objects are reference-counted, and created with an initial reference count of one.
	 * To share a buffer pool object between multiple parts of a program, create a new reference by calling #ic4_bufferpool_ref().
	 * When a reference is no longer required, call #ic4_bufferpool_unref().
	 *
	 * If the buffer pool object's internal reference count reaches zero, the buffer pool object is destroyed.
	 * Even after that, image buffers created by the buffer pool are still valid until they are released by calling #ic4_imagebuffer_unref.
	 */
	struct IC4_BUFFER_POOL;

	/**
	 * @brief Configures the behavior of a #IC4_BUFFER_POOL.
	 */
	struct IC4_BUFFER_POOL_CONFIG
	{
		/**
		 * @brief Maximum number of frames to keep in the buffer pool's cache
		 */
		size_t cache_frames_max;
		/**
		 * @brief Maximum size of the buffer pool cache in bytes, or @c 0 to not limit by size
		 */
		size_t cache_bytes_max;

		/**
		 * @brief A structure containing function pointers to customize the buffer pool's allocator.
		 *
		 * This parameter is optional, set all callback functions to \c NULL to use the default allocator.
		 *
		 * If @ref IC4_ALLOCATOR_CALLBACKS::allocate_buffer is set, @ref IC4_ALLOCATOR_CALLBACKS::free_buffer must be set as well.
		 */
		struct IC4_ALLOCATOR_CALLBACKS allocator;
		/**
		 * @brief A user-defined value that is passed to the allocator callbacks
		 *
		 * If \c callback_context points to a memory location, and callback functions access that memory,
		 * the program has to make sure that the memory is valid until the @ref IC4_ALLOCATOR_CALLBACKS::release callback is executed.
		 */
		void* allocator_context;
	};

	/**
	 * @brief Creates a new buffer pool.
	 * 
	 * @param[in] ppPool	Pointer to a buffer pool handle to receive the new buffer pool.
	 *						When the buffer pool is no longer required, release the object reference using #ic4_bufferpool_unref().
	 * @param[in] config	Pointer to a structure containing the buffer pool configuration
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_bufferpool_create(struct IC4_BUFFER_POOL** ppPool, const struct IC4_BUFFER_POOL_CONFIG* config);
	/**
	 * @brief Increases the buffer pool's internal reference count by one.
	 *
	 * @param[in] pPool A pointer to a buffer pool
	 *
	 * @return The pointer passed via \a pPool
	 *
	 * @remarks If \a pPool is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_bufferpool_unref
	 */
	IC4CORE_API struct IC4_BUFFER_POOL* ic4_bufferpool_ref(struct IC4_BUFFER_POOL* pPool);
	/**
	 * @brief Decreases the pool's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pPool A pointer to a buffer pool
	 *
	 * @remarks
	 * If \a pPool is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_bufferpool_ref
	 */
	IC4CORE_API void ic4_bufferpool_unref(struct IC4_BUFFER_POOL* pPool);

	/**
	 * @brief Contains options to configure the allocation when requesting an image buffer from a buffer pool.
	 * 
	 * @see ic4_bufferpool_get_buffer
	 */
	struct IC4_BUFFERPOOL_ALLOCATION_OPTIONS
	{
		/**
		 * @brief Specifies the alignment of the address of the buffer's memory.
		 * 
		 * Setting this to 0 lets the buffer pool select an alignment automatically.
		 * 
		 * The alignment must be a power of 2.
		 */
		size_t alignment;

		/**
		 * @brief Specifies the pitch to use when allocating the buffer.
		 * 
		 * A value of 0 lets the buffer pool select a pitch automatically.
		 * 
		 * Setting a pitch that is smaller than the amount of memory required to store one line of image data will lead to an error.
		 */
		ptrdiff_t pitch;

		/**
		 * @brief Overrides the automatic buffer size calculation.
		 * 
		 * A value of 0 lets the buffer pool calculate the required buffer size automatically.
		 * 
		 * Setting a size that is smaller than the amount of memory required to store an image of a known format will lead to an error.
		 */
		size_t buffer_size;
	};

	/**
	 * @brief Gets a buffer from the buffer pool.
	 * 
	 * The buffer is either newly allocated, or retrieved from the buffer pool's buffer cache.
	 * 
	 * @param[in] pPool				A buffer pool
	 * @param[in] image_type		Image type of the requested buffer
	 * @param[in] options			A pointer to a #IC4_BUFFERPOOL_ALLOCATION_OPTIONS structure specifying advance allocation options.\n
	 *								May be @c NULL to use default allocation parameters.
	 * @param[out] ppImageBuffer	Pointer to an image buffer handle to receive the buffer.\n
	 *								When the buffer is no longer required, release the object reference using #ic4_imagebuffer_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_bufferpool_get_buffer(struct IC4_BUFFER_POOL* pPool, const struct IC4_IMAGE_TYPE* image_type, const struct IC4_BUFFERPOOL_ALLOCATION_OPTIONS* options, struct IC4_IMAGE_BUFFER** ppImageBuffer);

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

#endif //IC4_C_BUFFERPOOL_H_INC_