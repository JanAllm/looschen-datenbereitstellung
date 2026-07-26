
#include <ic4/ic4.h>
#include <HALCONCpp/HalconCpp.h>

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
	 * @brief Contains functions for using data @ref ic4::ImageBuffer objects in @a MVTec @a HALCON.
	 *
	 * The @a HALCON interop support functions are declared in @c ic4interop/interop-HALCON.h.
	 */
	class HALCON
	{
	public:
		/**
		 * @brief Wraps the passed image buffer in an @a HALCON @c HImage
		 *
		 * @param[in] buffer	An image buffer
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A @a HALCON @c HImage using the image buffer's memory as pixel storage
		 *
		 * @remarks
		 * Wrapping image buffer data is only supported for @c Mono8 and @c Mono16 image buffers that do not have horizontal padding.\n
		 * The program has to make sure that the image buffer is not released/requeued while the returned HImage is in use.
		 */
		static HalconCpp::HImage wrap(const ic4::ImageBuffer& buffer, ic4::Error& err = ic4::Error::Default())
		{
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return {};

			if (!canWrap(buffer))
			{
				return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to wrap image buffer", HalconCpp::HImage());
			}

			HalconCpp::HImage img;

			switch (image_type.pixel_format())
			{
			case ic4::PixelFormat::Mono8:
				img.GenImage1Extern("byte", image_type.width(), image_type.height(), buffer.ptr(), nullptr);
				break;
			case ic4::PixelFormat::Mono16:
				img.GenImage1Extern("uint2", image_type.width(), image_type.height(), buffer.ptr(), nullptr);
				break;
			default:
				return ic4::detail::updateReturn(err, ic4::ErrorCode::Internal, "Unexpected pixel format", img);
			}

			return ic4::detail::clearReturn(err, img);
		}

		/**
		 * @brief Checks whether @ref wrap can work on the passed image buffer.
		 *
		 * @param[in] buffer	An image buffer
		 *
		 * @return @c true, if the contents of the image buffer can be wrapped in a @a HALCON @c HImage, otherwise @c false.
		 *
		 * @remarks
		 * Wrapping image buffer data is only supported for @c Mono8 and @c Mono16 image buffers that do not have horizontal padding.
		 */
		static bool canWrap(const ic4::ImageBuffer& buffer)
		{
			ic4::Error err;
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return false;

			switch (image_type.pixel_format())
			{
			case ic4::PixelFormat::Mono8: return buffer.pitch() == image_type.width();
			case ic4::PixelFormat::Mono16: return buffer.pitch() == image_type.width() * 2;
			default:
				return false;
			}
		}

	private:
		static bool needExtraCopy(const ic4::ImageBuffer& buffer, ic4::PixelFormat& copyFormat)
		{
			auto image_type = buffer.imageType();

			switch (image_type.pixel_format())
			{
				// Copy Mono/RGB formats if the source buffer has horizontal padding
				// If there is no padding, GenImage1/GenImageInterleaved can work
			case ic4::PixelFormat::Mono8:
				if (buffer.pitch() != image_type.width())
				{
					copyFormat = image_type.pixel_format();
					return true;
				}
				else
				{
					return false;
				}
			case ic4::PixelFormat::Mono16:
				if (buffer.pitch() != image_type.width() * 2)
				{
					copyFormat = image_type.pixel_format();
					return true;
				}
				else
				{
					return false;
				}
			case ic4::PixelFormat::BGR8:
				if (buffer.pitch() != image_type.width() * 3)
				{
					copyFormat = image_type.pixel_format();
					return true;
				}
				else
				{
					return false;
				}
			case ic4::PixelFormat::BGRa8:
				if (buffer.pitch() != image_type.width() * 4)
				{
					copyFormat = image_type.pixel_format();
					return true;
				}
				else
				{
					return false;
				}
			case ic4::PixelFormat::BGRa16:
				if (buffer.pitch() != image_type.width() * 8)
				{
					copyFormat = image_type.pixel_format();
					return true;
				}
				else
				{
					return false;
				}

			case ic4::PixelFormat::BayerBG8:
			case ic4::PixelFormat::BayerGB8:
			case ic4::PixelFormat::BayerGR8:
			case ic4::PixelFormat::BayerRG8:
			case ic4::PixelFormat::YUV422_8:
			case ic4::PixelFormat::YCbCr422_8:
			case ic4::PixelFormat::YCbCr411_8:
			case ic4::PixelFormat::YCbCr411_8_CbYYCrYY:
				copyFormat = ic4::PixelFormat::BGR8;
				return true;

			case ic4::PixelFormat::Mono10p:
			case ic4::PixelFormat::Mono12p:
			case ic4::PixelFormat::Mono12Packed:
				copyFormat = ic4::PixelFormat::Mono16;
				return true;

			case ic4::PixelFormat::BayerBG10p:
			case ic4::PixelFormat::BayerBG12p:
			case ic4::PixelFormat::BayerBG12Packed:
			case ic4::PixelFormat::BayerGB10p:
			case ic4::PixelFormat::BayerGB12p:
			case ic4::PixelFormat::BayerGB12Packed:
			case ic4::PixelFormat::BayerGR10p:
			case ic4::PixelFormat::BayerGR12p:
			case ic4::PixelFormat::BayerGR12Packed:
			case ic4::PixelFormat::BayerRG10p:
			case ic4::PixelFormat::BayerRG12p:
			case ic4::PixelFormat::BayerRG12Packed:
			case ic4::PixelFormat::BayerBG16:
			case ic4::PixelFormat::BayerGB16:
			case ic4::PixelFormat::BayerGR16:
			case ic4::PixelFormat::BayerRG16:
				copyFormat = ic4::PixelFormat::BGRa16;
				return true;
			default:
				copyFormat = ic4::PixelFormat::Invalid;
				return false;
			}
		}

		static HalconCpp::HImage copyInternal(const ic4::ImageBuffer& buffer, ic4::Error& err = ic4::Error::Default())
		{
			auto image_type = buffer.imageType(ic4::Error::Ignore());

			HalconCpp::HImage copy;

			auto w = image_type.width();
			auto h = image_type.height();

			switch (image_type.pixel_format())
			{
			case ic4::PixelFormat::Mono8:
				copy.GenImage1("byte", w, h, buffer.ptr());
				break;
			case ic4::PixelFormat::Mono16:
				copy.GenImage1("uint2", w, h, buffer.ptr());
				break;
			case ic4::PixelFormat::BGR8:
				copy.GenImageInterleaved(buffer.ptr(), "bgr", w, h, -1, "byte", w, h, 0, 0, -1, 0);
				break;
			case ic4::PixelFormat::BGRa8:
				copy.GenImageInterleaved(buffer.ptr(), "bgrx", w, h, -1, "byte", w, h, 0, 0, -1, 0);
				break;
			case ic4::PixelFormat::BGRa16:
				copy.GenImageInterleaved(buffer.ptr(), "bgrx64", w, h, -1, "uint2", w, h, 0, 0, -1, 0);
				break;
			default:
				return ic4::detail::updateReturn(err, ic4::ErrorCode::Internal, "Unexpected pixel format", HalconCpp::HImage());
			}

			return ic4::detail::clearReturn(err, copy);
		}

	public:
		/**
		 * @brief Creates a copy of the image buffer and stores it in a @a HALCON @c HImage.
		 *
		 * @param[in] buffer	An image buffer
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A @a HALCON @c HImage containing a copy of the image data from the passed image buffer.
		 */
		static HalconCpp::HImage copy(const ic4::ImageBuffer& buffer, ic4::Error& err = ic4::Error::Default())
		{
			auto image_type = buffer.imageType(err);
			if (err.isError())
				return {};

			ic4::PixelFormat tmp_format;
			if (needExtraCopy(buffer, tmp_format))
			{
				auto pool = ic4::BufferPool::create(err);
				if (pool == nullptr)
					return {};

				auto tmp_type = ic4::ImageType(tmp_format, image_type.width(), image_type.height());
				auto tmp = pool->getBuffer(tmp_type, err);
				if (tmp == nullptr)
					return {};

				if (!tmp->copyFrom(buffer, ic4::ImageBuffer::CopyOptions::SkipChunkData, err))
					return {};

				return copyInternal(*tmp, err);
			}

			return copyInternal(buffer, err);
		}

	private:
		template<typename TElement>
		static void packBGRx(TElement* red, TElement* green, TElement* blue, ic4::ImageBuffer& dest)
		{
			auto image_type = dest.imageType();
			auto width = image_type.width();
			auto height = image_type.height();
			auto pitch = dest.pitch();

			struct Destination
			{
				TElement B, G, R, A;
			};

			auto* dest_flat = static_cast<uint8_t*>(dest.ptr());

			for (unsigned int y = 0; y < height; ++y)
			{
				auto* r = &red[y * width];
				auto* g = &green[y * width];
				auto* b = &blue[y * width];
				auto* d = reinterpret_cast<Destination*>(&dest_flat[y * pitch]);

				for (unsigned int x = 0; x < width; ++x)
				{
					d[x].B = b[x];
					d[x].G = g[x];
					d[x].R = r[x];
					d[x].A = static_cast<TElement>(~TElement());
				}
			}
		}

		template<typename TElement>
		static void packBGRx(void* red, void* green, void* blue, ic4::ImageBuffer& dest)
		{
			return packBGRx(static_cast<TElement*>(red), static_cast<TElement*>(green), static_cast<TElement*>(blue), dest);
		}

		template<typename TElement>
		static bool copyBGRx(void* red, void* green, void* blue, ic4::PixelFormat fmt, uint32_t width, uint32_t height, std::function<ic4::ImageBuffer* (const ic4::ImageType&, ic4::Error&)> get_dest, ic4::Error& err)
		{
			auto dest_type = ic4::ImageType(fmt, width, height);

			auto* dest = get_dest(dest_type, err);
			if (dest == nullptr)
				return false;

			if (dest->imageType() != dest_type)
			{
				return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Incompatible destination buffer format", false);
			}

			packBGRx<TElement>(red, green, blue, *dest);

			return ic4::detail::clearReturn(err, true);
		}

		static bool copySingleChannel(void* ptr, ic4::PixelFormat fmt, uint32_t width, uint32_t height, std::function<ic4::ImageBuffer* (const ic4::ImageType&, ic4::Error&)> get_dest, ic4::Error& err)
		{
			auto tmpType = ic4::ImageType(fmt, width, height);
			auto bypp = ic4::getBitsPerPixel(fmt) / 8;
			auto tmp = ic4::ImageBuffer::wrapMemory(ptr, width * height * bypp, width * bypp, tmpType, {}, err);
			if (tmp == nullptr)
				return false;

			auto* dest = get_dest(tmpType, err);
			if (dest == nullptr)
				return false;

			if (!dest->copyFrom(*tmp, ic4::ImageBuffer::CopyOptions::Default, err))
				return false;

			return ic4::detail::clearReturn(err, true);
		}

		static bool copyInternal(const HalconCpp::HImage& src, std::function<ic4::ImageBuffer*(const ic4::ImageType&, ic4::Error&)> get_dest, ic4::Error& err)
		{
			if (src.CountChannels() == 1)
			{
				HalconCpp::HString type;
				Hlong width, height;
				void* ptr = src.GetImagePointer1(&type, &width, &height);

				if (type == "byte")
				{
					return copySingleChannel(ptr, ic4::PixelFormat::Mono8, static_cast<uint32_t>(width), static_cast<uint32_t>(height), get_dest, err);
				}
				else if (type == "uint2")
				{
					return copySingleChannel(ptr, ic4::PixelFormat::Mono16, static_cast<uint32_t>(width), static_cast<uint32_t>(height), get_dest, err);
				}
				else
				{
					return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to copy from image with type not in {byte, uint2}", false);
				}
			}
			else if (src.CountChannels() == 3)
			{
				void* red;
				void* green;
				void* blue;
				HalconCpp::HString type;
				Hlong width, height;
				src.GetImagePointer3(&red, &green, &blue, &type, &width, &height);

				if (type == "byte")
				{
					return copyBGRx<uint8_t>(red, green, blue, ic4::PixelFormat::BGRa8, static_cast<uint32_t>(width), static_cast<uint32_t>(height), get_dest, err);
				}
				else if (type == "uint2")
				{
					return copyBGRx<uint16_t>(red, green, blue, ic4::PixelFormat::BGRa16, static_cast<uint32_t>(width), static_cast<uint32_t>(height), get_dest, err);
				}
				else
				{
					return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to copy from image with type not in {byte, uint2}", false);
				}
			}
			else
			{
				return ic4::detail::updateReturn(err, ic4::ErrorCode::InvalidOperation, "Unable to copy from image with CountChannels not in {1, 3}", false);
			}
		}

	public:
		/**
		 * @brief Copies the contents of a @a HALCON @c HImage into a new @ref ic4::ImageBuffer.
		 * 
		 * @param[in] src		A @a HALCON @c HImage
		 * @param[in] pool		A buffer pool to query the new image buffer from
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return A new image buffer. If the function fails, @c nullptr is returned.
		 * 
		 * @remarks
		 * The resulting image buffer's pixel format depends on the type of @a src:
		 * - If @a src is a 1-channel image of type @c byte, the pixel format for the returned image buffer is @ref ic4::PixelFormat::Mono8.
		 * - If @a src is a 1-channel image of type @c uint2, the pixel format for the returned image buffer is @ref ic4::PixelFormat::Mono16.
		 * - If @a src is a 3-channel image of type @c byte, the pixel format for the returned image buffer is @ref ic4::PixelFormat::BGRa8.
		 * - If @a src is a 3-channel image of type @c uint2, the pixel format for the returned image buffer is @ref ic4::PixelFormat::BGRa16.
		 * - Other image types are not supported.
		 */
		static std::shared_ptr<ic4::ImageBuffer> copy(const HalconCpp::HImage& src, std::shared_ptr<ic4::BufferPool> pool, ic4::Error& err = ic4::Error::Default())
		{
			std::shared_ptr<ic4::ImageBuffer> result;

			if (!copyInternal(src,
				[&result, pool](const ic4::ImageType& imageType, ic4::Error& err) -> ic4::ImageBuffer*
				{
					result = pool->getBuffer(imageType, err);
					return result.get();
				},
				err
			))
			{
				return nullptr;
			}

			return result;
		}

		/**
		 * @brief Copies the contents of a @a HALCON @c HImage into a new @ref ic4::ImageBuffer.
		 *
		 * @param[in] src		A @a HALCON @c HImage
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A new image buffer. If the function fails, @c nullptr is returned.
		 *
		 * @remarks
		 * The resulting image buffer's pixel format depends on the type of @a src:
		 * - If @a src is a 1-channel image of type @c byte, the pixel format for the returned image buffer is @ref ic4::PixelFormat::Mono8.
		 * - If @a src is a 1-channel image of type @c uint2, the pixel format for the returned image buffer is @ref ic4::PixelFormat::Mono16.
		 * - If @a src is a 3-channel image of type @c byte, the pixel format for the returned image buffer is @ref ic4::PixelFormat::BGRa8.
		 * - If @a src is a 3-channel image of type @c uint2, the pixel format for the returned image buffer is @ref ic4::PixelFormat::BGRa16.
		 * - Other image types are not supported.
		 */
		static std::shared_ptr<ic4::ImageBuffer> copy(const HalconCpp::HImage& src, ic4::Error& err = ic4::Error::Default())
		{
			return copy(src, ic4::BufferPool::create(), err);
		}

		/**
		 * @brief Copies the contents of a @a HALCON @c HImage into an @ref ic4::ImageBuffer.
		 *
		 * @param[in] src		A @a HALCON @c HImage
		 * @param[out] dest		A destination buffer to receive the image data
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @remarks
		 * The image type of @a dest needs to match the dimensions and type of @a src.
		 * - If @a src is a 1-channel image of type @c byte, @a dest must be @ref ic4::PixelFormat::Mono8 of the same size.
		 * - If @a src is a 1-channel image of type @c uint2, @a dest must be @ref ic4::PixelFormat::Mono16 of the same size.
		 * - If @a src is a 3-channel image of type @c byte, @a dest must be @ref ic4::PixelFormat::BGRa8 of the same size.
		 * - If @a src is a 3-channel image of type @c uint2, @a dest must be @ref ic4::PixelFormat::BGRa16 of the same size.
		 * - Other image types are not supported.
		 */
		static bool copy(const HalconCpp::HImage& src, ic4::ImageBuffer& dest, ic4::Error& err = ic4::Error::Default())
		{
			return copyInternal(src, 
				[&dest](auto&, auto&) -> ic4::ImageBuffer*
				{
					return &dest;
				},
				err
			);
		}
	};
}