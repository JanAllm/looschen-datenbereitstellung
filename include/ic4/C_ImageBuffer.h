
#ifndef IC4_C_FRAME_H_INC_
#define IC4_C_FRAME_H_INC_

#include "ic4core_export.h"

#include "C_ImageType.h"

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
	 * @struct IC4_IMAGE_BUFFER
	 * @brief Represents an image buffer
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_IMAGE_BUFFER*.
	 * 
	 * Image buffer objects are created automatically by the various @ref sink types,
	 * on request by a @ref IC4_BUFFER_POOL, or by calling @ref ic4_imagebuffer_wrap_memory().
	 * 
	 * Image buffer objects are reference-counted.
	 * To share an image buffer object between multiple parts of a program, use ic4_imagebuffer_ref().
	 * Call ic4_imagebuffer_unref() when a reference is no longer required.
	 * 
	 * If the reference count reaches zero, the image buffer is returned to its source to be reused. For example,
	 * an image buffer retrieved from ic4_queuesink_pop_output_buffer() will be re-queued.
	 */
	struct IC4_IMAGE_BUFFER;

	/**
	 * @brief Increases the image buffer's internal reference count by one.
	 *
	 * @param[in] pImageBuffer An image buffer
	 *
	 * @return The pointer passed via \a pImageBuffer
	 *
	 * @remarks If \a pImageBuffer is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_imagebuffer_unref
	 */
	IC4CORE_API struct IC4_IMAGE_BUFFER* ic4_imagebuffer_ref(struct IC4_IMAGE_BUFFER* pImageBuffer);

	/**
	 * @brief Decreases the image buffer's internal reference count by one.
	 *
	 * If the reference count reaches zero, the image buffer is returned to its source to be reused.
	 *
	 * @param[in] pImageBuffer An image buffer
	 *
	 * @remarks If \a pImageBuffer is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_imagebuffer_ref
	 */
	IC4CORE_API void ic4_imagebuffer_unref(struct IC4_IMAGE_BUFFER* pImageBuffer);

	/**
	 * @brief Returns a pointer to the data managed by the image buffer.
	 *
	 * @param[in] pImageBuffer An image buffer
	 *
	 * @return A pointer to the image data contained in the image buffer,\n
	 *		   or \c NULL if an error occurred. Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks The memory pointed to by the returned pointer is valid as long as the image buffer object exists.
	 */
	IC4CORE_API void* ic4_imagebuffer_get_ptr(const struct IC4_IMAGE_BUFFER* pImageBuffer);

	/**
	 * @brief Returns the pitch for the image buffer.
	 * 
	 * The pitch is the distance between the starting memory location of two consecutive lines.
	 * 
	 * @param[in] pImageBuffer	An image buffer
	 * 
	 * @return	The pitch for this image buffer, or \c 0 if an error occurred.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 */
	IC4CORE_API ptrdiff_t ic4_imagebuffer_get_pitch(const struct IC4_IMAGE_BUFFER* pImageBuffer);

	/**
	 * @brief Returns the size of the image buffer.
	 * 
	 * @param[in] pImageBuffer	An image buffer
	 * 
	 * @return	The size of this image buffer, or \c 0 if an error occurred.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 */
	IC4CORE_API size_t ic4_imagebuffer_get_buffer_size(const struct IC4_IMAGE_BUFFER* pImageBuffer);

	/**
	 * @brief Queries information about the image buffers's image data type.
	 * 
	 * @param[in] pImageBuffer		An image buffer
	 * @param[out] image_type	A #IC4_IMAGE_TYPE structure receiving the image data type information.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_get_image_type(const struct IC4_IMAGE_BUFFER* pImageBuffer, struct IC4_IMAGE_TYPE* image_type);

	/**
	 * A structure containing frame metadata
	 */
	struct IC4_FRAME_METADATA
	{
		/**
		 * @brief The frame number assigned to the image by the video capture device
		 * 
		 * @remarks The behavior of this value, including starting value and possible rollover is device-specific.
		 */
		uint64_t device_frame_number;

		/**
		 * @brief The time stamp assigned to the image by the video capture device
		 * 
		 * @remarks The behavior of this value, including possible resets, its starting value or actual resolution is device-specific.
		 */
		uint64_t device_timestamp_ns;
	};

	/**
	 * @brief Retrieves metadata from an image buffer object.
	 * 
	 * @param[in] pImageBuffer An image buffer
	 * @param[out] metadata A #IC4_FRAME_METADATA structure receiving the metadata.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_get_metadata(const struct IC4_IMAGE_BUFFER* pImageBuffer, struct IC4_FRAME_METADATA* metadata);

	/**
	 * @brief Contains options to configure the behavior of #ic4_imagebuffer_copy().
	*/
	enum IC4_IMAGEBUFFER_COPY_FLAGS
	{
		/**
		 * @brief Instructs #ic4_imagebuffer_copy() to skip the image data when copying.
		 * 
		 * If included in the @c flags argument, #ic4_imagebuffer_copy() only copies the non-image parts of the buffer, and a
	     * program-defined algorithm can handle the image copy operation.
		 */
		IC4_IMAGEBUFFER_COPY_SKIP_IMAGE = 1,

		/**
		 * @brief Instructs #ic4_imagebuffer_copy() to not copy the chunk data.
		 * 
		 * This can be useful if the chunk data is large and not required in the destination frame.
		 */
		IC4_IMAGEBUFFER_COPY_SKIP_CHUNKDATA = 2
	};

	/**
	 * @brief Copies the contents of one image buffer to another image buffer.
	 * 
	 * @param source		Source buffer to copy from
	 * @param destination	Destination buffer to copy into
	 * @param flags			A bitwise combination of values from #IC4_IMAGEBUFFER_COPY_FLAGS.
	 * 
	 * @remarks
	 * If the pixel format of the images in @c source and @c destination is not equal, the image is converted. For example,
	 * if the pixel format of @c source is #IC4_PIXEL_FORMAT_BayerRG8 and the pixel format of @c destination is #IC4_PIXEL_FORMAT_BGR8,
	 * a demosaicing operation creates a color image.\n
	 * If @c flags contains #IC4_IMAGEBUFFER_COPY_SKIP_IMAGE, the function does not copy the image data. The function then only copies the meta data, and a
	 * program-defined algorithm can handle the image copy operation.\n
	 * If @c flags contains #IC4_IMAGEBUFFER_COPY_SKIP_CHUNKDATA, the function does not copy the chunk data contained in @c source. This can be useful if the
	 * chunk data is large and not required.
	 * 
	 * @note
	 * If the width or height of @c source and @c destination are not equal, the function fails and the error value is set to #IC4_ERROR_CONVERSION_NOT_SUPPORTED.\n
	 * If there is no algorithm available for the requested conversion, the function fails and the error value is set to #IC4_ERROR_CONVERSION_NOT_SUPPORTED.\n
	 * If the @c destination frame is not writable, the function fails and the error value is set to #IC4_ERROR_INVALID_OPERATION.\n
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_copy(const struct IC4_IMAGE_BUFFER* source, struct IC4_IMAGE_BUFFER* destination, unsigned int flags);

	/**
	 * @brief Checks whether an image buffer object is (safely) writable.
	 * 
	 * In some situations, image buffer objects are shared between the application holding a handle to the image buffer object
	 * and the library. For example, the image buffer might be shared with a display or a video writer.
	 * 
	 * A shared buffer is not safely writable. Writing to a buffer that is shared can lead to unexpected behavior, for example a
	 * modification may partially appear in the result of an operation that is happening in parallel.
	 * 
	 * Passing the image buffer into a function such as #ic4_display_display_buffer() or #ic4_videowriter_add_frame() can lead to a buffer becoming shared.
	 * 
	 * @note	This function only checks for whether the buffer is shared with other parts of the library. It is the program's responsibility \n
	 *			to track whether multiple parts of the program are accessing the buffer's memory at the same time.
	 * 
	 * @param[in] buffer	An image buffer object
	 * 
	 * @note	This function does not track sharing between multiple parts of the application. If the application owns multiple references to
	 *			an image buffer, or shares its buffer memory address between multiple threads, it is the application's responsibility to
	 *			avoid data races.
	 * 
	 * @return	@c true if the image buffer is not shared with any part of the library, and is therefore safely writable.\n
	 *			@c false, if the image buffer is shared and therefore should not be written into.
	 */
	IC4CORE_API bool ic4_imagebuffer_is_writable(const struct IC4_IMAGE_BUFFER* buffer);

	/**
	 * @brief Defines a callback function to be called when an image buffer created by @ref ic4_imagebuffer_wrap_memory
	 * is destroyed and the image data will no longer be accessed through it.
	 * 
	 * @param[in] ptr			Pointer to the image data as passed as @a data into @ref ic4_imagebuffer_wrap_memory
	 * @param[in] buffer_size	Buffer size as passed as @a buffer_size into @ref ic4_imagebuffer_wrap_memory
	 * @param[in] user_ptr		The @a on_release_user_ptr parameter that was specified when the image buffer was created.
	 */
	typedef void (*ic4_imagebuffer_memory_release)(void* ptr, size_t buffer_size, void* user_ptr);

	/**
	 * @brief Creates an image buffer object using external memory as storage area for the image data.
	 * 
	 * This function can be useful when copying image data into buffers of third-party libraries:
	 * - Create an image object in the third-party library
	 * - Wrap the third-party library's image data into an @ref IC4_IMAGE_BUFFER using @ref ic4_imagebuffer_wrap_memory().
	 * - Copy the data from an existing image buffer object into the third-party image buffer using @ref ic4_imagebuffer_copy().
	 * 
	 * @param[out] ppBuffer				Pointer to an image buffer handle to receive the new buffer object.\n
	 *									When the buffer is no longer required, release the object reference using @ref ic4_imagebuffer_unref().
	 * @param[in] data					Pointer to a region of memory to be used as image data by the image buffer object
	 * @param[in] buffer_size			Size of the region of memory pointed to by @a data
	 * @param[in] pitch					Difference between memory addresses of two consecutive lines of image data
	 * @param[in] image_type			Type of image to be stored in the image buffer
	 * @param[in] on_release			Optional pointer to a callback function to be called when the image buffer object is destroyed.\n
	 *									This can be useful when the program needs to keep track of the memory pointer to by the image buffer
	 *									and has to release it once the image buffer object no longer exists.\n
	 *									The lifetime of the image buffer can potentially be extended beyond the existance of its handle
	 *									when it is shared with API functions, e.g. @ref ic4_display_display_buffer or @ref ic4_videowriter_add_frame.
	 * @param[in] on_release_user_ptr	User pointer to be passed when calling @a on_release.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_wrap_memory(struct IC4_IMAGE_BUFFER** ppBuffer, void* data, size_t buffer_size, ptrdiff_t pitch, const struct IC4_IMAGE_TYPE* image_type, ic4_imagebuffer_memory_release on_release, void* on_release_user_ptr);

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

#endif //IC4_C_FRAME_H_INC_