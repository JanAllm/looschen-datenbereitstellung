

#ifndef IC4_C_IMAGETYPE_H_INC_
#define IC4_C_IMAGETYPE_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup imgbuf Image Buffers
 * 
 * Image data is passed around in image buffer objects (#IC4_IMAGE_BUFFER).
 * 
 * Image buffer objects are created automatically when using one of the various sink types of this library.
 * 
 * Image buffer objects are reference-counted.
 * To share an image buffer object between multiple parts of a program, use ic4_imagebuffer_ref().
 * Call ic4_imagebuffer_unref() when a reference is no longer required.
 *
 * If the reference count reaches zero, the image buffer is returned to its source to be reused. For example,
 * an image buffer retrieved from ic4_queuesink_pop_output_buffer() will be re-queued.
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
	 * The pixel format defines the representation of pixels in an image.
	 */
	enum IC4_PIXEL_FORMAT
	{
		IC4_PIXEL_FORMAT_Unspecified = 0,			///< Unspecified pixel format, used to partially define a image type

		IC4_PIXEL_FORMAT_Mono8 = 0x01080001,		///< Monochrome 8-bit
		IC4_PIXEL_FORMAT_Mono10p = 0x010A0046,		///< Monochrome 10-bit packed
		IC4_PIXEL_FORMAT_Mono12p = 0x010C0047,		///< Monochrome 12-bit packed
		IC4_PIXEL_FORMAT_Mono16 = 0x01100007,		///< Monochrome 16-bit

		IC4_PIXEL_FORMAT_BayerBG8 = 0x0108000B,		///< Bayer Blue-Green 8-bit
		IC4_PIXEL_FORMAT_BayerBG10p = 0x010A0052,	///< Bayer Blue-Green 10-bit packed
		IC4_PIXEL_FORMAT_BayerBG12p = 0x010C0053,	///< Bayer Blue-Green 12-bit packed
		IC4_PIXEL_FORMAT_BayerBG16 = 0x01100031,	///< Bayer Blue-Green 16-bit

		IC4_PIXEL_FORMAT_BayerGB8 = 0x0108000A,		///< Bayer Green-Blue 8-bit
		IC4_PIXEL_FORMAT_BayerGB10p = 0x010A0054,	///< Bayer Green-Blue 10-bit packed
		IC4_PIXEL_FORMAT_BayerGB12p = 0x010C0055,	///< Bayer Green-Blue 12-bit packed
		IC4_PIXEL_FORMAT_BayerGB16 = 0x01100030,	///< Bayer Green-Blue 16-bit

		IC4_PIXEL_FORMAT_BayerGR8 = 0x01080008,		///< Bayer Green-Red 8-bit
		IC4_PIXEL_FORMAT_BayerGR10p = 0x010A0056,	///< Bayer Green-Red 10-bit packed
		IC4_PIXEL_FORMAT_BayerGR12p = 0x010C0057,	///< Bayer Green-Red 12-bit packed
		IC4_PIXEL_FORMAT_BayerGR16 = 0x0110002E,	///< Bayer Green-Red 16-bit

		IC4_PIXEL_FORMAT_BayerRG8 = 0x01080009,		///< Bayer Red-Green 8-bit
		IC4_PIXEL_FORMAT_BayerRG10p = 0x010A0058,	///< Bayer Red-Green 10-bit packed
		IC4_PIXEL_FORMAT_BayerRG12p = 0x010C0059,	///< Bayer Red-Green 12-bit packed
		IC4_PIXEL_FORMAT_BayerRG16 = 0x0110002F,	///< Bayer Red-Green 16-bit

		IC4_PIXEL_FORMAT_BGRa8 = 0x02200017,		///< Blue-Green-Red-alpha 8-bit
		IC4_PIXEL_FORMAT_BGRa16 = 0x02400051,		///< Blue-Green-Red-alpha 16-bit
		IC4_PIXEL_FORMAT_BGR8 = 0x02180015,			///< Blue-Green-Red 8-bit

		IC4_PIXEL_FORMAT_Mono12Packed = 0x010C0006,		///< GigE Vision specific format, Monochrome 12-bit packed
		IC4_PIXEL_FORMAT_BayerBG12Packed = 0x010C002D,	///< GigE Vision specific format, Bayer Blue-Green 12-bit packed
		IC4_PIXEL_FORMAT_BayerGB12Packed = 0x010C002C,	///< GigE Vision specific format, Bayer Green-Blue 12-bit packed
		IC4_PIXEL_FORMAT_BayerGR12Packed = 0x010C002A,	///< GigE Vision specific format, Bayer Green-Red 12-bit packed
		IC4_PIXEL_FORMAT_BayerRG12Packed = 0x010C002B,	///< GigE Vision specific format, Bayer Red-Green 12-bit packed

		IC4_PIXEL_FORMAT_YUV422_8 = 0x02100032,				///< YUV 4:2:2 8-bit
		IC4_PIXEL_FORMAT_YCbCr422_8 = 0x0210003B,			///< YCbCr 4:2:2 8-bit
		IC4_PIXEL_FORMAT_YCbCr411_8_CbYYCrYY = 0x020C003C,	///< YCbCr 4:1:1 8-bit (CbYYCrYY)
		IC4_PIXEL_FORMAT_YCbCr411_8 = 0x020C005A,			///< YCbCr 4:1:1 8-bit (YYCbYYCr)

		/**
		 * @brief Virtual pixel format value to select any 8-bit bayer format
		 * 
		 * When setting the camera's @ref IC4_PROPID_PIXEL_FORMAT to this value, automatically selects one of the 8-bit bayer pixel formats
		 * @ref IC4_PIXEL_FORMAT_BayerBG8, @ref IC4_PIXEL_FORMAT_BayerGB8, @ref IC4_PIXEL_FORMAT_BayerRG8 or @ref IC4_PIXEL_FORMAT_BayerGR8.
		 */
		IC4_PIXEL_FORMAT_AnyBayer8 = (int)0x8108FF01,

		/**
		 * @brief Virtual pixel format value to select any 10-bit packed bayer format
		 * 
		 * When setting the camera's @ref IC4_PROPID_PIXEL_FORMAT to this value, automatically selects one of the 10-bit packed bayer pixel formats
		 * @ref IC4_PIXEL_FORMAT_BayerBG10p, @ref IC4_PIXEL_FORMAT_BayerGB10p, @ref IC4_PIXEL_FORMAT_BayerRG10p or @ref IC4_PIXEL_FORMAT_BayerGR10p.
		 */
		IC4_PIXEL_FORMAT_AnyBayer10p = (int)0x810AFF01,

		/**
		 * @brief Virtual pixel format value to select any 12-bit packed bayer format
		 * 
		 * When setting the camera's @ref IC4_PROPID_PIXEL_FORMAT to this value, automatically selects one of the 12-bit packed bayer pixel formats
		 * @ref IC4_PIXEL_FORMAT_BayerBG12p, @ref IC4_PIXEL_FORMAT_BayerGB12p, @ref IC4_PIXEL_FORMAT_BayerRG12p, @ref IC4_PIXEL_FORMAT_BayerGR12p,
		 * @ref IC4_PIXEL_FORMAT_BayerBG12Packed, @ref IC4_PIXEL_FORMAT_BayerGB12Packed, @ref IC4_PIXEL_FORMAT_BayerRG12Packed or @ref IC4_PIXEL_FORMAT_BayerGR12Packed.
		 */
		IC4_PIXEL_FORMAT_AnyBayer12p = (int)0x810CFF01,

		/**
		 * @brief Virtual pixel format value to select any 16-bit bayer format
		 * 
		 * When setting the camera's @ref IC4_PROPID_PIXEL_FORMAT to this value, automatically selects one of the 16-bit bayer pixel formats
		 * @ref IC4_PIXEL_FORMAT_BayerBG16, @ref IC4_PIXEL_FORMAT_BayerGB16, @ref IC4_PIXEL_FORMAT_BayerRG16 or @ref IC4_PIXEL_FORMAT_BayerGR16.
		 */
		IC4_PIXEL_FORMAT_AnyBayer16 = (int)0x8110FF01,

		IC4_PIXEL_FORMAT_Invalid = -1,				///< Invalid pixel format
	};

	/**
	 * @struct IC4_IMAGE_TYPE
	 * 
	 * Represents an image type, including pixel format and image dimensions.
	 * 
	 * Using a partially-specified image type is allowed when defining the buffer format of a sink.
	 * The sink will fill the other fields with data from the device automatically.
	 */
	struct IC4_IMAGE_TYPE
	{
		/**
		 * Specifies the pixel format of the image.
		 * 
		 * Setting \a pixel_format to \c IC4_PIXEL_FORMAT_Unspecified creates a partially-specified image type.
		 */
		enum IC4_PIXEL_FORMAT pixel_format;

		/**
		 * Specifies the width of the image.
		 * 
		 * Setting \a width to \c 0 creates a partially-specified image type.
		 */
		uint32_t width;
		/**
		 * Specifies the height of the image.
		 * 
		 * Setting \a height to \c 0 creates a partially-specified image type.
		 */
		uint32_t height;
	};

	/**
	 * @brief Returns the name of a pixel format.
	 * 
	 * @param[in] pixel_format	A pixel format
	 * 
	 * @return	A pointer to a null-terminated string containing the name of the pixel format.\n
	 *			The memory pointed to by the return value is valid forever does not need to be released.\n
	 *			If the passed pixel format is unknown, the function returns \c NULL.
	 */
	IC4CORE_API const char* ic4_pixelformat_tostring(enum IC4_PIXEL_FORMAT pixel_format);

	/**
	 * @brief Returns the bits per pixel of a pixel format.
	 * 
	 * @param[in] pixel_format  pixel format
	 * 
	 * @return	The bits required to store one pixel using the given pixel format.\n
	 *			The function returns \c 0 if the bits per pixel of the pixel format is unknown.
	 */
	IC4CORE_API size_t ic4_pixelformat_bpp(enum IC4_PIXEL_FORMAT pixel_format);

	/**
	 * @brief Convert a given image type into a string representation.
	 *
	 * @param[out] image_type		Pointer to a #IC4_IMAGE_TYPE value to receive the error code.
	 * @param[out] buffer			Pointer to a character array to receive the string.\n
	 *								This parameter can be \c NULL to find out the required space without allocating an initial array.
	 * @param[in, out] buffer_size	Pointer to a \c size_t describing the length of the array pointed to by \a buffer.\n
	 *								The function always writes the actual number of characters required to store the string representation.
	 *
	 * @return	\c true on success.
	 * @return	If \a image_type is \c NULL, the function fails and returns \c false.
	 * @return	If \a buffer is not \c NULL, and \c *buffer_size is smaller than the required number
	 *			of bytes to store the string, the function fails and returns \c false.
	 * @return	Use #ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_imagetype_tostring(const struct IC4_IMAGE_TYPE* image_type, char* buffer, size_t* buffer_size);

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

#endif //IC4_C_IMAGETYPE_H_INC_