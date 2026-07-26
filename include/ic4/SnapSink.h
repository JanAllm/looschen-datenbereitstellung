
#ifndef IC4_snapsink_H_INC_
#define IC4_snapsink_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_SnapSink.h"
#include "C_ImageBuffer.h"

#include "Allocator.h"
#include "Sink.h"
#include "ImageBuffer.h"
#include "ImageType.h"
#include "Error.h"

#include <memory>
#include <chrono>
#include <algorithm>

namespace ic4
{
    class SnapSink;
    class Grabber;

    /**
     * @brief The snap sink is a sink implementation that allows a program to capture single images or sequences of images on demand,
     * while still having a display showing all images.
     * 
     * To create a snap sink, call @ref SnapSink::create().
     * 
     * To grab a single image out of the stream, call @ref SnapSink::snapSingle(). To grab a sequence of images, call @ref SnapSink::snapSequence().
     *
     * The snap sink manages the buffers used for background image aquisition as well as for the grabbed images.
     * During stream setup, a number of buffers is allocated depending on the configured allocation strategy.
     * Additional buffers can be automatically created on demand, if the allocation strategy allows.
     * Likewise, if there is a surplus of unused image buffers, unused buffers are reclaimed and released automatically.
     *
     * Image buffers that were returned by one of the \a snap functions are owned by their respective caller through a @c std::shared_ptr.
     * Once all @c std::shared_ptr pointers are reset, they are automatically returned to the snap sink for reuse.
     *
     * Please note that if there are no buffers available in the sink when the device tries to deliver a frame,
     * the frame will be dropped. Use @ref Grabber::streamStatistics() to find out whether a buffer underrun occurred.
     *
     * By default, the sink uses buffers provided by the device driver or the implicitly created transformation filter.
     * It is possible to use program-defined buffers be used by providing a @ref BufferAllocator to the sink creation function.
     */
    class SnapSink final : public Sink
    {
#ifndef IC4_DOXYGEN_INVISIBLE
        friend class Grabber;
#endif
    public:
        /**
         * @brief The buffer allocation strategy defines how many buffers are pre-allocated, when additional buffers are created,
         * and when excess buffers are reclaimed.
         */
        enum class AllocationStrategy
        {
            /**
             * @brief Use the default strategy
             *
             * This strategy pre-allocates an automatically selected number of buffers depending on the requirements of the data stream
             * and the image size.
             *
             * The @ref Config::customAllocationStrategy setting is ignored.
             */
            Default = ic4::c_interface::IC4_SNAPSINK_ALLOCATION_STRATEGY_DEFAULT,
            /**
             * @brief Custom allocation strategy
             *
             * The @ref CustomAllocationStrategy::num_buffers_allocate_on_connect, @ref CustomAllocationStrategy::num_buffers_allocation_threshold
             * @ref CustomAllocationStrategy::num_buffers_free_threshold and @ref CustomAllocationStrategy::num_buffers_max parameters determine
             * the sink's allocation behavior.
             */
            Custom = ic4::c_interface::IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM,
        };

        /**
         * @brief Specifies a custom allocation strategy.
         */
        struct CustomAllocationStrategy
        {
            /**
             * @brief Defines the number of buffers to auto-allocate when the stream is set up.
             */
            size_t num_buffers_allocate_on_connect;
            /**
             * @brief Defines the minimum number of required free buffers.
             *
             * If the number of free buffers falls below this, new buffers are allocated.
             */
            size_t num_buffers_allocation_threshold;
            /**
             * @brief Defines the maximum number of free buffers
             *
             * If the number of free buffers grows above this, buffers are freed.
             *
             * If set to @c 0, buffers are not freed automatically.
             *
             * @note
             * If @a num_buffers_free_threshold is not @c 0, it must be larger than @ref num_buffers_allocation_threshold @c + @c 2.
             */
            size_t num_buffers_free_threshold;
            /**
             * @brief Defines the maximum total number of buffers this sink will allocate.
             *
             * This includes both free buffers managed by the sink and filled buffers owned by the program.
             *
             * If set to @c 0, there is no limit to the total number of buffers.
             */
            size_t num_buffers_max;
        };

        /**
         * @brief The SnapSink configuration structure.
         * 
         * Passed to @ref SnapSink::create(const Config&, Error&) to configure the sink's behavior.
         */
        struct Config
        {
            /**
             * @brief Specifies the sink's buffer allocation strategy.
             *
             * The buffer allocation strategy defines how many buffers are pre-allocated, when additional buffers are created,
             * and when excess buffers are reclaimed.
             */
            AllocationStrategy allocationStrategy;
            /**
             * @brief Specifies the custom allocation strategy
             * 
             * This member is ignored unless @ref allocationStrategy is equal to @ref AllocationStrategy::Custom.
             */
            CustomAllocationStrategy customAllocationStrategy;

            /**
             * @brief A @c std::shared_ptr to a @ref BufferAllocator that replaces sink's default allocator.
             *
             * If this is @c nullptr, a default allocator is used.
             */
            std::shared_ptr<BufferAllocator> bufferAllocator;

            /**
             * @brief An array of possible pixel formats that the sink can receive.
             * 
             * If the vector is empty, all pixel formats are accepted.
             */
            std::vector<PixelFormat> acceptedPixelFormats;
        };

        /**
         * @brief Creates a SnapSink using the default configuration.
         * 
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @see create(const Config&, Error&)
         */
        static std::shared_ptr<SnapSink> create(Error& err = Error::Default())
        {
            return create(Config{}, err);
        }
        /**
         * @brief Creates a SnapSink using a customized buffer allocation strategy.
         *
         * @param[in] strategy  A custom buffer allocation strategy
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @see create(const Config&, Error&)
         */
        static std::shared_ptr<SnapSink> create(const CustomAllocationStrategy& strategy, Error& err = Error::Default())
        {
            Config config =
            {
                AllocationStrategy::Custom, // .allocationStrategy =
                strategy,                   // .customAllocationStrategy =
                {},                         // .bufferAllocator =
                {},                         // .acceptedImageTypes =
            };

            return create(config, err);
        }
        /**
         * @brief Creates a SnapSink using a custom buffer allocator.
         *
         * @param[in] allocator A custom buffer allocator
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see create(const Config&, Error&)
         */
        static std::shared_ptr<SnapSink> create(const std::shared_ptr<BufferAllocator>& allocator, Error& err = Error::Default())
        {
            Config config =
            {
                AllocationStrategy::Default,// .allocationStrategy =
                {},                         // .customAllocationStrategy =
                allocator,                  // .bufferAllocator =
                {},                         // .acceptedImageTypes =
            };

            return create(config, err);
        }
        /**
         * @brief Creates a SnapSink specifying a list of accepted image types.
         *
         * @param[in] acceptedPixelFormats  A @c std::vector containing the accepted pixel formats.
         * @param[out] err                  Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see create(const Config&, Error&)
         */
        static std::shared_ptr<SnapSink> create(const std::vector<PixelFormat>& acceptedPixelFormats, Error& err = Error::Default())
        {
            Config config =
            {
                AllocationStrategy::Default,// .allocationStrategy =
                {},                         // .customAllocationStrategy =
                {},                         // .bufferAllocator =
                acceptedPixelFormats        // .acceptedImageTypes =
            };

            return create(config, err);
        }
        /**
         * @brief Creates a SnapSink specifying an accepted image type.
         *
         * @param[in] acceptedPixelFormat   The pixel format accepted by the sink.
         * @param[out] err                  Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see create(const Config&, Error&)
         */
        static std::shared_ptr<SnapSink> create(PixelFormat acceptedPixelFormat, Error& err = Error::Default())
        {
            Config config =
            {
                AllocationStrategy::Default,// .allocationStrategy =
                {},                         // .customAllocationStrategy =
                {},                         // .bufferAllocator =
                { acceptedPixelFormat },    // .acceptedPixelFormats =
            };

            return create(config, err);
        }

        /**
         * @brief Creates a SnapSink using a configuration structure.
         * 
         * This generic overload allows specifying a combination of configuration options like allocation strategy,
         * buffer allocator and accepted frame types.
         *
         * @param[in] config        A configuration structure specifying sink behavior
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         */
        static std::shared_ptr<SnapSink> create(const Config& config, Error& err = Error::Default())
        {
            c_interface::IC4_SNAPSINK_CONFIG sink_config =
            {
                static_cast<c_interface::IC4_SNAPSINK_ALLOCATION_STRATEGY>(config.allocationStrategy),      // .strategy = 
                config.customAllocationStrategy.num_buffers_allocate_on_connect,                            // .num_buffers_alloc_on_connect = 
                config.customAllocationStrategy.num_buffers_allocation_threshold,                           // .num_buffers_allocation_threshold =
                config.customAllocationStrategy.num_buffers_free_threshold,                                 // .num_buffers_free_threshold =
                config.customAllocationStrategy.num_buffers_max,                                            // .num_buffers_max =
                reinterpret_cast<const c_interface::IC4_PIXEL_FORMAT*>(config.acceptedPixelFormats.data()), // .pixel_formats = 
                config.acceptedPixelFormats.size(),                                                         // .num_pixel_formats = 
            };

            if (config.bufferAllocator != nullptr)
            {
                sink_config.allocator = detail::AllocatorAdapter::callbacks();
                sink_config.allocator_context = detail::AllocatorAdapter::wrap(config.bufferAllocator);;
            };

            c_interface::IC4_SINK* ptr;
            if (!c_interface::ic4_snapsink_create(&ptr, &sink_config))
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, std::shared_ptr<SnapSink>(new SnapSink(ptr)));
        }

    public:
        /**
         * @brief Queries the image type of the images the sink is configured to receive.
         *
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  The image type the sink is configured for.
         *          If the function fails, the image type's pixel format is @ref PixelFormat::Invalid.
         *          Check the @a err output parameter for error code and error message.
         */
        ImageType outputImageType(Error& err = Error::Default()) const
        {
            ImageType result = {};

            c_interface::IC4_IMAGE_TYPE image_type;
            if (ic4_snapsink_get_output_image_type(ptr_, &image_type))
            {
                result = ImageType(&image_type);
                return detail::clearReturn(err, result);
            }
            return detail::updateFromLastErrorReturn(err, result);
        }

        /**
         * @brief Grabs a single image out of the video stream received from the video capture device.
         * 
         * @param timeout   Time to wait for a new image to arrive
         * @param[out] err  Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the filled image buffer, or @c nullptr in case of an error.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @pre
         * This operation is only valid while the sink is connected to a device in a data stream.
         * 
         * @note
         * After a successfull call, the returned @c std::shared_ptr owns the image buffer.\n
         * The pointer has to be reset to return the image buffer to the sink for reuse.
         */
        std::shared_ptr<ImageBuffer> snapSingle(std::chrono::milliseconds timeout, Error& err = Error::Default())
        {
            return snapSingle(timeout.count(), err);
        }
        /**
         * @brief Grabs a single image out of the video stream received from the video capture device.
         *
         * @param timeout_ms    Time to wait for a new image to arrive
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the filled image buffer, or @c nullptr in case of an error.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @pre
         * This operation is only valid while the sink is connected to a device in a data stream.
         *
         * @note
         * After a successfull call, the returned @c std::shared_ptr owns the image buffer.\n
         * The pointer has to be reset to return the image buffer to the sink for reuse.
         */
        std::shared_ptr<ImageBuffer> snapSingle(int64_t timeout_ms, Error& err = Error::Default())
        {
            c_interface::IC4_IMAGE_BUFFER* buffer = nullptr;
            if (!c_interface::ic4_snapsink_snap_single( ptr_, &buffer, timeout_ms))
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, detail::buffer_wrap(buffer));
        }

        /**
         * @brief Grabs a sequence of images out of the video stream received from the video capture device.
         *
         * This function waits until @a count images have been grabbed, or the timeout has expired.
         * If the timeout expires, the function returns the number of images grabber and the error value is set to @ref ErrorCode::Timeout.
         * 
         * @param count         Number of images to grab
         * @param timeout       Time to wait for all images to arrive
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::vector of @c std::shared_ptr to image buffers.\n
         *          If an error occurred, the vector's size is smaller than @a count.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @pre
         * This operation is only valid while the sink is connected to a device in a data stream.
         * 
         * @note
         * After a successfull call, the returned <tt>std::shared_ptr</tt>s own the image buffers.\n
         * All pointers have to be reset to return the image buffers to the sink for reuse.
         */
        std::vector<std::shared_ptr<ImageBuffer>> snapSequence(size_t count, std::chrono::milliseconds timeout, Error& err = Error::Default())
        {
            return snapSequence(count, timeout.count(), err);
        }
        /**
         * @brief Grabs a sequence of images out of the video stream received from the video capture device.
         *
         * This function waits until @a count images have been grabbed, or the timeout has expired.
         * If the timeout expires, the function returns the number of images grabber and the error value is set to @ref ErrorCode::Timeout.
         *
         * @param count         Number of images to grab
         * @param timeout_ms    Time to wait for all images to arrive
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::vector of @c std::shared_ptr to image buffers.\n
         *          If an error occurred, the vector's size is smaller than @a count.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @pre
         * This operation is only valid while the sink is connected to a device in a data stream.
         *
         * @note
         * After a successfull call, the returned <tt>std::shared_ptr</tt>s own the image buffers.\n
         * All pointers have to be reset to return the image buffers to the sink for reuse.
         */
        std::vector<std::shared_ptr<ImageBuffer>> snapSequence(size_t count, int64_t timeout_ms, Error& err = Error::Default())
        {
            std::vector<c_interface::IC4_IMAGE_BUFFER*> request((std::max)(count, size_t(1)));

            std::vector<std::shared_ptr<ImageBuffer>> result;

            size_t num_snapped = c_interface::ic4_snapsink_snap_sequence( ptr_, request.data(), count, timeout_ms);
            if (num_snapped == 0)
            {
                return detail::updateFromLastErrorReturn(err, result);
            }

            result.reserve(num_snapped);

            for (size_t i = 0; i < num_snapped; ++i)
            {
                result.push_back(detail::buffer_wrap(request[i]));
            }

            return detail::clearReturn(err, result);
        }

        SinkType sinkType() const noexcept final
        {
            return SinkType::SnapSink;
        }

    private:
        SnapSink(c_interface::IC4_SINK* p) noexcept
            : Sink(p)
        {
        }
    };
}

#endif // IC4_snapsink_H_INC_
