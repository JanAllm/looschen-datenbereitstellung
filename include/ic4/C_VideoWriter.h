
#ifndef IC4_C_VIDEO_WRITER_H_INC_
#define IC4_C_VIDEO_WRITER_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stdint.h>

/** @defgroup videowriter Video Writer
 *
 * @brief Video writers allow compressing and saving images into video files.
 *
 * There are multiple video writers available:
 *		- MP4 file with H264 encoder
 *		- MP4 file with H265 encoder
 * 
 * To create a new video writer, call #ic4_videowriter_create() passing the desired video writer type.
 * 
 * After calling #ic4_videowriter_begin_file(), repeatedly call #ic4_videowriter_add_frame() to write images into the file.
 * Call #ic4_videowriter_finish_file() to finalize and close the file.
 * 
 * A video writer can be configured through the property map returned by #ic4_videowriter_get_property_map(). For details on how
 * to use such a property map, see @ref properties.
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
	 * Defines the available video writer types
	 */
	enum IC4_VIDEO_WRITER_TYPE
	{
		IC4_VIDEO_WRITER_MP4_H264,	///< Create MP4 files with H.264 encoding
		IC4_VIDEO_WRITER_MP4_H265,	///< Create MP4 files with H.265/HEVC encoding
	};

	/**
	 * @struct IC4_VIDEO_WRITER
	 * 
	 * @brief Represents a video writer
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_VIDEO_WRITER*.
	 * 
	 * Video writer objects are reference-counted.
	 * To increase the reference count, use #ic4_videowriter_ref().
	 * Call ic4_videowriter_unref() when a reference is no longer required.
	 */
	struct IC4_VIDEO_WRITER;

	struct IC4_IMAGE_BUFFER;

	/**
	 * @brief Creates a new video writer of the specified type.
	 * 
	 * @param[in] type				Selects the type of video writer to create
	 * @param[out] ppVideoWriter	Pointer to receive the handle of the new video writer\n
	 *								When the video writer is no longer required, release the object reference using #ic4_videowriter_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_create(enum IC4_VIDEO_WRITER_TYPE type, struct IC4_VIDEO_WRITER** ppVideoWriter);	

	/**
	 * @brief Increases the video writer's internal reference count by one.
	 *
	 * @param[in] pVideoWriter A pointer to a video writer
	 *
	 * @return The pointer passed via \a pDisplay
	 *
	 * @remarks If \a pVideoWriter is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_videowriter_unref
	 */
	IC4CORE_API struct IC4_VIDEO_WRITER* ic4_videowriter_ref(struct IC4_VIDEO_WRITER* pVideoWriter);
	/**
	 * @brief Decreases the video writer's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pVideoWriter A pointer to a video writer
	 *
	 * @remarks If \a pVideoWriter is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_videowriter_ref
	 */
	IC4CORE_API void ic4_videowriter_unref(struct IC4_VIDEO_WRITER* pVideoWriter);

	/**
	 * @brief Opens a new video file ready to write images into.
	 * 
	 * @param[in] pVideoWriter	A video writer
	 * @param[in] file_name		Name of the new video file
	 * @param[in] image_type	Image type of the images that are going to be written
	 * @param[in] frame_rate	Playback frame rate of the video file
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_begin_file(struct IC4_VIDEO_WRITER* pVideoWriter, const char* file_name, const struct IC4_IMAGE_TYPE* image_type, double frame_rate);
#ifdef _WIN32
	IC4CORE_API bool ic4_videowriter_begin_fileW(struct IC4_VIDEO_WRITER* pVideoWriter, const wchar_t* file_name, const struct IC4_IMAGE_TYPE* image_type, double frame_rate);
#endif
	/**
	 * @brief Finishes writing a video file.
	 * 
	 * @param[in] pVideoWriter	A video writer that previously began writing a file
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_finish_file(struct IC4_VIDEO_WRITER* pVideoWriter);

	/**
	 * @brief Adds an image to the currently open video file.
	 * 
	 * @param[in] pVideoWriter	A video writer that previously began writing a file
	 * @param[in] buffer		An image buffer
	 * 
	 * @note
	 * The image buffer's image type must be equal to the \c image_type parameter passed to #ic4_videowriter_begin_file() when starting the file.\n
	 * The video writer can retain a reference to the image buffer. This can delay the release and possible reuse of the image buffer.
	 * In this case, the buffer becomes shared, and is no longer safely writable (see @ref ic4_imagebuffer_is_writable).\n
	 * Use @ref ic4_videowriter_add_frame_copy to always let the video writer immediately copy the data out of the image buffer.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_add_frame(struct IC4_VIDEO_WRITER* pVideoWriter, struct IC4_IMAGE_BUFFER* buffer);

	/**
	 * @brief Adds an image to the currently open video file, copying its contents in the process.
	 *
	 * @param[in] pVideoWriter	A video writer that previously began writing a file
	 * @param[in] buffer		An image buffer
	 *
	 * @note
	 * The image buffer's image type must be equal to the \c image_type parameter passed to #ic4_videowriter_begin_file() when starting the file.\n
	 * The image buffer's contents will be copied, so that the buffer's reference count is not increased and it can be reused immedietely if
	 * the final reference is released.\n
	 * Use @ref ic4_videowriter_add_frame to avoid the copy operation if it is not necessary.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_add_frame_copy(struct IC4_VIDEO_WRITER* pVideoWriter, const struct IC4_IMAGE_BUFFER* buffer);

	/**
	 * @brief Returns the property map for encoder configuration.
	 * 
	 * @param[in] pVideoWriter		A video writer
	 * @param[out] ppPropertyMap	Pointer to a handle that receives the property map.\n
	 *								When the property map is longer required, call #ic4_propmap_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_videowriter_get_property_map(struct IC4_VIDEO_WRITER* pVideoWriter, struct IC4_PROPERTY_MAP** ppPropertyMap);
		
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

#endif //IC4_C_VIDEO_WRITER_H_INC_