
#ifndef IC4_C_SAVEIMAGES_H_INC_
#define IC4_C_SAVEIMAGES_H_INC_

#include "ic4core_export.h"

#include "C_ImageBuffer.h"

/** @defgroup saveimages Saving Images
 *
 * @brief A collection of functions to store image buffers as in various types of images files
 *
 * The supported files formats include
 *		- Bitmap
 *		- Jpeg
 *		- Tiff
 *		- Png
 *
 * For each file format, there is a separate function to save the image buffer in a file,
 * for example #ic4_imagebuffer_save_as_bmp() or #ic4_imagebuffer_save_as_jpeg()
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

	struct IC4_IMAGE_BUFFER;

	/**
	 * @brief Contains image file storage options for bitmap files.
	 */
	struct IC4_IMAGEBUFFER_SAVE_OPTIONS_BMP
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		int store_bayer_raw_data_as_monochrome;
	};

	/**
	 * Defines the possible PNG file compression levels.
	 * 
	 * Higher compression levels can generate smaller files, but the compression can take more time.
	 */
	enum IC4_PNG_COMPRESSION_LEVEL
	{
		IC4_PNG_COMPRESSION_AUTO,		///< Automatically select a compression level
		IC4_PNG_COMPRESSION_LOW,		///< Low compression
		IC4_PNG_COMPRESSION_MEDIUM,		///< Medium compression
		IC4_PNG_COMPRESSION_HIGH,		///< High compression
		IC4_PNG_COMPRESSION_HIGHEST,	///< Highest compression
	};

	/**
	 * @brief Contains image file storage options for PNG files.
	 */
	struct IC4_IMAGEBUFFER_SAVE_OPTIONS_PNG
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		int store_bayer_raw_data_as_monochrome;

		/**
		 * Specifies the PNG compression level
		 */
		enum IC4_PNG_COMPRESSION_LEVEL compression_level;
	};

	/**
	 * @brief Contains image file storage options for Jpeg files.
	 */
	struct IC4_IMAGEBUFFER_SAVE_OPTIONS_JPEG
	{
		/**
		 * @brief Specifies the Jpeg image quality in percent.
		 *
		 * High quality images will take more disk space, low quality images are smaller.
		 */
		int quality_pct;
	};

	/**
	 * @brief Contains image file storage options for TIFF files.
	 */
	struct IC4_IMAGEBUFFER_SAVE_OPTIONS_TIFF
	{
		/**
		 * @brief If set, and the image buffer's pixel format is a bayer format, interpret the pixel data as monochrome
		 * and store the raw data as a monochrome image.
		 */
		int store_bayer_raw_data_as_monochrome;
	};


	/**
	 * @brief Saves an image buffer as a Bitmap file.
	 * 
	 * @param[in] pImageBuffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] options	Options structure configuring the save operation
	 * 
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a 8-bit monochrome bitmap file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 and stored as a 24-bit color bitmap file
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_save_as_bmp(struct IC4_IMAGE_BUFFER* pImageBuffer, const char* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_BMP* options);
	/**
	 * @brief Saves an image buffer as a Jpeg file.
	 *
	 * @param[in] pImageBuffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] options	Options structure configuring the save operation
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats are converted to Mono8 and stored as a monochrome jpeg file
	 *	- Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a color jpeg file
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_save_as_jpeg(struct IC4_IMAGE_BUFFER* pImageBuffer, const char* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_JPEG* options);
	/**
	 * @brief Saves an image buffer as a Tiff file.
	 *
	 * @param[in] pImageBuffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] options	Options structure configuring the save operation
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome Tiff file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome Tiff file with 8 bits per channel
	 *	- Bayer format with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel Tiff with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel Tiff file with 8 bits per channel
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_save_as_tiff(struct IC4_IMAGE_BUFFER* pImageBuffer, const char* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_TIFF* options);
	/**
	 * @brief Saves an image buffer as a PNG file.
	 *
	 * @param[in] pImageBuffer	An image buffer
	 * @param[in] file_path	Path of the image file
	 * @param[in] options	Options structure configuring the save operation
	 *
	 * @note
	 * Depending on the pixel format of the image buffer, a transformation is applied before saving the image.
	 *	- Monochrome pixel formats with a bit depth higher than 8bpp are converted to Mono16 and stored as a monochrome PNG file with 16 bits per channel
	 *	- Mono8 image buffers are stored as a monochrome PNG file with 8 bits per channel
	 *	- Bayer format with a bit depth higher than 8bpp are converted to BGRa16 and stored as a 4-channel PNG with 16 bits per channel
	 *	- 8-bit Bayer, RGB and YUV pixel formats are converted to BGR8 stored as a 3-channel PNG file with 8 bits per channel
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagebuffer_save_as_png(struct IC4_IMAGE_BUFFER* pImageBuffer, const char* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_PNG* options);

#ifndef IC4_DOXYGEN_INVISIBLE
#ifdef _WIN32
	IC4CORE_API bool ic4_imagebuffer_save_as_bmpW(struct IC4_IMAGE_BUFFER* pImageBuffer, const wchar_t* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_BMP* options);
	IC4CORE_API bool ic4_imagebuffer_save_as_jpegW(struct IC4_IMAGE_BUFFER* pImageBuffer, const wchar_t* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_JPEG* options);
	IC4CORE_API bool ic4_imagebuffer_save_as_tiffW(struct IC4_IMAGE_BUFFER* pImageBuffer, const wchar_t* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_TIFF* options);
	IC4CORE_API bool ic4_imagebuffer_save_as_pngW(struct IC4_IMAGE_BUFFER* pImageBuffer, const wchar_t* file_path, const struct IC4_IMAGEBUFFER_SAVE_OPTIONS_PNG* options);
#endif
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

#endif //IC4_C_SAVEIMAGES_H_INC_