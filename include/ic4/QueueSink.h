
#ifndef IC4_queuesink_H_INC_
#define IC4_queuesink_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_QueueSink.h"
#include "C_ImageBuffer.h"

#include "Allocator.h"
#include "Sink.h"
#include "ImageBuffer.h"
#include "ImageType.h"

#include <memory>
#include <functional>

namespace ic4
{
    class QueueSink;
    class Grabber;
    
    /**
     * @brief Abstract base class for QueueSink notifications.
     */
    class QueueSinkListener
    {
    public:
        virtual ~QueueSinkListener() = default;

        /**
         * @brief Called when the data stream to the sink is created
         * 
         * @param[in] sink                  The sink object
         * @param[in] imageType             The negotiated image type that the sink will receive
         * @param[in] min_buffers_required  The minimum number of buffers required by the device to start a stream
         * 
         * @return  @c true, if the data stream should be created.
         *          If @c false is returned the call to @ref Grabber::streamSetup() will fail.
         * 
         * @note
         * The function is executed on the thread that calls @ref Grabber::streamSetup().
		 *
		 * @note
		 * If the callback function does not allocate any buffers, the sink will automatically allocate the minimum number of buffers required.
         */
        virtual bool    sinkConnected(QueueSink& sink, const ImageType& imageType, size_t min_buffers_required)
        {
            (void)sink; (void)imageType; (void)min_buffers_required;
            return true;
        }

        /**
         * @brief Called when the data stream to the sink is stopped
         * 
         * @param[in] sink      The sink object
         * 
         * @note
         * The function is executed on the thread that calls @ref Grabber::streamStop().
         */
        virtual void    sinkDisconnected(QueueSink& sink) { (void)sink; }

        /**
         * @brief Called when new images were added to the sink's queue of filled buffers.
         * 
         * This callback usually calls @ref QueueSink::popOutputBuffer() to access the filled image buffers.
         * 
         * @param[in] sink      The sink object
         * 
         * @note
         * If this callback function performs a lengthy operation, it is recommended to regularly check @ref QueueSink::isCancelRequested()
         * to determine whether the data stream is being stopped.
         * 
         * @note
         * The function is executed on dedicated thread managed by the sink.
         * 
         * @warning
         * When the data stream to the sink is stopped, the @ref Grabber::streamStop() call wait until this function returns.
         * This can quickly lead to a deadlock, if code in the \a framesQueued callback performs an operation that unconditionally requires
         * activity on the thread that called @ref Grabber::streamStop().
         */
        virtual void    framesQueued(QueueSink& sink) = 0;
    };

    /**
     * @brief A sink implementation that allows a program to process all images received from a video capture device.
     * 
     * A queue sink manages a number of buffers that are organized in two queues:
     *  - A free queue that buffers are pulled from to fill with data from the device
     *  - An output queue that contains the filled buffers ready to be picked up by the program
     * 
     * To create a queue sink, call @ref QueueSink::create().
	 *
	 * Pass the sink to @ref Grabber::streamSetup() to feed images into the sink.
     * 
     * Usually, the queue sink is interacted with by deriving a class from @ref QueueSinkListener and implementing
     * its member functions.
     * The functions are called at different significant points in the lifetime of a queue sink:
     *  - @ref QueueSinkListener::sinkConnected() is called when a data stream is being set up from the device to the sink.
     *      The callback is responsible for making sure there are enough buffers queued for streaming to begin.
     *  - @ref QueueSinkListener::framesQueued() is called whenever there are images available in the output queue.
     *  - @ref QueueSinkListener::sinkDisconnected() is called when a previously-created data stream is stopped.
     * 
     * To retrieve the oldest available image from the output queue, call @ref QueueSink::popOutputBuffer().
     * The image buffer is owned by the returned @c shared_ptr. The caller is responsible to reset the @c shared_ptr
     * at a later time to return the image buffer to the sink's free queue.
     * 
     * A program does not necessarily have to requeue all image buffers immediately; it can choose to store pointers to a number of them
     * in its own data structures. However, please note that if there are no buffers in the free queue when the device tries to deliver a frame,
     * the frame will be dropped. Use @ref Grabber::streamStatistics() to find out whether a buffer underrun occurred.
     */
    class QueueSink final : public Sink
    {
        class ListenerAdapter;

#ifndef IC4_DOXYGEN_INVISIBLE
        friend class Grabber;
#endif

    public:
        /**
         * @brief The QueueSink configuration structure.
         *
         * Passed to @ref QueueSink::create(QueueSinkListener&, const Config&, Error&),
         * @ref QueueSink::create(const std::shared_ptr<QueueSinkListener>&, const Config&, Error&)
         * or @ref QueueSink::create(std::function<void(ic4::QueueSink&)>, const Config&, Error&)
         * to configure the sink's behavior.
         */
        struct Config
        {
            /**
             * @brief Defines the maximum number of buffers that are stored in the sink's output queue.
             *
             * If set to @c 0, the number of buffers is unlimited.
             * If a new frame arrives at the sink, and the output queue size would exceed @a maxOutputBuffers,
             * the oldest image is discarded and its buffer is added to the free queue.
             */
            size_t maxOutputBuffers = 0;
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
         * @brief Creates a new queue sink using the default configuration.
         * 
         * @param[in] cb        A reference to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @remarks The program has to make sure that the passed listener exists as long as the sink does.
         *
         * @see @ref create(QueueSinkListener&, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(QueueSinkListener& cb, Error& err = Error::Default())
        {
            return create_internal(err, ListenerAdapter::wrap(cb), {});
        }
        /**
         * @brief Creates a new queue sink using the default configuration.
         *
         * @param[in] cb        A @c std::shared_ptr to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see @ref create(const std::shared_ptr<QueueSinkListener>&, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(const std::shared_ptr<QueueSinkListener>& cb, Error& err = Error::Default())
        {
            if (cb == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "cb == nullptr", nullptr);

            return create_internal(err, ListenerAdapter::wrap(cb), {});
        }
        /**
         * @brief Creates a new queue sink using the default configuration.
         *
         * @param[in] frames_queued A function to be called when new frames have been queued in the sink.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see @ref create(std::function<void(ic4::QueueSink&)>, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(std::function<void(ic4::QueueSink&)> frames_queued, Error& err = Error::Default())
        {
            if (frames_queued == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "frames_queued == nullptr", nullptr);

            return create_internal(err, FramesQueuedAdapter::wrap(frames_queued), {});
        }
        /**
         * @brief Creates a new queue sink specifying a pixel format for the sink.
         *
         * @param[in] cb            A reference to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[in] sink_format   The pixel format of the buffers received by the sink.\n
         *                          If this differs from the device's pixel format, the image data is transformed automatically.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @remarks The program has to make sure that the passed listener exists as long as the sink does.
         *
         * @see @ref create(QueueSinkListener&, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(QueueSinkListener& cb, PixelFormat sink_format, Error& err = Error::Default())
        {
            Config config = {};
            config.acceptedPixelFormats.push_back(sink_format);

            return create_internal(err, ListenerAdapter::wrap(cb), config);
        }
        /**
         * @brief Creates a new queue sink specifying a pixel format for the sink.
         *
         * @param[in] cb            A @c std::shared_ptr to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[in] sink_format   The pixel format of the buffers received by the sink.\n
         *                          If this differs from the device's pixel format, the image data is transformed automatically.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see @ref create(const std::shared_ptr<QueueSinkListener>&, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(const std::shared_ptr<QueueSinkListener>& cb, PixelFormat sink_format, Error& err = Error::Default())
        {
            if (cb == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "cb == nullptr", nullptr);

            Config config = {};
            config.acceptedPixelFormats.push_back(sink_format);

            return create_internal(err, ListenerAdapter::wrap(cb), config);
        }
        /**
         * @brief Creates a new queue sink specifying a pixel format for the sink.
         *
         * @param[in] frames_queued A function to be called when new frames have been queued in the sink.
         * @param[in] sink_format   The pixel format of the buffers received by the sink.\n
         *                          If this differs from the device's pixel format, the image data is transformed automatically.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         *
         * @see @ref create(std::function<void(ic4::QueueSink&)>, const Config&, Error&)
         */
        static std::shared_ptr<QueueSink> create(std::function<void(ic4::QueueSink&)> frames_queued, PixelFormat sink_format, Error& err = Error::Default())
        {
            if (frames_queued == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "frames_queued == nullptr", nullptr);

            Config config = {};
            config.acceptedPixelFormats.push_back(sink_format);

            return create_internal(err, FramesQueuedAdapter::wrap(frames_queued), config);
        }
        /**
         * @brief Creates a new queue sink using a configuration structure.
         * 
         * This generic overload allows specifying a combination of configuration options like
         * buffer allocator and accepted frame types.
         *
         * @param[in] cb        A reference to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[in] config    A configuration structure specifying sink behavior
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @remarks The program has to make sure that the passed listener exists as long as the sink does.
         */
        static std::shared_ptr<QueueSink> create(QueueSinkListener& cb, const Config& config, Error& err = Error::Default())
        {
            return create_internal(err, ListenerAdapter::wrap(cb), config);
        }
        /**
         * @brief Creates a new queue sink using a configuration structure.
         *
         * This generic overload allows specifying a combination of configuration options like
         * buffer allocator and accepted frame types.
         *
         * @param[in] cb        A @c std::shared_ptr to a QueueSinkListener-derived listener receiving the sink callbacks.
         * @param[in] config    A configuration structure specifying sink behavior
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         */
        static std::shared_ptr<QueueSink> create(const std::shared_ptr<QueueSinkListener>& cb, const Config& config, Error& err = Error::Default())
        {
            if (cb == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "cb == nullptr", nullptr);

            return create_internal(err, ListenerAdapter::wrap(cb), config);
        }
        /**
         * @brief Creates a new queue sink using a configuration structure.
         *
         * This generic overload allows specifying a combination of configuration options like
         * buffer allocator and accepted frame types.
         *
         * @param[in] frames_queued A function to be called when new frames have been queued in the sink.
         * @param[in] config        A configuration structure specifying sink behavior
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the new sink, or @c nullptr if an error occurred.\n
         *          Check the @a err output parameter for error code and error message.
         */
        static std::shared_ptr<QueueSink> create(std::function<void(ic4::QueueSink&)> frames_queued, const Config& config, Error& err = Error::Default())
        {
            if (frames_queued == nullptr)
                return detail::updateReturn(err, ErrorCode::InvalidParameter, "frames_queued == nullptr", nullptr);

            return create_internal(err, FramesQueuedAdapter::wrap(frames_queued), config);
        }

    private:
        template<typename TCallbackAdapter>
        static std::shared_ptr<QueueSink> create_internal(Error& err, TCallbackAdapter* listenerAdapter, const Config& config)
        {
            c_interface::IC4_QUEUESINK_CONFIG sink_config =
            {
                TCallbackAdapter::callbacks(),                                                              // .callbacks =
                listenerAdapter,                                                                            // .callback_context =
                reinterpret_cast<const c_interface::IC4_PIXEL_FORMAT*>(config.acceptedPixelFormats.data()), // .pixel_formats =
                config.acceptedPixelFormats.size(),                                                         // .num_pixel_formats =
                {},                                                                                         // .allocator =
                nullptr,                                                                                    // .allocator_context =
                config.maxOutputBuffers,                                                                    // .max_output_buffers =
            };

            if (config.bufferAllocator != nullptr)
            {
                sink_config.allocator = detail::AllocatorAdapter::callbacks();
                sink_config.allocator_context = detail::AllocatorAdapter::wrap(config.bufferAllocator);
            };

            c_interface::IC4_SINK* ptr = nullptr;
            if (ic4_queuesink_create(&ptr, &sink_config))
            {                
                return detail::clearReturn(err, std::shared_ptr<QueueSink>(new QueueSink(ptr)));
            }
            return detail::updateFromLastErrorReturn(err, nullptr);
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
        ImageType	outputImageType(Error& err = Error::Default()) const
        {
            ImageType result = {};

            c_interface::IC4_IMAGE_TYPE image_type;
            if (ic4_queuesink_get_output_image_type( ptr_, &image_type))
            {
                result = ImageType(&image_type);
                return detail::clearReturn(err, result);
            }

            return detail::updateFromLastErrorReturn(err, result);
        }

        SinkType	sinkType() const noexcept final
        {
            return SinkType::QueueSink;
        }

        /**
         * @brief Allocates a number of buffers matching the sink's image type and puts them into the free queue.
         * 
         * @param[in] num_buffers   Number of buffers to allocate
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  @c true on success, otherwise @c false.\n
         *          Check the @a err output parameter for error code and error message.
        */
        bool allocAndQueueBuffers(size_t num_buffers, Error& err = Error::Default())
        {
            return detail::returnUpdateFromLastError(err, ic4::c_interface::ic4_queuesink_alloc_and_queue_buffers(ptr_, num_buffers));
        }

        /**
         * @brief Retrieves a buffer that was filled with image data from the sink's output queue.
         * 
         * @param[out] err  Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A @c std::shared_ptr to the buffer that was successfully dequeued, otherwise @c nullptr.\n
         *          Check the @a err output parameter for error code and error message.
         * 
         * @pre
         * This operation is only valid while the sink is connected to a device in a data stream.
         * 
         * @note
         * The buffers are retrieved in order they were received from the video capture device; the oldest image is
         * returned first.
         * 
         * @note
         * After a successful call, the caller owns the image buffer via the @c std::shared_ptr.
         * The pointer must be reset to put the image buffer into the sink's free queue for later reuse.
         */
        std::shared_ptr<ImageBuffer> popOutputBuffer(Error& err = Error::Default())
        {
            ic4::c_interface::IC4_IMAGE_BUFFER* buffer = NULL;
            bool status = ic4::c_interface::ic4_queuesink_pop_output_buffer(ptr_, &buffer);
            if (!status)
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, detail::buffer_wrap(buffer));
        }

        /**
         * @brief Checks whether the data stream this sink is connected to is in the process of being stopped.
         * 
         * This function can be used to cancel a long-running operation in the @ref QueueSinkListener::framesQueued() callback.
         * 
         * @param[out] err  Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  @c true if cancel is requested, otherwise @c false.\n
         *          In case of an error, the function returns @c false.\n
         *          Check the @a err output parameter for error code and error message.
         */
        bool isCancelRequested(Error& err = Error::Default())
        {
            bool cancel_requested = false;
            if (!c_interface::ic4_queuesink_is_cancel_requested(ptr_, &cancel_requested))
            {
                return detail::returnUpdateFromLastError(err, false);
            }

            return detail::clearReturn(err, cancel_requested);
        }

        /**
         * @brief Contains information about the current queue lengths inside the queue sink.
         */
        struct QueueSizes
        {
            size_t free_queue_length;       ///< Number of image buffers in the free queue
            size_t output_queue_length;     ///< Number of filled image buffers in the output queue
        };

        /**
         * @brief Query information about the number of image buffers in the queue sink's queues.
         * 
         * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return  A structure containing queue size information.
         *          If the function fails, both queues are reported to have size 0.
         *          Check the @a err output parameter for error code and error message.
         */
        QueueSizes queueSizes(Error& err = Error::Default())
        {
            QueueSizes result = {};

            c_interface::IC4_QUEUESINK_QUEUE_SIZES sizes = {};
            if (!c_interface::ic4_queuesink_get_queue_sizes(ptr_, &sizes))
            {
                return detail::updateFromLastErrorReturn(err, result);
            }
            
            result.free_queue_length = sizes.free_queue_length;
            result.output_queue_length = sizes.output_queue_length;

            return detail::clearReturn(err, result);
        }

    private:
        QueueSink(c_interface::IC4_SINK* p) noexcept
            : Sink(p)
        {
        }

    private:
        class ListenerAdapter
        {
        private:
            std::shared_ptr<QueueSinkListener> listener_shared;
            QueueSinkListener& listener;

            ListenerAdapter(QueueSinkListener& li) noexcept
                : listener(li)
            {
            }
            ListenerAdapter(const std::shared_ptr<QueueSinkListener>& li) noexcept
                : listener_shared(li)
                , listener(*li)
            {
                assert( li != nullptr );
            }

            static void release(void* context)
            {
                auto adapter = static_cast<ListenerAdapter*>(context);

                delete adapter;
            }

            static bool sink_connected(ic4::c_interface::IC4_SINK* sink, void* context, const c_interface::IC4_IMAGE_TYPE* image_type, size_t min_buffers)
            {
                auto adapter = static_cast<ListenerAdapter*>(context);
                QueueSink tmp_sink(ic4_sink_ref(sink));

                return adapter->listener.sinkConnected(tmp_sink, ic4::ImageType(image_type), min_buffers);
            }

            static void sink_disconnected(ic4::c_interface::IC4_SINK* sink, void* context)
            {
                auto adapter = static_cast<ListenerAdapter*>(context);
                QueueSink tmp_sink(ic4_sink_ref(sink));

                adapter->listener.sinkDisconnected(tmp_sink);
            }

            static void frames_queued(ic4::c_interface::IC4_SINK* sink, void* context)
            {
                auto adapter = static_cast<ListenerAdapter*>(context);
                QueueSink tmp_sink(ic4_sink_ref(sink));

                adapter->listener.framesQueued(tmp_sink);
            }

        public:
            static c_interface::IC4_QUEUESINK_CALLBACKS callbacks() noexcept
            {
                return
                {
                    ListenerAdapter::release,
                    ListenerAdapter::sink_connected,
                    ListenerAdapter::sink_disconnected,
                    ListenerAdapter::frames_queued
                };
            }

            static ListenerAdapter* wrap(QueueSinkListener& cb)
            {
                return new ListenerAdapter(cb);
            }
            static ListenerAdapter* wrap(const std::shared_ptr<QueueSinkListener>& cb)
            {
                return new ListenerAdapter(cb);
            }

        };

        class FramesQueuedAdapter
        {
        private:
            std::function<void(ic4::QueueSink&)> frames_queued_cb;

            FramesQueuedAdapter(std::function<void(ic4::QueueSink&)> fq) noexcept
                : frames_queued_cb(fq)
            {
                assert(fq != nullptr);
            }

            static void release(void* context)
            {
                auto adapter = static_cast<FramesQueuedAdapter*>(context);

                delete adapter;
            }

            static bool sink_connected(ic4::c_interface::IC4_SINK* sink, void* context, const c_interface::IC4_IMAGE_TYPE* image_type, size_t min_buffers)
            {
                (void)sink, (void)context, (void)image_type, (void)min_buffers;

                return true;
            }

            static void sink_disconnected(ic4::c_interface::IC4_SINK* sink, void* context)
            {
                (void)sink, (void)context;
            }

            static void frames_queued(ic4::c_interface::IC4_SINK* sink, void* context)
            {
                auto adapter = static_cast<FramesQueuedAdapter*>(context);

                QueueSink tmp_sink(ic4_sink_ref(sink));
                adapter->frames_queued_cb(tmp_sink);
            }
        public:
            static c_interface::IC4_QUEUESINK_CALLBACKS callbacks() noexcept
            {
                return
                {
                    FramesQueuedAdapter::release,
                    FramesQueuedAdapter::sink_connected,
                    FramesQueuedAdapter::sink_disconnected,
                    FramesQueuedAdapter::frames_queued
                };
            }

            static FramesQueuedAdapter* wrap(std::function<void(ic4::QueueSink&)> framesQueued)
            {
                return new FramesQueuedAdapter(framesQueued);
            }
        };
    };


}

#endif // IC4_queuesink_H_INC_
