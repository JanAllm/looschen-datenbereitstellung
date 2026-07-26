
#ifndef IC4_VIDEOWRITER_H_INC_
#define IC4_VIDEOWRITER_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_VideoWriter.h"

#include "ImageBuffer.h"
#include "Error.h"
#include "ImageType.h"
#include "Properties.h"
#include "HandleRef.h"

#include <memory>

namespace ic4
{
	/**
	 * @brief Defines the available video writer types
	 */
	enum class VideoWriterType
	{
		MP4_H264 = c_interface::IC4_VIDEO_WRITER_MP4_H264,		///< Create MP4 files with H.264 encoding
		MP4_H265 = c_interface::IC4_VIDEO_WRITER_MP4_H265,		///< Create MP4 files with H.265/HEVC encoding
	};

	/**
	 * @brief Represents a video writer
	 * 
	 * To create a video writer, use the @ref VideoWriter::VideoWriter constructor, passing the type of the desired video file.
	 * 
	 * After creating the video writer, call @ref VideoWriter::beginFile() to start writing a new video file.
	 * Call @ref VideoWriter::addFrame() to add images to the video file.
	 * To complete the recording, call @ref VideoWriter::finishFile().
	 * 
	 * Use @ref VideoWriter::propertyMap() to access encoder configuration options.
	 * 
	 * VideoWriter objects are movable.
	 */
	class VideoWriter
	{
		detail::MovableHandleRef<c_interface::IC4_VIDEO_WRITER, c_interface::ic4_videowriter_unref> ptr_;

	public:
		/**
		 * @brief	Creates a new video writer.
		 *
		 * @param[in] type			The type of video file to create a writer for
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @note If the constructor fails, the video writer object is in an invalid state. All member function calls will fail.
		 *
		 * @see VideoWriter::is_valid
		 */
		VideoWriter(VideoWriterType type, Error& err = Error::Default())
		{
			auto writer_type = static_cast<c_interface::IC4_VIDEO_WRITER_TYPE>(type);
			if (!c_interface::ic4_videowriter_create(writer_type, &ptr_))
			{
				detail::returnUpdateFromLastError(err, false);
			}
			else
			{
				detail::clear(err);
			}
		}

	public:
		/**
		 * @brief	Checks whether this video writer is a valid object.
		 *
		 * If the constructor could not create a valid device information object, and the passed @c err was not set to #ic4::Error::Throw(),
		 * an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this video writer object was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the error parameter of the source function for details.
		 *
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Opens a new video file ready to write images into.
		 * 
		 * @param[in] file_name		Name of the new video file
		 * @param[in] image_type	Image type of the images that are going to be written
		 * @param[in] frame_rate	Playback frame rate of the video file
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see VideoWriter::finishFile()
		 */
		bool beginFile(const char* file_name, const ImageType& image_type, double frame_rate, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_videowriter_begin_file(ptr_, file_name, &image_type.data, frame_rate));
		}
		/**
		 * @brief Opens a new video file ready to write images into.
		 *
		 * @param[in] file_name		Name of the new video file
		 * @param[in] image_type	Image type of the images that are going to be written
		 * @param[in] frame_rate	Playback frame rate of the video file
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 *
		 * @see VideoWriter::finishFile()
		 */
		bool beginFile(const std::string& file_name, const ImageType& image_type, double frame_rate, Error& err = Error::Default())
		{
			return beginFile(file_name.c_str(), image_type, frame_rate, err);
		}
#ifdef _WIN32
		bool beginFile(const wchar_t* file_name, const ImageType& image_type, double frame_rate, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_videowriter_begin_fileW(ptr_, file_name, &image_type.data, frame_rate));
		}
		bool beginFile(const std::wstring& file_name, const ImageType& image_type, double frame_rate, Error& err = Error::Default())
		{
			return beginFile(file_name.c_str(), image_type, frame_rate, err);
		}
#endif

		/**
		 * @brief Finishes writing a video file.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 *
		 * @see VideoWriter::beginFile()
		*/
		bool finishFile(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_videowriter_finish_file(ptr_));
		}

		/**
		 * @brief Adds an image to the currently open video file.
		 * 
		 * @param[in] buffer	Image buffer to be added to the video file.
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @pre
		 * @ref VideoWriter::beginFile() must be called before adding images to the video file.
		 * 
		 * @note
		 * The image buffer's image type must be equal to the @a image_type parameter
		 * passed to @ref VideoWriter::beginFile() when starting the file.
		 * 
		 * @note
		 * The video writer can retain a reference to the image buffer. This can delay the release and possible reuse of the image buffer.
		 * In this case, the buffer becomes shared, and is no longer safely writable (see @ref ImageBuffer::isWritable).\n
		 */
		bool addFrame(const std::shared_ptr<ImageBuffer>& buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_videowriter_add_frame(ptr_, detail::buffer_ptr(buffer)));
		}

		/**
		 * @brief Adds an image to the currently open video file, copying its contents in the process.
		 * 
		 * @param[in] buffer	Image buffer to be added to the video file.
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @pre
		 * @ref VideoWriter::beginFile() must be called before adding images to the video file.
		 *
		 * @note
		 * The image buffer's image type must be equal to the @a image_type parameter
		 * passed to @ref VideoWriter::beginFile() when starting the file.
		 */
		bool addFrame(const ImageBuffer& buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_videowriter_add_frame_copy(ptr_, detail::buffer_ptr(buffer)));
		}

		/**
		 * @brief Returns the property map for encoder configuration.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A property map. If the function fails, the property map is invalid.\n
		 *			Check the @a err output parameter or @ref PropertyMap::is_valid().
		 */
		PropertyMap propertyMap(Error& err = Error::Default())
		{
			c_interface::IC4_PROPERTY_MAP* map = nullptr;
			if (!c_interface::ic4_videowriter_get_property_map(ptr_, &map))
			{
				return detail::updateFromLastErrorReturn(err, detail::propmap_invalid());
			}
			return detail::clearReturn(err, detail::propmap_attach(map));
		}
	};
}

#endif // IC4_VIDEOWRITER_H_INC_