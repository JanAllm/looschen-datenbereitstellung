

#ifndef IC4_C_snapsink_H_INC_
#define IC4_C_snapsink_H_INC_

#include "ic4core_export.h"

#include "C_Sink.h"
#include "C_ImageType.h"
#include "C_Allocator.h"

/** @addtogroup sink
 * @{
 */

 /** @defgroup snapsink Snap Sink
  *
  * @brief The snap sink is a sink implementation that allows a program to capture single images or sequences of images on demand,
  * while still having a display showing all images.
  *
  * To create a snap sink, call @ref ic4_snapsink_create().
  *
  * To grab a single image out of the stream, call @ref ic4_snapsink_snap_single(). To grab a sequence of images, call @ref ic4_snapsink_snap_sequence().
  *
  * The snap sink manages the buffers used for background image aquisition as well as for the grabbed images.
  * During stream setup, a number of buffers is allocated depending on the configured allocation strategy.
  * Additional buffers can be automatically created on demand, if the allocation strategy allows.
  * Likewise, if there is a surplus of unused image buffers, unused buffers are reclaimed and released automatically.
  * 
  * Image buffers that were returned by one of the \a snap functions are owned by their respective caller until @ref ic4_imagebuffer_unref() is called on them.
  * Once the image buffer objects are released, they are automatically returned to the snap sink for reuse.
  * 
  * Please note that if there are no buffers available in the sink when the device tries to deliver a frame,
  * the frame will be dropped. Use #ic4_grabber_get_stream_stats() to find out whether a buffer underrun occurred.
  * 
  * By default, the sink uses buffers provided by the device driver or the implicitly created transformation filter.
  * It is possible to use program-defined buffers be used by setting allocator callback functions in @ref IC4_SNAPSINK_CONFIG::allocator.
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
     * @brief The buffer allocation strategy defines how many buffers are pre-allocated, when additional buffers are created,
     * and when excess buffers are reclaimed.
     */
    enum IC4_SNAPSINK_ALLOCATION_STRATEGY
    {
        /**
         * @brief Use the default strategy
         * 
         * This strategy pre-allocates an automatically selected number of buffers depending on the requirements of the data stream
         * and the image size.
         * 
         * The custom strategy parameters in @ref IC4_SNAPSINK_CONFIG setting are ignored.
         */
        IC4_SNAPSINK_ALLOCATION_STRATEGY_DEFAULT = 0,

        /**
         * @brief Custom allocation strategy
         * 
         * The @ref IC4_SNAPSINK_CONFIG::num_buffers_alloc_on_connect, @ref IC4_SNAPSINK_CONFIG::num_buffers_allocation_threshold
         * @ref IC4_SNAPSINK_CONFIG::num_buffers_free_threshold and @ref IC4_SNAPSINK_CONFIG::num_buffers_max parameters determine
         * the sink's allocation behavior.
         */
        IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM,
    };

    /**
     * @struct IC4_SNAPSINK_CONFIG
     *
     * @brief Configures the behavior of a snap sink.
     *
     * A pointer to a \c IC4_SNAPSINK_CONFIG is passed to @ref ic4_snapsink_create().
     */
    struct IC4_SNAPSINK_CONFIG
    {
        /**
         * @brief Specifies the sink's buffer allocation strategy.
         * 
         * The buffer allocation strategy defines how many buffers are pre-allocated, when additional buffers are created,
         * and when excess buffers are reclaimed.
         */
        enum IC4_SNAPSINK_ALLOCATION_STRATEGY strategy;

        /**
         * @brief Defines the number of buffers to auto-allocate when the stream is set up.
         * 
         * This value is ignored unless @a strategy is set to @ref IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM.
         */
        size_t num_buffers_alloc_on_connect;
        
        /**
         * @brief Defines the minimum number of required free buffers.
         * 
         * If the number of free buffers falls below this, new buffers are allocated.
         * 
         * This value is ignored unless @a strategy is set to @ref IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM.
         */
        size_t num_buffers_allocation_threshold;

        /**
         * @brief Defines the maximum number of free buffers
         * 
         * If the number of free buffers grows above this, buffers are freed.
         * 
         * If set to @c 0, buffers are not freed automatically.
         * 
         * This value is ignored unless @a strategy is set to @ref IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM.
         * 
         * @note
         * If @c num_buffers_free_threshold is not @c 0, it must be larger than @ref num_buffers_allocation_threshold @c + @c 2.
         */
        size_t num_buffers_free_threshold;

        /**
         * @brief Defines the maximum total number of buffers this sink will allocate.
         * 
         * This includes both free buffers managed by the sink and filled buffers owned by the program.
         * 
         * If set to @c 0, there is no limit to the total number of buffers.
         * 
         * This value is ignored unless @a strategy is set to @ref IC4_SNAPSINK_ALLOCATION_STRATEGY_CUSTOM.
         */
        size_t num_buffers_max;

        /**
         * @brief An array of possible pixel formats that the sink can receive.
         *
         * The image types can be partially specified.
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
         * This parameter is optional, set all callback functions to \c to use the default allocator.
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
    };

    /**
     * @brief Creates a new @ref snapsink.
     * 
     * @param[in] ppSink            Pointer to a sink handle to receive the new snap sink.
     * @param[in] config            Pointer to a structure containing the sink configuration.\n
     *                              This parameter is optional; passing @c NULL will use the default configuration.
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * The image type of the images the sink receives is determined when the data stream to the sink is created
     * in a call to #ic4_grabber_stream_setup() using the following steps:
     *  - If @ref IC4_SNAPSINK_CONFIG::num_pixel_formats is \c 0, the device format is selected.
     *  - If the device's output format matches one of the pixel formats passed in @ref IC4_SNAPSINK_CONFIG::pixel_formats, the first match is selected.
     *  - If there is no direct match, but a conversion between the device's output format format and one of the passed pixel formats exists,
     *      the first format with a conversion is selected.
     *  - If no conversion between the device's output format and any of the values in @ref IC4_SNAPSINK_CONFIG::pixel_formats exists, the stream setup fails.
     */
    IC4CORE_API bool ic4_snapsink_create(struct IC4_SINK** ppSink, const struct IC4_SNAPSINK_CONFIG* config);

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
    IC4CORE_API bool ic4_snapsink_get_output_image_type(const struct IC4_SINK* pSink, struct IC4_IMAGE_TYPE* image_type);

    /**
     * @brief Grabs a single image out of the video stream received from the video capture device.
     * 
     * This function waits until either the next buffer from the video capture device is received, or @c timeout_ms milliseconds have passed.
     * If the timeout expires, the function fails and the error value is set to @ref IC4_ERROR_TIMEOUT.
     * 
     * @param[in] pSink             A snap sink
     * @param[out] ppImageBuffer    A pointer to a frame handle to receive the newly-filled image
     * @param[in] timeout_ms        Time to wait for a new image to arrive
     * 
     * @return \c true on success, otherwise \c false.\n
     *			Use ic4_get_last_error() to query error information.
     * 
     * @pre
     * This operation is only valid while the sink is connected to a device in a data stream.
     * 
     * @note
     * After a successfull call, the handle pointed to by \c ppImageBuffer owns the frame object.\n
     * A call to #ic4_imagebuffer_unref() is required to return the image buffer to the sink for reuse.\n
     */
    IC4CORE_API bool ic4_snapsink_snap_single(const struct IC4_SINK* pSink, struct IC4_IMAGE_BUFFER** ppImageBuffer, int64_t timeout_ms);

    /**
     * @brief Grabs a sequence of images out of the video stream received from the video capture device.
     * 
     * This function waits until @a count images have been grabbed, or @a timeout_ms milliseconds have passed.
     * If the timeout expires, the function returns the number of images grabber and the error value is set to @ref IC4_ERROR_TIMEOUT.
     * 
     * @param[in] pSink         A snap sink
     * @param[out] pImageBufferList   A pointer to an array of frame handles to receive the newly-filled images
     * @param[in] count         Number of images to grab
     * @param[in] timeout_ms    Time to wait for all images to arrive
     * 
     * @return  The number of images grabbed successfully.\n
     *          If an error occurred, the function returns @c 0. Use ic4_get_last_error() to query error information.
     * 
     * @pre
     * This operation is only valid while the sink is connected to a device in a data stream.
     *
     * @note
     * After a successfull call, the handles pointed to by \c pImageBufferList own the frame objects.\n
     * A call to #ic4_imagebuffer_unref() is required to return each image buffer to the sink for reuse.\n
     */
    IC4CORE_API size_t ic4_snapsink_snap_sequence(const struct IC4_SINK* pSink, struct IC4_IMAGE_BUFFER** pImageBufferList, size_t count, int64_t timeout_ms);

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

#endif //IC4_C_snapsink_H_INC_