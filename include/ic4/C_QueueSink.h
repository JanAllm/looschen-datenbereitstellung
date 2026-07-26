

#ifndef IC4_C_queuesink_H_INC_
#define IC4_C_queuesink_H_INC_

#include "ic4core_export.h"

#include "C_Sink.h"
#include "C_ImageType.h"
#include "C_Allocator.h"

/** @addtogroup sink
 * @{
 */

/** @defgroup queuesink Queue Sink
 * 
 * @brief The queue sink is a sink implementation that allows a program to process all images received from a video capture device.
 * 
 * A queue sink manages a number of buffers that are organized in two queues:
 *  - A free queue that buffers are pulled from to fill with data from the device
 *  - An output queue that contains the filled buffers ready to be picked up by the program
 * 
 * To create a queue sink, call #ic4_queuesink_create().
 *
 * Usually, the queue sink is interacted with by registering a set of callback functions in #IC4_QUEUESINK_CALLBACKS.
 * The callback functions are called at different significant points in the lifetime of a queue sink:
 *  - @ref IC4_QUEUESINK_CALLBACKS.sink_connected is called when a data stream is being set up from the device to the sink.
 *      The callback is responsible for making sure there are enough buffers queued for streaming to begin.
 *  - @ref IC4_QUEUESINK_CALLBACKS.frames_queued is called whenever there are images available in the output queue.
 *  - @ref IC4_QUEUESINK_CALLBACKS.sink_disconnected is called when a previously-created data stream is stopped.
 *  - @ref IC4_QUEUESINK_CALLBACKS.release is called when the sink object is being destroyed.
 *      Resources that the sink or other callback functions depends on can be released.
 * 
 * To retrieve the oldest available image from the output queue, call #ic4_queuesink_pop_output_buffer().
 * The caller owns the #IC4_IMAGE_BUFFER that is returned.
 * It is responsible to call #ic4_imagebuffer_unref() at a later time to return the buffer to the sink's free queue.
 * 
 * A program does not necessarily have to requeue all image buffers immediately; it can choose to store references to a number of them
 * in its own data structures. However, please note that if there are no buffers in the free queue when the device tries to deliver a frame,
 * the frame will be dropped. Use #ic4_grabber_get_stream_stats() to find out whether a buffer underrun occurred.
 * 
 * @{
 */

#include <stdbool.h>
#include <stddef.h>

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

	struct IC4_SINK;

    /**
     * Contains function pointers used to specify the behavior of a queue sink.
     */
	struct IC4_QUEUESINK_CALLBACKS
	{
        /**
         * @brief Notifies the user that the sink will not call any additional callback functions.
         * 
         * @anchor queuesink_release
         * 
         * Any resources attached to the \c context parameter can be released.
         * 
         * @param[in] context   The \c context parameter that was passed to #ic4_queuesink_create() when the sink was created
         * 
         * @note
         * The \c release callback function is executed on the thread that destroys the sink using the final call to #ic4_sink_unref().
         */
		void (*release)(void* context);

        /**
         * @brief Called when the data stream to the sink is created
         * 
         * @anchor queuesink_sink_connected
         * 
         * @param[in] sink                  The sink object
         * @param[in] context               The \c context parameter that was passed to #ic4_queuesink_create() when the sink was created
         * @param[in] image_type            The negotiated image type that the sink will receive
         * @param[in] min_buffers_required  The minimum number of buffers required by the device to start a stream
         * 
         * @return  \c true, if the data stream should be created.
         *          If \c false is returned the call to #ic4_grabber_stream_setup() will fail.
         * 
         * @note
         * @a min_buffers_required buffers have to be allocated and queued in order for the sink to begin operating.
         * If the function does call @ref ic4_queuesink_alloc_and_queue_buffers(), the required number of buffers
         * will be created automatically after the function returns.
         * If the function does allocate buffers, but the number is lower than required, @ref ic4_grabber_stream_setup() will fail.
         * 
         * @note
         * The \a queuesink_sink_connected function is executed on the thread that calls @ref ic4_grabber_stream_setup().
         */
		bool (*sink_connected)(struct IC4_SINK* sink, void* context, const struct IC4_IMAGE_TYPE* image_type, size_t min_buffers_required);

        /**
         * Called when the data stream to the sink is stopped
         * 
         * @anchor queuesink_sink_disconnected 
         * 
         * @param[in] sink          The sink object
         * @param[in] context       The \c context parameter that was passed to #ic4_queuesink_create() when the sink was created
         * 
         * @note
         * The \a sink_disconnected function is executed on the thread that calls @ref ic4_grabber_stream_stop().
         * 
         * @warning
         * When the data stream to ths sink is stopped, the @ref ic4_grabber_stream_stop() call wait until this function returns.
         * This can quickly lead to a deadlock, if code in the \a sink_disconnected callback performs an operation that unconditionally requires
         * activity on the thread that called @ref ic4_grabber_stream_stop().
         */
		void (*sink_disconnected)(struct IC4_SINK* sink, void* context);

        /**
         * Called when new images were added to the sink's queue of filled buffers
         * 
         * This callback usually calls @ref ic4_queuesink_pop_output_buffer() to access the image buffers.
         * 
         * @anchor queuesink_frames_queued
         * 
         * @param[in] sink          The sink object
         * @param[in] context       The \c context parameter that was passed to #ic4_queuesink_create() when the sink was created
         * 
         * @note
         * If this callback function performs a lengthy operation, it is recommended to regularly check #ic4_queuesink_is_cancel_requested()
         * to determine whether the data stream is being stopped.
         * 
         * @note
         * The \a frames_queued function is executed on a dedicated thread managed by the sink.
         * 
         * @warning
         * When the data stream to ths sink is stopped, the #ic4_grabber_stream_stop() call wait until this function returns. 
         * This can quickly lead to a deadlock, if code in the \c frames_queued callback performs an operation that unconditionally requires
         * activity on the thread that called \c ic4_grabber_stream_stop.
         */
		void (*frames_queued)(struct IC4_SINK* sink, void* context);
	};

    /**
     * @struct IC4_QUEUESINK_CONFIG
     * 
     * @brief Configures the behavior of a queue sink.
     * 
     * A pointer to a \c IC4_QUEUESINK_CONFIG is passed to @ref ic4_queuesink_create().
     */
    struct IC4_QUEUESINK_CONFIG
    {
        /**
         * @brief A structure containing function pointers to customize the sink's behavior.
         * 
         * Programs usually at least register a callback for @ref IC4_QUEUESINK_CALLBACKS::frames_queued to be able
         * to process new images immediately.
         */
        struct IC4_QUEUESINK_CALLBACKS callbacks;
        /**
         * @brief A user-defined value that is passed to the callbacks
         * 
         * If \c callback_context points to a memory location, and callback functions access that memory,
         * the program has to make sure that the memory is valid until the @ref IC4_QUEUESINK_CALLBACKS::release callback is executed.
         */
        void* callback_context;

        /**
         * @brief An array of possible pixel formats that the sink can receive.
         */
        const enum IC4_PIXEL_FORMAT* pixel_formats;
        /**
         * @brief Length of the @ref pixel_formats array.
         * 
         * If this value is \c 0, the sink will accept any pixel format.
         */
        size_t num_pixel_formats;

        /**
         * @brief A structure containing function pointers to customize the sink's allocator.
         * 
         * This parameter is optional, set all callback functions to \c NULL to use the default allocator.
         *
         * If @ref IC4_ALLOCATOR_CALLBACKS::allocate_buffer is set, @ref IC4_ALLOCATOR_CALLBACKS::free_buffer must be set as well.
         */
        struct IC4_ALLOCATOR_CALLBACKS allocator;
        /**
         * @brief A user-defined value that is passed to the allocator callbacks
         *
         * If \c callback_context points to a memory location, and callback functions access that memory,
         * the program has to make sure that the memory is valid until the @ref IC4_ALLOCATOR_CALLBACKS::release callback is executed.
         */
        void* allocator_context;

        /**
         * @brief Defines the maximum number of buffers that are stored in the sink's output queue.
         * 
         * If set to @c 0, the number of buffers is unlimited.
         * If a new frame arrives at the sink, and the output queue size would exceed @a max_output_buffers,
         * the oldest image is discarded and its buffer is added to the free queue.
         */
        size_t max_output_buffers;
    };

    /**
     * @brief Creates a new @ref queuesink.
     * 
     * @param[in] ppSink            Pointer to a sink handle to receive the new queue sink.
     * @param[in] config            Pointer to a structure containing the sink configuration
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * The image type of the images the sink receives is determined when the data stream to the sink is created
     * in a call to #ic4_grabber_stream_setup() using the following steps:
     *  - If @ref IC4_QUEUESINK_CONFIG::num_pixel_formats is \c 0, the device format is selected.
     *  - If the device's output format matches one of the pixel formats passed in @ref IC4_QUEUESINK_CONFIG::pixel_formats, the first match is selected.
     *  - If there is no direct match, but a conversion between the device's output format format and one of the passed pixel formats exists,
     *      the first format with a conversion is selected.
     *  - If no conversion between the device's output format and any of the values in @ref IC4_QUEUESINK_CONFIG::pixel_formats exists, the stream setup fails.
     */
	IC4CORE_API bool ic4_queuesink_create(struct IC4_SINK** ppSink, const struct IC4_QUEUESINK_CONFIG* config);

    /**
     * @brief Queries the image type of the images the sink is configured to receive.
     * 
     * @param[in] pSink         A queue sink
     * @param[out] image_type   A structure receiving the image type information
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * @pre This operation is only valid while there is a data stream from a device to the sink.
     */
	IC4CORE_API bool ic4_queuesink_get_output_image_type(const struct IC4_SINK* pSink, struct IC4_IMAGE_TYPE* image_type);

    /**
     * @brief Allocates a number of buffers matching the sink's image type and puts them into the free queue.
     * 
     * @param[in] pSink         A queue sink
     * @param[in] num_buffers   Number of buffers to allocate
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * @pre This operation is only valid while the sink's image type is known. This is the case\n
     *          - inside the @ref IC4_QUEUESINK_CALLBACKS.sink_connected callback function
     *          - when the sink was successfully connected to a data stream
     */
    IC4CORE_API bool ic4_queuesink_alloc_and_queue_buffers(struct IC4_SINK* pSink, size_t num_buffers);

    /**
     * @brief Retrieves a buffer that was filled with image data from the sink's output queue.
     * 
     * @param[in] pSink         A queue sink
     * @param[out] ppImageBuffer      A pointer to a frame handle to receive the newly-filled image\n
     * 
     * @return \c true if a buffer was successfully dequeued, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * @pre
     * This operation is only valid while the sink is connected to a device in a data stream.
     * 
     * @note
     * The buffers are retrieved in order they were received from the video capture device; the oldest image is
     * returned first.
     * 
     * @note
     * After a successfull call, the handle pointed to by \c ppImageBuffer owns the frame object.\n
     * A call to #ic4_imagebuffer_unref() is required to put the image buffer into the sink's free queue for later reuse.\n
     */
    IC4CORE_API bool ic4_queuesink_pop_output_buffer(struct IC4_SINK* pSink, struct IC4_IMAGE_BUFFER** ppImageBuffer);

    /**
     * @brief Checks whether the data stream this sink is connected to is in the process of being stopped.
     * 
     * This function can be used to cancel a long-running operation in the @ref IC4_QUEUESINK_CALLBACKS.frames_queued callback.
     * 
     * @param[in] pSink             A queue sink
     * @param[out] cancel_requested Pointer to a flag that receives the cancel request
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     */
    IC4CORE_API bool ic4_queuesink_is_cancel_requested(const struct IC4_SINK* pSink, bool* cancel_requested);

    /**
     * @brief Contains information about the current queue lengths inside the queue sink.
     */
    struct IC4_QUEUESINK_QUEUE_SIZES
    {
        size_t free_queue_length;       ///< Number of image buffers in the free queue
        size_t output_queue_length;     ///< Number of filled image buffers in the output queue
    };

    /**
     * @brief Query information about the number of image buffers in the queue sink's queues.
     * 
     * @param[in] pSink     A queue sink
     * @param[out] sizes    A pointer to a structure to receive the queue lengths
     * 
     * @pre
     * This operation is only valid while there is a data stream from a device to the sink.
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     */
    IC4CORE_API bool ic4_queuesink_get_queue_sizes(const struct IC4_SINK* pSink, struct IC4_QUEUESINK_QUEUE_SIZES* sizes);

#ifdef __cplusplus
#ifdef IC4_C_IN_NAMESPACE
}
}
#endif
}
#endif

/**
 * @}
 *
 * @}
 */

#endif //IC4_C_queuesink_H_INC_