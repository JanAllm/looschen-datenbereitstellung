
#ifndef IC4_SAVEIMAGES_H_INC_
#define IC4_SAVEIMAGES_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_SaveImages.h"

#include "Error.h"

namespace ic4
{
	/**
	 * @brief Contains image file storage options for bitmap files.
	 * 
	 * @see imageBufferSaveAsBitmap()
	 */
	struct SaveBitmapOptions
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		bool store_raw_bayer_data_as_monochrome = false;
	};

	/**
	 * @brief Saves an image buffer as a Bitmap file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a 8-bit monochrome bitmap file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 and stored as a 24-bit color bitmap file
	 */
	inline bool	imageBufferSaveAsBitmap(const ic4::ImageBuffer& buffer, const char* file_path, const SaveBitmapOptions& opt = {}, Error & err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_BMP options = { opt.store_raw_bayer_data_as_monochrome };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_bmp(detail::buffer_ptr(buffer), file_path, &options));
	}
	/**
	 * @brief Saves an image buffer as a Bitmap file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a 8-bit monochrome bitmap file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 and stored as a 24-bit color bitmap file
	 */
	inline bool	imageBufferSaveAsBitmap(const ic4::ImageBuffer& buffer, const std::string& file_path, const SaveBitmapOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsBitmap(buffer, file_path.c_str(), opt, err);
	}

#ifdef _WIN32
	inline bool imageBufferSaveAsBitmap(const ic4::ImageBuffer& buffer, const wchar_t* file_path, const SaveBitmapOptions& opt = {}, Error& err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_BMP options = { opt.store_raw_bayer_data_as_monochrome };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_bmpW(detail::buffer_ptr(buffer), file_path, &options));
	}

	inline bool	imageBufferSaveAsBitmap(const ic4::ImageBuffer& buffer, const std::wstring& file_path, const SaveBitmapOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsBitmap(buffer, file_path.c_str(), opt, err);
	}
#endif

	/**
	 * @brief Defines the possible PNG file compression levels.
	 * 
	 * Higher compression levels can generate smaller files, but the compression can take more time.
	 */
	enum class PngCompressionLevel
	{
		Auto = c_interface::IC4_PNG_COMPRESSION_AUTO,		///< Automatically select a compression level
		Low = c_interface::IC4_PNG_COMPRESSION_LOW,			///< Low compression
		Medium = c_interface::IC4_PNG_COMPRESSION_MEDIUM,	///< Medium compression
		High = c_interface::IC4_PNG_COMPRESSION_HIGH,		///< High compression
		Highest = c_interface::IC4_PNG_COMPRESSION_HIGHEST,	///< Highest compression
	};

	/**
	 * @brief Contains image file storage options for PNG files.
	 * 
	 * @see imageBufferSaveAsPng()
	 */
	struct SavePngOptions
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		bool store_raw_bayer_data_as_monochrome = false;

		/**
		 * @brief Specifies the PNG compression level
		 */
		PngCompressionLevel compression_level = PngCompressionLevel::Auto;
	};

	/**
	 * @brief Saves an image buffer as a PNG file.
	 * 
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 * 
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome PNG file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome PNG file with 8 bits per channel
	 *	- Bayer formats with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel PNG with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel PNG file with 8 bits per channel
	 */
	inline bool	imageBufferSaveAsPng(const ic4::ImageBuffer& buffer, const char* file_path, const SavePngOptions& opt = {}, Error & err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_PNG options =
		{
			opt.store_raw_bayer_data_as_monochrome,
			static_cast<c_interface::IC4_PNG_COMPRESSION_LEVEL>(opt.compression_level)
		};

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_png(detail::buffer_ptr(buffer), file_path, &options));
	}
	/**
	 * @brief Saves an image buffer as a PNG file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome PNG file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome PNG file with 8 bits per channel
	 *	- Bayer formats with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel PNG with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel PNG file with 8 bits per channel
	 */
	inline bool	imageBufferSaveAsPng(const ic4::ImageBuffer& buffer, const std::string& file_path, const SavePngOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsPng(buffer, file_path.c_str(), opt, err);
	}

#ifdef _WIN32
	inline bool imageBufferSaveAsPng(const ic4::ImageBuffer& buffer, const wchar_t* file_path, const SavePngOptions& opt = {}, Error& err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_PNG options =
		{
			opt.store_raw_bayer_data_as_monochrome,
			static_cast<c_interface::IC4_PNG_COMPRESSION_LEVEL>(opt.compression_level)
		};

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_pngW(detail::buffer_ptr(buffer), file_path, &options));
	}

	inline bool	imageBufferSaveAsPng(const ic4::ImageBuffer& buffer, const std::wstring& file_path, const SavePngOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsPng(buffer, file_path.c_str(), opt, err);
	}
#endif

	/**
	 * @brief Contains image file storage options for TIFF files.
	 * 
	 * @see imageBufferSaveAsTiff()
	 */
	struct SaveTiffOptions
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		bool store_raw_bayer_data_as_monochrome = false;
	};

	/**
	 * @brief Saves an image buffer as a Tiff file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome Tiff file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome Tiff file with 8 bits per channel
	 *	- Bayer formats with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel Tiff with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel Tiff file with 8 bits per channel
	 */
	inline bool	imageBufferSaveAsTiff(const ic4::ImageBuffer& buffer, const char* file_path, const SaveTiffOptions& opt = {}, Error & err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_TIFF options = { opt.store_raw_bayer_data_as_monochrome };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_tiff(detail::buffer_ptr(buffer), file_path, &options));
	}
	/**
	 * @brief Saves an image buffer as a Tiff file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome Tiff file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome Tiff file with 8 bits per channel
	 *	- Bayer formats with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel Tiff with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel Tiff file with 8 bits per channel
	 */
	inline bool	imageBufferSaveAsTiff(const ic4::ImageBuffer& buffer, const std::string& file_path, const SaveTiffOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsTiff(buffer, file_path.c_str(), opt, err);
	}

#ifdef _WIN32
	inline bool imageBufferSaveAsTiff(const ic4::ImageBuffer& buffer, const wchar_t* file_path, const SaveTiffOptions& opt = {}, Error& err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_TIFF options = { opt.store_raw_bayer_data_as_monochrome };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_tiffW(detail::buffer_ptr(buffer), file_path, &options));
	}

	inline bool	imageBufferSaveAsTiff(const ic4::ImageBuffer& buffer, const std::wstring& file_path, const SaveTiffOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsTiff(buffer, file_path.c_str(), opt, err);
	}
#endif

	/**
	 * @brief Contains image file storage options for Jpeg files.
	 * 
	 * @see imageBufferSaveAsJpeg()
	 */
	struct SaveJpegOptions
	{
		/**
		 * @brief Specifies the Jpeg image quality in percent.
		 *
		 * High quality images will take more disk space, low quality images are smaller.
		 */
		int quality_pct = 75;
	};

	/**
	 * @brief Saves an image buffer as a Jpeg file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a monochrome jpeg file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a color jpeg file
	 */
	inline bool	imageBufferSaveAsJpeg(const ic4::ImageBuffer& buffer, const char* file_path, const SaveJpegOptions& opt = {}, Error & err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_JPEG options = { opt.quality_pct };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_jpeg(detail::buffer_ptr(buffer), file_path, &options));
	}
	/**
	 * @brief Saves an image buffer as a Jpeg file.
	 *
	 * @param[in] buffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] opt		Options structure configuring the save operation
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	@c true on success, otherwise @c false.\n
	 *			Check the @a err output parameter for error code and error message.
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a monochrome jpeg file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a color jpeg file
	 */
	inline bool	imageBufferSaveAsJpeg(const ic4::ImageBuffer& buffer, const std::string& file_path, const SaveJpegOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsJpeg(buffer, file_path.c_str(), opt, err);
	}

#ifdef _WIN32
	inline bool imageBufferSaveAsJpeg(const ic4::ImageBuffer& buffer, const wchar_t* file_path, const SaveJpegOptions& opt = {}, Error& err = Error::Default())
	{
		const c_interface::IC4_IMAGEBUFFER_SAVE_OPTIONS_JPEG options = { opt.quality_pct };

		return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_save_as_jpegW(detail::buffer_ptr(buffer), file_path, &options));
	}

	inline bool	imageBufferSaveAsJpeg(const ic4::ImageBuffer& buffer, const std::wstring& file_path, const SaveJpegOptions& opt = {}, Error& err = Error::Default())
	{
		return imageBufferSaveAsJpeg(buffer, file_path.c_str(), opt, err);
	}
#endif
}

#endif //IC4_SAVEIMAGES_H_INC_
