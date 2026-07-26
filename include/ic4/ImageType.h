
#ifndef IC4_IMAGETYPE_H_INC_
#define IC4_IMAGETYPE_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_ImageType.h"

#include "Error.h"
#include "StringUtil.h"

#include <vector>
#include <string>
#include <cassert>

namespace ic4
{
	/**
	 * The pixel format defines the representation of pixels in an image.
	 */
	enum class PixelFormat : int32_t
	{
		Unspecified = c_interface::IC4_PIXEL_FORMAT_Unspecified,		///< Unspecified pixel format, used to partially define a image type

		Mono8 = c_interface::IC4_PIXEL_FORMAT_Mono8,					///< Monochrome 8-bit
		Mono10p = c_interface::IC4_PIXEL_FORMAT_Mono10p,				///< Monochrome 10-bit packed
		Mono12p = c_interface::IC4_PIXEL_FORMAT_Mono12p,				///< Monochrome 12-bit packed
		Mono16 = c_interface::IC4_PIXEL_FORMAT_Mono16,					///< Monochrome 16-bit

		BayerBG8 = c_interface::IC4_PIXEL_FORMAT_BayerBG8,				///< Bayer Blue-Green 8-bit
		BayerBG10p = c_interface::IC4_PIXEL_FORMAT_BayerBG10p,			///< Bayer Blue-Green 10-bit packed
		BayerBG12p = c_interface::IC4_PIXEL_FORMAT_BayerBG12p,			///< Bayer Blue-Green 12-bit packed
		BayerBG16 = c_interface::IC4_PIXEL_FORMAT_BayerBG16,			///< Bayer Blue-Green 16-bit

		BayerGB8 = c_interface::IC4_PIXEL_FORMAT_BayerGB8,				///< Bayer Green-Blue 8-bit
		BayerGB10p = c_interface::IC4_PIXEL_FORMAT_BayerGB10p,			///< Bayer Green-Blue 10-bit packed
		BayerGB12p = c_interface::IC4_PIXEL_FORMAT_BayerGB12p,			///< Bayer Green-Blue 12-bit packed
		BayerGB16 = c_interface::IC4_PIXEL_FORMAT_BayerGB16,			///< Bayer Green-Blue 16-bit

		BayerGR8 = c_interface::IC4_PIXEL_FORMAT_BayerGR8,				///< Bayer Green-Red 8-bit
		BayerGR10p = c_interface::IC4_PIXEL_FORMAT_BayerGR10p,			///< Bayer Green-Red 10-bit packed
		BayerGR12p = c_interface::IC4_PIXEL_FORMAT_BayerGR12p,			///< Bayer Green-Red 12-bit packed
		BayerGR16 = c_interface::IC4_PIXEL_FORMAT_BayerGR16,			///< Bayer Green-Red 16-bit

		BayerRG8 = c_interface::IC4_PIXEL_FORMAT_BayerRG8,				///< Bayer Red-Green 8-bit
		BayerRG10p = c_interface::IC4_PIXEL_FORMAT_BayerRG10p,			///< Bayer Red-Green 10-bit packed
		BayerRG12p = c_interface::IC4_PIXEL_FORMAT_BayerRG12p,			///< Bayer Red-Green 12-bit packed
		BayerRG16 = c_interface::IC4_PIXEL_FORMAT_BayerRG16,			///< Bayer Red-Green 16-bit

		BGRa8 = c_interface::IC4_PIXEL_FORMAT_BGRa8,					///< Blue-Green-Red-alpha 8-bit
		BGRa16 = c_interface::IC4_PIXEL_FORMAT_BGRa16,					///< Blue-Green-Red-alpha 16-bit
		BGR8 = c_interface::IC4_PIXEL_FORMAT_BGR8,						///< Blue-Green-Red 8-bit

		Mono12Packed = c_interface::IC4_PIXEL_FORMAT_Mono12Packed,		///< GigE Vision specific format, Monochrome 12-bit packed
		BayerBG12Packed = c_interface::IC4_PIXEL_FORMAT_BayerBG12Packed,///< GigE Vision specific format, Bayer Blue-Green 12-bit packed
		BayerGB12Packed = c_interface::IC4_PIXEL_FORMAT_BayerGB12Packed,///< GigE Vision specific format, Bayer Green-Blue 12-bit packed
		BayerGR12Packed = c_interface::IC4_PIXEL_FORMAT_BayerGR12Packed,///< GigE Vision specific format, Bayer Green-Red 12-bit packed
		BayerRG12Packed = c_interface::IC4_PIXEL_FORMAT_BayerRG12Packed,///< GigE Vision specific format, Bayer Red-Green 12-bit packed

		YUV422_8 = c_interface::IC4_PIXEL_FORMAT_YUV422_8,						///< YUV 4:2:2 8-bit
		YCbCr422_8 = c_interface::IC4_PIXEL_FORMAT_YCbCr422_8,					///< YCbCr 4:2:2 8-bit
		YCbCr411_8_CbYYCrYY = c_interface::IC4_PIXEL_FORMAT_YCbCr411_8_CbYYCrYY,///< YCbCr 4:1:1 8-bit (CbYYCrYY)
		YCbCr411_8 = c_interface::IC4_PIXEL_FORMAT_YCbCr411_8,					///< YCbCr 4:1:1 8-bit (YYCbYYCr)

		/**
		 * @brief Virtual pixel format value to select any 8-bit bayer format
		 *
		 * When setting the camera's @ref ic4::PropId::PixelFormat to this value, automatically selects one of the 8-bit bayer pixel formats
		 * @ref PixelFormat::BayerBG8, @ref PixelFormat::BayerGB8, @ref PixelFormat::BayerRG8 or @ref PixelFormat::BayerGR8.
		 */
		AnyBayer8 = c_interface::IC4_PIXEL_FORMAT_AnyBayer8,

		/**
		 * @brief Virtual pixel format value to select any 10-bit packed bayer format
		 *
		 * When setting the camera's @ref ic4::PropId::PixelFormat to this value, automatically selects one of the 10-bit packed bayer pixel formats
		 * @ref PixelFormat::BayerBG10p, @ref PixelFormat::BayerGB10p, @ref PixelFormat::BayerRG10p or @ref PixelFormat::BayerGR10p.
		 */
		AnyBayer10p = c_interface::IC4_PIXEL_FORMAT_AnyBayer10p,

		/**
		 * @brief Virtual pixel format value to select any 12-bit packed bayer format
		 *
		 * When setting the camera's @ref ic4::PropId::PixelFormat to this value, automatically selects one of the 12-bit packed bayer pixel formats
		 * @ref PixelFormat::BayerBG12p, @ref PixelFormat::BayerGB12p, @ref PixelFormat::BayerRG12p, @ref PixelFormat::BayerGR12p,
		 * @ref PixelFormat::BayerBG12Packed, @ref PixelFormat::BayerGB12Packed, @ref PixelFormat::BayerRG12Packed or @ref PixelFormat::BayerGR12Packed.
		 */
		AnyBayer12p = c_interface::IC4_PIXEL_FORMAT_AnyBayer12p,

		/**
		 * @brief Virtual pixel format value to select any 16-bit bayer format
		 *
		 * When setting the camera's @ref ic4::PropId::PixelFormat to this value, automatically selects one of the 16-bit bayer pixel formats
		 * @ref PixelFormat::BayerBG16, @ref PixelFormat::BayerGB16, @ref PixelFormat::BayerRG16 or @ref PixelFormat::BayerGR16.
		 */
		AnyBayer16 = c_interface::IC4_PIXEL_FORMAT_AnyBayer16,

		Invalid = c_interface::IC4_PIXEL_FORMAT_Invalid,				///< Invalid pixel format
	};

	/**
	 * @brief Returns the bits per pixel of a pixel format.
	 *
	 * @param[in] pixel_format  A pixel format
	 *
	 * @return	The bits required to store one pixel using the given pixel format.\n
	 *			The function returns \c 0 if the bits per pixel of the pixel format is unknown.
	 */
	inline size_t getBitsPerPixel(PixelFormat pixel_format) noexcept
	{
		return c_interface::ic4_pixelformat_bpp(static_cast<c_interface::IC4_PIXEL_FORMAT>(pixel_format));
	}

	/**
	 * @brief Represents an image type, including pixel format and image dimensions.
	 * 
	 * Using a partially-specified image type is allowed when defining the buffer format of a sink.
	 * The sink will fill the other fields with data from the device automatically.
	 */
	struct ImageType
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		c_interface::IC4_IMAGE_TYPE data;
#endif
		/**
		 * @brief Constructs an invalid image type.
		 */
		ImageType() noexcept
		{
			data = c_interface::IC4_IMAGE_TYPE{ c_interface::IC4_PIXEL_FORMAT::IC4_PIXEL_FORMAT_Invalid };
		}
#ifndef IC4_DOXYGEN_INVISIBLE
		explicit ImageType(const c_interface::IC4_IMAGE_TYPE* ptr) noexcept
		{
			assert( ptr != nullptr );
			data = *ptr;
		}
#endif
		/**
		 * @brief Constructs an image type, specifying only a pixel format.
		 * 
		 * Width and height remain unspecified.
		 * 
		 * @param[in] pixel_format	The pixel format of the image
		 */
		ImageType(PixelFormat pixel_format) noexcept
		{
			data = c_interface::IC4_IMAGE_TYPE{ static_cast<c_interface::IC4_PIXEL_FORMAT>(pixel_format) };
		}
		/**
		 * @brief Constructs an image type, specifying pixel format, width and height.
		 * 
		 * @param[in] pixel_format	The pixel format of the image
		 * @param[in] width			The width of the image in pixels
		 * @param[in] height		The height of the image in pixels
		*/
		ImageType(PixelFormat pixel_format, uint32_t width, uint32_t height) noexcept
		{
			data = c_interface::IC4_IMAGE_TYPE{ static_cast<c_interface::IC4_PIXEL_FORMAT>(pixel_format), width, height };
		}

		/**
		 * @brief Returns the pixel format of this image type.
		 * 
		 * @return The pixel format of this image type, or @ref PixelFormat::Unspecified if the pixel format is not specified.
		 */
		PixelFormat pixel_format() const noexcept { return static_cast<PixelFormat>(data.pixel_format); }
		/**
		 * @brief Returns the width of this image type.
		 * 
		 * @return The width of this image type, or @c 0 if the height is not specified.
		 */
		uint32_t width() const noexcept { return data.width; }
		/**
		 * @brief Returns the height of this image type.
		 * 
		 * @return The height of this image type, or @c 0 if the height is not specified.
		 */
		uint32_t height() const noexcept { return data.height; }

		/**
		 * @brief Checks whether two image types are equal.
		 * 
		 * @param op2	Other image type
		 * 
		 * @return @c true if this image type is equal to @a op2, otherwise @c false.
		*/
		bool	operator==(const ImageType& op2) const noexcept
		{
			return (pixel_format() == op2.pixel_format()) && (width() == op2.width()) && (height() == op2.height());
		}
		/**
		 * @brief Checks whether two image types are not equal.
		 * 
		 * @param op2	Other image type
		 * 
		 * @return @c true if this image type is different from @a op2, otherwise @c false.
		 */
		bool	operator!=(const ImageType& op2) const noexcept
		{
			return !(*this == op2);
		}
	};

	/**
	 * @brief Creates a string representation for an image type.
	 * 
	 * @param[in] image_type	An image type
	 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	A string representation for @a image_type.\n
	 *			If no string representation could be created, an empty string is returned.
	 *			Check the @a err output parameter for error code and error message.
	 */
	inline std::string to_string(const ImageType& image_type, Error& err = Error::Default())
	{
		size_t required_size = 128;

		std::string result(required_size, '\0' );
		if (!ic4_imagetype_tostring(&image_type.data, &result[0], &required_size))
		{
			result.resize(required_size);

			if (!ic4_imagetype_tostring(&image_type.data, &result[0], &required_size))
			{
				return detail::updateFromLastErrorReturn(err, std::string{});
			}
		}
		if (required_size > 0) {
			result.resize(required_size - 1);
		}
		return detail::clearReturn(err, result);
	}

	/**
	 * @brief Creates a string representation for a pixel format.
	 *
	 * @param[in] fmt		A pixel format
	 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
	 *
	 * @return	A string representation for @a fmt.\n
	 *			If no string representation could be created, an empty string is returned.
	 *			Check the @a err output parameter for error code and error message.
	 *
	 */
	inline std::string to_string(PixelFormat fmt, Error& err = Error::Default())
	{
		return detail::updateFromLastErrorReturn(err, detail::string_or_empty(ic4_pixelformat_tostring(static_cast<c_interface::IC4_PIXEL_FORMAT>(fmt))));
	}
}


#endif // IC4_IMAGETYPE_H_INC_