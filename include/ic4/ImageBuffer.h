
#ifndef IC4_IMAGEBUFFER_H_INC_
#define IC4_IMAGEBUFFER_H_INC_

#define IC4_C_IN_NAMESPACE
#include "C_ImageBuffer.h"

#include "ImageType.h"
#include "Error.h"
#include "HandleRef.h"

#include <cstdint>
#include <memory>

namespace ic4
{
    class ImageBuffer;

    namespace detail
    {
        c_interface::IC4_IMAGE_BUFFER* buffer_ptr(const ImageBuffer& buffer) noexcept;
        c_interface::IC4_IMAGE_BUFFER* buffer_ptr(const std::shared_ptr<ImageBuffer>& buffer) noexcept;
        std::shared_ptr<ImageBuffer> buffer_wrap(c_interface::IC4_IMAGE_BUFFER* buffer);
    }

    /**
     * @brief Represents an image buffer
     * 
     * Image buffer objects are created automatically by the various @ref Sink types.
     * They can also be created manually on request by a @ref BufferPool, or by calling @ref ImageBuffer::wrapMemory().
     * 
     * Programs use image buffers via @c std::shared_ptr<ImageBuffer>. If all pointers are released, the image buffer object is returned
     * to its source for reuse. For example, an image buffer retrieved from @ref QueueSink::popOutputBuffer() will be re-queued.
     */
    class ImageBuffer
    {
    private:
        detail::FixedHandleRef<c_interface::IC4_IMAGE_BUFFER, c_interface::ic4_imagebuffer_unref> ptr_;
        mutable ic4::ImageType ftype_;

#ifndef IC4_DOXYGEN_INVISIBLE
        friend c_interface::IC4_IMAGE_BUFFER* detail::buffer_ptr(const ImageBuffer& buffer) noexcept;
        friend c_interface::IC4_IMAGE_BUFFER* detail::buffer_ptr(const std::shared_ptr<ImageBuffer>& buffer) noexcept;
        friend std::shared_ptr<ImageBuffer> detail::buffer_wrap(c_interface::IC4_IMAGE_BUFFER* buffer);
#endif

        ImageBuffer(c_interface::IC4_IMAGE_BUFFER* buffer) noexcept
            : ptr_(buffer)
        {
        }

    public:
        /**
         * @brief Queries information about the image buffers's image data type.
         * 
         * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
         * 
         * @return The type of image that can be stored in the image buffer.
         */
        const ImageType& imageType(Error& err = Error::Default()) const
        {
            c_interface::IC4_IMAGE_TYPE ftype = {};

            if (!c_interface::ic4_imagebuffer_get_image_type(ptr_, &ftype))
            {
                return detail::updateFromLastErrorReturn(err, ftype_);
            }

            ftype_ = ImageType(&ftype);
            return detail::clearReturn(err, ftype_);
        }

        /**
         * @brief Returns a pointer to the data managed by the image buffer.
         * 
         * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A pointer to the image data, or @c nullptr if an error occurred.\n
		 *			Check the @c err output parameter for details.
         * 
         * @remarks The memory pointed to by the returned pointer is valid as long as the image buffer object exists.
         */
        void* ptr(Error& err = Error::Default()) const
        {
            void* ptr = c_interface::ic4_imagebuffer_get_ptr(ptr_);
            if (!ptr)
                return detail::updateFromLastErrorReturn(err, nullptr);

            return detail::clearReturn(err, ptr);
        }

        /**
         * @brief Returns the size of the image buffer.
         * 
         * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  The size of the image buffer, or @c 0 if an error occurred.\n
		 *			Check the @c err output parameter for details.
         */
        size_t bufferSize(Error& err = Error::Default()) const
        {
            size_t sz = c_interface::ic4_imagebuffer_get_buffer_size(ptr_);
            if( !sz )
                return detail::updateFromLastErrorReturn<size_t>(err, 0);

            return detail::clearReturn(err, sz);
        }

        /**
         * @brief Returns the pitch for the image buffer.
         * 
         * The pitch is the distance between the starting memory location of two consecutive lines.
         * 
         * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  The pitch of the image buffer, or @c 0 if an error occurred.\n
         *			Check the @c err output parameter for details.
         */
        ptrdiff_t pitch(Error& err = Error::Default()) const
        {
            ptrdiff_t pitch = c_interface::ic4_imagebuffer_get_pitch(ptr_);
            if (!pitch)
                return detail::updateFromLastErrorReturn<ptrdiff_t>(err, 0);

            return detail::clearReturn(err, pitch);
        }

        /**
         * @brief A structure containing frame metadata
         */
        struct MetaData
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
         * @brief Retrieves frame metadata from an image buffer object.
         * 
         * @param[out] err		    Reference to an error handler. See @ref technical_article_error_handling for details.
         * 
         * @return A @ref MetaData structur containing frame metadata
         */
        MetaData metaData(Error& err = Error::Default())
        {
            MetaData result = {};

            c_interface::IC4_FRAME_METADATA cmeta = {};
            if (!c_interface::ic4_imagebuffer_get_metadata(ptr_, &cmeta))
                return detail::updateFromLastErrorReturn(err, result);

            result.device_frame_number = cmeta.device_frame_number;
            result.device_timestamp_ns = cmeta.device_timestamp_ns;

            return detail::clearReturn(err, result);
        }

        /**
         * @brief Contains options to configure the behavior of @ref ImageBuffer::copyFrom().
         */
        enum class CopyOptions : int
        {
            /**
             * @brief Default behavior. Copy image data, meta data and chunk data.
             */
            Default = 0,
            /**
             * @brief Instructs @ref ImageBuffer::copyFrom() to skip the image data when copying.
             *
             * If included in the @c flags argument, @ref ImageBuffer::copyFrom() only copies the non-image parts of the buffer, and a
             * program-defined algorithm can handle the image copy operation.
             */
            SkipImage = c_interface::IC4_IMAGEBUFFER_COPY_SKIP_IMAGE,
            /**
             * @brief Instructs @ref ImageBuffer::copyFrom() to not copy the chunk data.
             *
             * This can be useful if the chunk data is large and not required in the destination frame.
             */
            SkipChunkData = c_interface::IC4_IMAGEBUFFER_COPY_SKIP_CHUNKDATA
        };        

        /**
         * @brief Copies the contents of one image buffer to another image buffer.
         * 
         * @param[in] buffer        Source buffer to copy from
         * @param[in] options       A bitwise combination of @ref CopyOptions to customize the copy operation
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return @c true on success, otherwise @c false.\n
         *			Check the @c err output parameter for details.
         * 
         * @remarks
         * If the pixel format of the images in @c source and @c destination is not equal, the image is converted. For example,
         * if the pixel format of @c source is @ref PixelFormat::BayerRG8 and the pixel format of @c destination is @ref PixelFormat::BGR8,
         * a demosaicing operation creates a color image.\n
         * If @c flags contains @ref CopyOptions::SkipImage, the function does not copy the image data. The function then only copies the meta data, and a
         * program-defined algorithm can handle the image copy operation.\n
         * If @c flags contains @ref CopyOptions::SkipChunkData, the function does not copy the chunk data contained in @c source. This can be useful if the
         * chunk data is large and not required.
         *
         * @note
         * If the width or height of @c source and @c destination are not equal, the function fails and the error value is set to @ref ErrorCode::ConversionNotSupported. \n
         * If there is no algorithm available for the requested conversion, the function fails and the error value is set to @ref ErrorCode::ConversionNotSupported. \n
         * If the @c destination image buffer is not writable, the function fails and the error value is set to @ref ErrorCode::InvalidOperation. \n
         */
        bool copyFrom(const ImageBuffer& buffer, CopyOptions options = CopyOptions::Default, Error& err = Error::Default())
        {
            return detail::returnUpdateFromLastError(err, c_interface::ic4_imagebuffer_copy(buffer.ptr_, ptr_, static_cast<unsigned>(options)));
        }

        /**
         * @brief Checks whether an image buffer object is (safely) writable.
         *
         * In some situations, image buffer objects are shared between the application holding a handle to the image buffer object
         * and the library. For example, the image buffer might be shared with a display or a video writer.
         *
         * A shared buffer is not safely writable. Writing to a buffer that is shared can lead to unexpected behavior, for example a
         * modification may partially appear in the result of an operation that is happening in parallel.
         *
         * Passing the image buffer into a function such as @ref Display::displayBuffer() or @ref VideoWriter::addFrame() can lead to a buffer becoming shared.
         * 
         * @return	@c true if the image buffer is not shared with any part of the library, and is therefore safely writable.\n
         *			@c false, if the image buffer is shared and therefore should not be written into.
         */
        bool isWritable() const
        {
            return c_interface::ic4_imagebuffer_is_writable(ptr_);
        }

        /**
         * @brief Defines a callback function to be called when an image buffer created by @ref ImageBuffer::wrapMemory
         * is destroyed and the image data will no longer be accessed through it.
         *
         * @param[in] ptr			Pointer to the image data as passed as @a data into @ref wrapMemory
         * @param[in] bufferSize	Buffer size as passed as @a buffer_size into @ref wrapMemory
         */
        using ReleaseMemoryHandler = std::function<void(void* ptr, size_t bufferSize)>;

        /**
         * @brief Creates an image buffer object using external memory as storage area for the image data.
         *
         * This function can be useful when copying image data into buffers of third-party libraries:
         * - Create an image object in the third-party library
         * - Wrap the third-party library's image data into an @ref ImageBuffer using @ref ImageBuffer::wrapMemory().
         * - Copy the data from an existing image buffer object into the third-party image buffer using @ref ImageBuffer::copyFrom().
         * 
         * @param[in] data          Pointer to a region of memory to be used as image data by the image buffer object
         * @param[in] bufferSize    Size of the region of memory pointed to by @a data
         * @param[in] pitch         Difference between memory addresses of two consecutive lines of image data
         * @param[in] imageType     Type of image to be stored in the image buffer
         * @param[in] on_release    Optional callback function to be called when the image buffer object is destroyed.\n
         *                          This can be useful when the program needs to keep track of the memory pointer to by the image buffer
         *                          and has to release it once the image buffer object no longer exists.\n
         *                          The lifetime of the image buffer can potentially be extended beyond the existance of its handle
         *                          when it is shared with API functions, e.g. @ref Display::displayBuffer or @ref VideoWriter::addFrame.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         * 
         * @return The new image buffer, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<ImageBuffer> wrapMemory(void* data, size_t bufferSize, ptrdiff_t pitch, const ImageType& imageType, ReleaseMemoryHandler on_release = {}, Error& err = Error::Default())
        {
            c_interface::ic4_imagebuffer_memory_release release_cb = nullptr;
            void* release_user_ptr = nullptr;
            if (on_release)
            {
                release_cb = releaseMemoryCallback;
                release_user_ptr = new ReleaseMemoryData{ on_release };
            }

            c_interface::IC4_IMAGE_BUFFER* buf = nullptr;
            if (!c_interface::ic4_imagebuffer_wrap_memory(&buf, data, bufferSize, pitch, &imageType.data, release_cb, release_user_ptr))
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, detail::buffer_wrap(buf));
        }
    private:
        struct ReleaseMemoryData
        {
            ReleaseMemoryHandler handler;
        };

        static void releaseMemoryCallback(void* ptr, size_t buffer_size, void* user_ptr)
        {
            auto* data = static_cast<ReleaseMemoryData*>(user_ptr);
            data->handler(ptr, buffer_size);
            delete data;
        }
    };

#ifndef IC4_DOXYGEN_INVISIBLE
    inline ImageBuffer::CopyOptions operator|(ImageBuffer::CopyOptions a, ImageBuffer::CopyOptions b)
    {
        return static_cast<ImageBuffer::CopyOptions>(static_cast<int>(a) | static_cast<int>(b));
    }
    inline ImageBuffer::CopyOptions& operator|=(ImageBuffer::CopyOptions& a, ImageBuffer::CopyOptions b)
    {
        a = a | b;
        return a;
    }
#endif

    namespace detail
    {
        inline c_interface::IC4_IMAGE_BUFFER* buffer_ptr(const ImageBuffer& buffer) noexcept
        {
            return buffer.ptr_;
        }
        inline c_interface::IC4_IMAGE_BUFFER* buffer_ptr( const std::shared_ptr<ImageBuffer>& buffer ) noexcept
        {
            if( !buffer )    return nullptr;
            return buffer->ptr_;
        }
        inline std::shared_ptr<ImageBuffer> buffer_wrap(c_interface::IC4_IMAGE_BUFFER* buffer)
        {
            return std::shared_ptr<ImageBuffer>(new ImageBuffer(buffer));
        }
    }
}

#endif //IC4_IMAGEBUFFER_H_INC_