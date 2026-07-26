
#include <ic4/ic4.h>
#include <opencv2/opencv.hpp>

/**
 * @brief Contains functions providing interoperability between IC Imaging Control 4 and third-party libraries.
 *
 * The provided functions include conversions between @ref ic4::ImageBuffer objects and third-party library types
 * as well as display adapters for GUI toolkits.
 *
 * The interop support functions are declared separate headers:
 * - OpenCV: @c ic4interop/interop-OpenCV.h
 * - MVTec HALCON: @c ic4interop/interop-HALCON.h
 * - Qt5 and Qt6: @c interop/interop-Qt.h
 */
namespace ic4interop
{
	/**
	 * @brief Contains functions for using @ref ic4::ImageBuffer objects using @a OpenCV.
	 * 
	 * The @a OpenCV interop support functions are declared in @c ic4interop/interop-OpenCV.h.
	 */
	class OpenCV
	{
	public:
		/**
		 * @brief Wraps the passed image buffer in an @a OpenCV @c cv::Mat.
		 * 
		 * @param[in] buffer	An image buffer
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return An @a OpenCV @c cv::Mat using the image buffer's memory as pixel storage
		 * 
		 * @remarks
		 * Wrapping the contents of an image buffer in an @a OpenCV @c cv::Mat is only possible if the pixel format is one of the following:
		 * - @c Mono8 or any Bayer8 format
		 * - @c Mono16 or any Bayer16 format
		 * - @c BGRa8, @c BGR8 or @c BGRa16
		 * - @c YCbCr422_8 or @c YUV422_8
		 */
		static cv::Mat wrap(const ic4::ImageBuffer& buffer, ic4::Error& err = ic4::Error::Default())
		{
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return {};

			auto info = analyze_format(image_type.pixel_format());

			if (!info.can_wrap)
			{
				return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to wrap image buffer", cv::Mat());
			}

			auto mat = cv::Mat(
				static_cast<int>(image_type.height()),
				static_cast<int>(image_type.width()),
				info.cv_type,
				buffer.ptr(),
				static_cast<size_t>(buffer.pitch())
			);

			return ic4::detail::clearReturn(err, mat);
		}

		/**
		 * @brief Checks whether @ref wrap can work on the image buffer's pixel format.
		 * 
		 * @param[in] buffer	An image buffer
		 * 
		 * @return @c true, if the contents of the image buffer can be wrapped in an @a OpenCV @c cv::Mat, otherwise @c false.
		 * 
		 * @remarks
		 * Wrapping the contents of an image buffer in an @a OpenCV @c cv::Mat is only possible if the pixel format is one of the following:
		 * - @c Mono8 or any Bayer8 format
		 * - @c Mono16 or any Bayer16 format
		 * - @c BGRa8, @c BGR8 or @c BGRa16
		 * - @c YCbCr422_8 or @c YUV422_8
		 */
		static bool canWrap(const ic4::ImageBuffer& buffer)
		{
			ic4::Error err;
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return false;

			auto info = analyze_format(image_type.pixel_format());

			return info.can_wrap;
		}

		/**
		 * @brief Creates a copy of the image buffer and stores it in an @a OpenCV @c cv::Mat.
		 * 
		 * @param[in] buffer	An image buffer
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return An @a OpenCV @c cv::Mat containing a copy of the image data from the passed image buffer.
		 */
		static cv::Mat copy(const ic4::ImageBuffer& buffer, ic4::Error& err = ic4::Error::Default())
		{
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return {};

			auto info = analyze_format(image_type.pixel_format());

			if (info.copy_fmt == ic4::PixelFormat::Invalid)
			{
				return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to copy image buffer", cv::Mat());
			}

			auto copy = cv::Mat(
				static_cast<int>(image_type.height()),
				static_cast<int>(image_type.width()),
				info.cv_type
			);

			auto copy_type = ic4::ImageType(info.copy_fmt, image_type.width(), image_type.height());
			auto copy_wrap = ic4::ImageBuffer::wrapMemory(
				copy.ptr(),
				static_cast<size_t>(copy.step[0]) * static_cast<size_t>(copy.rows),
				static_cast<ptrdiff_t>(copy.step[0]),
				copy_type,
				{}, err
			);

			if (!copy_wrap)
				return {};

			if (!copy_wrap->copyFrom(buffer, ic4::ImageBuffer::CopyOptions::SkipChunkData, err))
				return {};

			return ic4::detail::clearReturn(err, copy);
		}

	private:
		struct FormatInfo
		{
			bool can_wrap;
			int cv_type;
			ic4::PixelFormat copy_fmt;
		};

		static FormatInfo analyze_format(ic4::PixelFormat fmt)
		{
			switch (fmt)
			{
			case ic4::PixelFormat::Mono8:
			case ic4::PixelFormat::BayerBG8:
			case ic4::PixelFormat::BayerGB8:
			case ic4::PixelFormat::BayerGR8:
			case ic4::PixelFormat::BayerRG8:
				return { true, CV_8UC1, fmt };
			case ic4::PixelFormat::Mono16:
			case ic4::PixelFormat::BayerBG16:
			case ic4::PixelFormat::BayerGB16:
			case ic4::PixelFormat::BayerGR16:
			case ic4::PixelFormat::BayerRG16:
				return { true, CV_16UC1, fmt };
			case ic4::PixelFormat::BGRa8:
				return { true, CV_8UC4, fmt };
			case ic4::PixelFormat::BGRa16:
				return { true, CV_16UC4, fmt };
			case ic4::PixelFormat::BGR8:
				return { true, CV_8UC3, fmt };
			case ic4::PixelFormat::YCbCr422_8:
			case ic4::PixelFormat::YUV422_8:
				return { true, CV_8UC2, fmt };
			case ic4::PixelFormat::Mono10p:
			case ic4::PixelFormat::Mono12p:
			case ic4::PixelFormat::Mono12Packed:
				return { false, CV_16UC1, ic4::PixelFormat::Mono16 };
			case ic4::PixelFormat::BayerBG10p:
			case ic4::PixelFormat::BayerBG12p:
			case ic4::PixelFormat::BayerBG12Packed:
				return { false, CV_16UC1, ic4::PixelFormat::BayerBG16 };
			case ic4::PixelFormat::BayerGB10p:
			case ic4::PixelFormat::BayerGB12p:
			case ic4::PixelFormat::BayerGB12Packed:
				return { false, CV_16UC1, ic4::PixelFormat::BayerGB16 };
			case ic4::PixelFormat::BayerGR10p:
			case ic4::PixelFormat::BayerGR12p:
			case ic4::PixelFormat::BayerGR12Packed:
				return { false, CV_16UC1, ic4::PixelFormat::BayerGR16 };
			case ic4::PixelFormat::BayerRG10p:
			case ic4::PixelFormat::BayerRG12p:
			case ic4::PixelFormat::BayerRG12Packed:
				return { false, CV_16UC1, ic4::PixelFormat::BayerRG16 };
			default:
				return { false, -1, ic4::PixelFormat::Invalid };
			}
		}
	};
}