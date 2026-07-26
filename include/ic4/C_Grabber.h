
#ifndef IC4_C_GRABBER_H_INC_
#define IC4_C_GRABBER_H_INC_

#include "ic4core_export.h"

#include "C_Error.h"
#include "C_DeviceEnum.h"

#include <stdbool.h>

/**
 * @defgroup grabber Grabber
 * 
 * @brief Represents an opened video capture device, allowing device configuration and stream setup.
 * 
 * The grabber object is the core component used when working with video capture devices.
 * 
 * To create a grabber object, call #ic4_grabber_create().
 * 
 * After creation, the most common operation on a grabber is to open a device. Call #ic4_grabber_device_open() or one of its siblings.
 * A device can also be selected and configured by calling #ic4_grabber_device_open_from_state() or #ic4_grabber_device_open_from_state_file().
 * 
 * To establish a data stream from the opened video capture device, call #ic4_grabber_stream_setup(), specifying a sink and/or a display.
 * 
 * One of the @ref sink is required if the program needs to access, process, or store image data.
 * There are several sink types available to choose from, which are useful depending on the application, e.g. Queue Sink or Snap Sink.
 *
 * A @ref display can be used to automatically display all images received from a video capture device.
 * 
 * After the data stream has been set up, call #ic4_grabber_acquisition_start() to begin the transfer of images.
 * 
 * When the grabber object is no longer needed, call #ic4_grabber_unref(). Please note that some other object references, e.g. #IC4_IMAGE_BUFFER,
 * can keep the device opened as long as they exist, since they point into device driver memory.
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
	 * @struct IC4_GRABBER
	 * 
	 * @brief Represents an opened video capture device, allowing device configuration and stream setup.
	 * 
	 * The grabber object is the core component used when working with video capture devices.
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_GRABBER*.
	 * 
	 * Grabber objects are reference-counted, and created with an initial reference count of one.
	 * To share a grabber object between multiple parts of a program, create a new reference by calling #ic4_grabber_ref().
	 * When a reference is no longer required, call #ic4_grabber_unref().
	 * 
	 * If the grabber object's internal reference count reaches zero, the grabber object is destroyed.
	 * 
	 * @note
	 * Some object references, e.g. #IC4_IMAGE_BUFFER, can keep the device and/or driver opened as long as they exist,
	 * since they point into device driver memory. To free all device-related resources, all objects references have to be released by calling
	 * their unref-function.
	 */
	struct IC4_GRABBER;

	struct IC4_SINK;
	struct IC4_PROPERTY_MAP;
	struct IC4_DISPLAY;

	/**
	 * @brief Creates a new grabber.
	 *
	 * @param[out] ppGrabber Pointer to receive the handle to the new grabber object.\n
	 *             When the grabber is no longer required, release the object reference using #ic4_grabber_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @see ic4_grabber_unref
	 */
	IC4CORE_API bool ic4_grabber_create(struct IC4_GRABBER** ppGrabber);

	/**
	 * @brief Increases the grabber's internal reference count by one.
	 *
	 * @param[in] pGrabber A pointer to a grabber
	 *
	 * @return The pointer passed via \a pGrabber
	 *
	 * @remarks If \a pGrabber is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_grabber_unref
	 */
	IC4CORE_API struct IC4_GRABBER* ic4_grabber_ref(struct IC4_GRABBER* pGrabber);
	/**
	 * @brief Decreases the grabber's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pGrabber A pointer to a grabber
	 *
	 * @remarks
	 * If \a pGrabber is \c NULL, the function does nothing. An error value is not set.
	 * 
	 * @remarks
	 * If the grabber object is destroyed, all its resources are released:
	 *	- If image acquisition is active, it is stopped.
	 *	- If a data stream was set up, it is stopped.
	 *  - References to data stream-related objects are released, possibly destroying the sink and/or display.
	 *  - The device is closed. @ref properties objects become invalid.
	 *
	 * @see ic4_grabber_ref
	 */
	IC4CORE_API void ic4_grabber_unref(struct IC4_GRABBER* pGrabber);

	/**
	 * @brief Opens the video capture device specified by the passed #IC4_DEVICE_INFO.
	 * 
	 * @param[in] pGrabber	A grabber instance that does not have an opened video capture device
	 * @param[in] dev		A #IC4_DEVICE_INFO representing the video capture device to be opened
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If the grabber already has a device open, the function will fail and the error value is set to @ref IC4_ERROR_INVALID_OPERATION.
	 * 
	 * @see ic4_grabber_open_dev_by_name
	 * @see ic4_grabber_device_close
	 * @see ic4_grabber_is_device_open
	 * @see ic4_grabber_is_device_valid
	 */
	IC4CORE_API bool ic4_grabber_device_open(struct IC4_GRABBER* pGrabber, struct IC4_DEVICE_INFO* dev);

	/**
	 * @brief Opens the video capture matching the specified identifier.
	 * 
	 * @param[in] pGrabber		A grabber instance that does not have an opened video capture device
	 * @param[in] identifier	The model name, unique name, serial, user id, IPV4 address or MAC address of a connected video capture device
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If the grabber already has a device open, the function will fail and the error value is set to #IC4_ERROR_INVALID_OPERATION. \n
	 * If there are multiple devices matching the specified identifier, the function will fail and the error value is set to #IC4_ERROR_AMBIGUOUS. \n
	 * If there is no device with the specified identifier, the function will fail and the error value is set to #IC4_ERROR_DEVICE_NOT_FOUND. \n
	 * 
	 * @see ic4_grabber_device_open
	 * @see ic4_grabber_device_close
	 * @see ic4_grabber_is_device_open
	 * @see ic4_grabber_is_device_valid
	 */
	IC4CORE_API bool ic4_grabber_device_open_by_identifier(struct IC4_GRABBER* pGrabber, const char* identifier);

	/**
	 * @brief Returns information about the currently opened video capture device.
	 * 
	 * @param[in] pGrabber	A grabber instance with an opened video capture device
	 * @param[out] ppDev	A pointer to a handle to receive the device information.\n
	 *						When the device information is no longer required, release the object reference using #ic4_devinfo_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If the grabber does not have an opened video capture device, the function will fail and the error value is set to #IC4_ERROR_INVALID_OPERATION.\n
	 * 
	 * @see ic4_grabber_is_device_open
	 */
	IC4CORE_API bool ic4_grabber_get_device(struct IC4_GRABBER* pGrabber, struct IC4_DEVICE_INFO** ppDev);

	/**
	 * @brief Checks whether the grabber currently has an opened video capture device.
	 * 
	 * @param[in] pGrabber	A grabber object
	 * 
	 * @return \c true, if the grabber has an opened video capture device, otherwise \c false.
	 * 
	 * @remarks
	 * This function neither clears nor sets the error status returned by ic4_get_last_error().
	 * 
	 * @see ic4_grabber_device_open
	 */
	IC4CORE_API bool ic4_grabber_is_device_open(struct IC4_GRABBER* pGrabber);

	/**
	 * @brief Checks whether the grabber's currently opened video capture device is ready to use.
	 * 
	 * @param[in] pGrabber	A grabber object
	 * 
	 * @return \c true, if the grabber has an opened video capture device that is ready to use, otherwise \c false.
	 * 
	 * @remarks
	 * This function neither clears nor sets the error status returned by ic4_get_last_error().\n
	 * There are multiple reasons for why this function may return \c false:
	 *	- No device has been opened
	 *	- The device was disconnected
	 *	- There is a loose hardware connection
	 *	- There was an internal error in the video capture device
	 *	- There was a driver error
	 */
	IC4CORE_API bool ic4_grabber_is_device_valid(struct IC4_GRABBER* pGrabber);

	/**
	 * @brief Closes the video capture device currently opened by this grabber instance
	 * 
	 * @param[in] pGrabber	A grabber with an opened video capture device
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If the device is closed, all its resources are released:
	 *	- If image acquisition is active, it is stopped.
	 *	- If a data stream was set up, it is stopped.
	 *  - References to data stream-related objects are released, possibly destroying the sink and/or display.
	 *  - @ref properties objects become invalid.
	 * 
	 */
	IC4CORE_API bool ic4_grabber_device_close(struct IC4_GRABBER* pGrabber);

	/**
	 * @brief Returns the property map for the currently opened video capture device.
	 * 
	 * The property map returned from this function is the origin for all device feature manipulation operations.
	 *
	 * @param[in] pGrabber			A grabber with an opened video capture device
	 * @param[out] ppPropertyMap	Pointer to a handle that receives the property map.\n
	 *								When the property map is longer required, call #ic4_propmap_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * The property map handle and dependent objects retrieved from it will not keep the device in an opened state.
	 * If the device is closed, all future operations on the property map will result in an error.
	 * 
	 * @see ic4_propmap_unref
	 */
	IC4CORE_API bool ic4_grabber_device_get_property_map(struct IC4_GRABBER* pGrabber, struct IC4_PROPERTY_MAP** ppPropertyMap);

	/**
	 * @brief Returns the property map for the driver of the currently opened video capture device.
	 *
	 * The property map returned from this function is the origin for driver-related feature operations.
	 *
	 * @param[in] pGrabber			A grabber with an opened video capture device
	 * @param[out] ppPropertyMap	Pointer to a handle that receives the property map.\n
	 *								When the property map is longer required, call #ic4_propmap_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * The property map handle and dependent objects retrieved from it will not keep the device in an opened state.
	 * If the device is closed, all future operations on the property map will result in an error.
	 *
	 * @see ic4_propmap_unref
	 */
	IC4CORE_API bool ic4_grabber_driver_get_property_map(struct IC4_GRABBER* pGrabber, struct IC4_PROPERTY_MAP** ppPropertyMap);

	/**
	 * Establishes the data stream from the device.
	 * 
	 * A data stream is required for image acquisition from the video capture device, and must include a sink (@ref sink), a @ref display, or both.
	 * 
	 * @param[in] pGrabber			A grabber object that has opened a video capture device
	 * @param[in] sink				A sink (@ref sink) to receive the images
	 * @param[in] display			A @ref display to display images
	 * @param[in] start_acquisition	If \c true, immediately start image acquisition after the data stream was set up.\n
	 *								Otherwise, a call to #ic4_grabber_acquisition_start() is required to start image acquisition later.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @pre
	 * A device was previously opened using #ic4_grabber_device_open() or one of its sibling functions.
	 * 
	 * @note
	 * The grabber takes references to the passed sink and display, tying their lifetime to the grabber until the data stream is stopped.
	 * 
	 * @see ic4_grabber_stream_stop
	 * @see ic4_grabber_acquisition_start
	 */
	IC4CORE_API bool ic4_grabber_stream_setup(struct IC4_GRABBER* pGrabber, struct IC4_SINK* sink, struct IC4_DISPLAY* display, bool start_acquisition);
	/**
	 * Stops a data stream that was previously set up by a call to #ic4_grabber_stream_setup().
	 * 
	 * @param[in] pGrabber	A grabber with an established data stream
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note
	 * This function releases the sink and display references that were passed to #ic4_grabber_stream_setup().\n
	 * If there are no external references to the sink or display, the sind or display is destroyed.
	 * 
	 * @see ic4_grabber_stream_setup
	 */
	IC4CORE_API bool ic4_grabber_stream_stop(struct IC4_GRABBER* pGrabber);
	/**
	 * Checks whethere there is a data stream established from this grabber's video capture device.
	 * 
	 * @param[in] pGrabber	A grabber object
	 * 
	 * @return	\c true, if a data stream was previously established by calling #ic4_grabber_stream_setup().\n
	 *			Otherwise, or if the data stream was stopped again, \c false.
	 * 
	 * @remarks
	 * This function neither clears nor sets the error status returned by ic4_get_last_error().
	 * 
	 * @see ic4_grabber_stream_setup
	 * @see ic4_grabber_stream_stop
	 */
	IC4CORE_API bool ic4_grabber_is_streaming(struct IC4_GRABBER* pGrabber);

	/**
	 * Starts the acquisition of images from the video capture device.
	 * 
	 * @param[in] pGrabber	A grabber with an established data stream
	 * 
	 * @return	\c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @pre A data stream has was previously established using #ic4_grabber_stream_setup().
	 * 
	 * @note
	 * This operation is equivalent to executing the \c AcquisitionStart command on the video capture device's property map.
	 * 
	 * @see ic4_grabber_acquisition_stop
	 * @see ic4_grabber_stream_setup
	 */
	IC4CORE_API bool ic4_grabber_acquisition_start(struct IC4_GRABBER* pGrabber);
	/**
	 * Stops the acquisition of images from the video capture device.
	 * 
	 * @param[in] pGrabber	A grabber with acquisition active
	 * 
	 * @return	\c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note
	 * This operation is equivalent to executing the \c AcquisitionStop command on the video capture device's property map.
	 * 
	 * @see ic4_grabber_acquisition_start
	 */
	IC4CORE_API bool ic4_grabber_acquisition_stop(struct IC4_GRABBER* pGrabber);
	/**
	 * Checks whether image acquisition is currently enabled for this grabber's video capture device.
	 * 
	 * @param[in] pGrabber	A grabber object
	 * 
	 * @return	\c true, if image acquisition is currently active, otherwise \c false.
	 * 
	 * @remarks
	 * This function neither clears nor sets the error status returned by @ref ic4_get_last_error.
	 * 
	 * @see ic4_grabber_acquisition_start
	 * @see ic4_grabber_acquisition_stop
	 */
	IC4CORE_API bool ic4_grabber_is_acquisition_active(struct IC4_GRABBER* pGrabber);

	/**
	 * Returns a reference to the \ref sink object that was passed to #ic4_grabber_stream_setup()
	 * when setting up the currently established data stream.
	 * 
	 * @param[in] pGrabber	A grabber with an established data stream
	 * @param[out] ppSink	A pointer to a sink handle to receive the currently connected sink.\n
	 *						This is a new reference. If it is no longer in use, it must be released using #ic4_sink_unref().
	 * 
	 * @return	\c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_grabber_stream_setup
	 */
	IC4CORE_API bool ic4_grabber_get_sink(struct IC4_GRABBER* pGrabber, struct IC4_SINK** ppSink);
	/**
	 * Returns a reference to the \ref display object that was passed to #ic4_grabber_stream_setup()
	 * when setting up the currently established data stream.
	 *
	 * @param[in] pGrabber		A grabber with an established data stream
	 * @param[out] ppDisplay	A pointer to a sink handle to receive the currently connected display.\n
	 *							This is a new reference. If it is no longer in use, it must be released using #ic4_display_unref().
	 *
	 * @return	\c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @see ic4_grabber_stream_setup
	 */
	IC4CORE_API bool ic4_grabber_get_display(struct IC4_GRABBER* pGrabber, struct IC4_DISPLAY** ppDisplay);

	/**
	 * Function pointer for the device-lost handler
	 * 
	 * @param[in] pGrabber	Pointer to the grabber whose device was lost
	 * @param[in] user_ptr	User data that was specified when calling #ic4_grabber_event_add_device_lost()
	 */
	typedef void (*ic4_grabber_device_lost_handler)(struct IC4_GRABBER* pGrabber, void* user_ptr);
	/**
	 * Function pointer for cleanup of the device-lost user data
	 * 
	 * @param[in] user_ptr	User data that was specified when calling #ic4_grabber_event_add_device_lost()
	 */
	typedef void (*ic4_grabber_device_lost_deleter)(void* user_ptr);
	/**
	 * Registers a function to be called when the currently opened video capture device was disconnected.
	 * 
	 * @param[in] pGrabber	The grabber for which the callback is registered
	 * @param[in] handler	The function to be called when the device is lost
	 * @param[in] user_ptr	User data to be passed in calls to \a handler.
	 * @param[in] deleter	A function to be called when the handler was unregistered and the user_ptr will no longer be required.\n
	 *						The deleter function can be used to release data associated with \a user_ptr.\n
	 *						The \a deleter function will be called when the device-lost handler is unregistered,
	 *						or the grabber object itself is destroyed.
	 * 
	 * @note
	 * To unregister a device-lost handler, call #ic4_grabber_event_remove_device_lost().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_grabber_event_add_device_lost(struct IC4_GRABBER* pGrabber, ic4_grabber_device_lost_handler handler, void* user_ptr, ic4_grabber_device_lost_deleter deleter);

	/**
	 * Unregisters a device-lost handler that was previously registered using #ic4_grabber_event_add_device_lost().
	 * 
	 * @param[in] pGrabber	The grabber on which the callback is currently registered
	 * @param[in] handler	Pointer to the function to be unregistered
	 * @param[in] user_ptr	User data that the function was previously registered with
	 * 
	 * @note
	 * The pair of \a handler and \a user_ptr has to be an exact match to the parameters used in the call to #ic4_grabber_event_add_device_lost().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_grabber_event_remove_device_lost(struct IC4_GRABBER* pGrabber, ic4_grabber_device_lost_handler handler, void* user_ptr);

	/**
	 * @brief Contains statistics counters that can be used to analyze the stream behavior and identify possible bottlenecks.
	 * 
	 * This structure is filled by calling #ic4_grabber_get_stream_stats().
	 */
	struct IC4_STREAM_STATS
	{
		uint64_t device_delivered;			///< Number of frames delivered by the device
		uint64_t device_transmission_error;	///< Number of frames dropped because of transmission errors, e.g. unrecoverable packet loss
		uint64_t device_underrun;			///< Number of frames dropped by the device driver, because there was no free image buffer available
				
		uint64_t transform_delivered;		///< Number of frames delivered by the transform element
		uint64_t transform_underrun;		///< Number of frames dropped by the transform element, because there was no free image buffer available

		uint64_t sink_delivered;			///< Number of frames processed by the sink
		uint64_t sink_underrun;				///< Number of frames dropped by the sink, because there was no free image buffer available
		uint64_t sink_ignored;				///< Number of frames ignored by the sink, because the sink was disabled or not instructed to process the data
	};
	/**
	 * @brief Query statistics counters from the currently running or previously stopped data stream.
	 * 
	 * @param[in] pGrabber	A grabber object
	 * @param[out] stats	A pointer to a structure to receive the stream statistics
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @pre This operation is only valid after a data stream was established once.
	 */
	IC4CORE_API bool ic4_grabber_get_stream_stats(struct IC4_GRABBER* pGrabber, struct IC4_STREAM_STATS* stats);

	/**
	 * @brief Callback function called to allocate memory during the call of #ic4_grabber_device_save_state.
	 *
	 * @param[in]	size	Size of the memory buffer to be allocated.
	 *
	 * @return		The pointer to the allocated memory buffer, or @c NULL if the allocation was not possible.
	 *
	 * @note
	 * If this function returns @c NULL, the call to #ic4_grabber_device_save_state will fail.
	 *
	 * @see ic4_grabber_device_save_state
	 */
	typedef void* (*ic4_device_state_allocator)(size_t size);

	/**
	 * @brief Saves the currently opened video capture device and all its settings into a memory buffer.
	 * 
	 * @param[in]	pGrabber	A grabber object with an opened device
	 * @param[in]	alloc		Pointer to a function that allocates the memory buffer.\n
	 *							For example, @c malloc can be passed here.
	 * @param[out]	ppData		Pointer to a pointer to receive the newly-allocated memory buffer containing the device state.\n
	 *							The caller is responsible for releasing the memory, using a function that can free memory returned by @c alloc.
	 * @param[out]	data_size	Pointer to size_t to receive the size of the memory buffer allocated by the call
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note
	 * To restore the device state at a later time, use #ic4_grabber_device_open_from_state.\n
	 * In addition to serializing the device's properties (like #ic4_propmap_serialize_to_memory() would), this function also saves the
	 * currently opened video capture device so that it can be re-opened at a later time with all settings restored.
	 * 
	 * @see ic4_grabber_load_device_state
	 * @see ic4_grabber_save_device_state_to_file
	 */
	IC4CORE_API bool ic4_grabber_device_save_state(struct IC4_GRABBER* pGrabber, ic4_device_state_allocator alloc, void** ppData, size_t* data_size);

	/**
	 * @brief Saves the currently opened video capture device and all its settings into a file.
	 * 
	 * @param[in]	pGrabber	A grabber object with an opened device
	 * @param[in]	file_path	Path to a file that the device state is written to.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * To restore the device state at a later time, use #ic4_grabber_device_open_from_state_file().\n
	 * In addition to serializing the device's properties (like #ic4_propmap_serialize_to_file() would), this function also saves the
	 * currently opened video capture device so that it can be re-opened at a later time with all settings restored.
	 *
	 * @see ic4_grabber_device_open_from_state_file
	 * @see ic4_grabber_device_save_state
	 */
	IC4CORE_API bool ic4_grabber_device_save_state_to_file(struct IC4_GRABBER* pGrabber, const char* file_path);
#ifdef _WIN32
	IC4CORE_API bool ic4_grabber_device_save_state_to_fileW(struct IC4_GRABBER* pGrabber, const wchar_t* file_path);
#endif

	/**
	 * @brief Restores the opened device and its settings from a memory buffer containing data that was previously written by #ic4_grabber_device_save_state.
	 *
	 * @param[in]	pGrabber	A grabber object without an opened device
	 * @param[in]	data		Pointer to a memory buffer containing data that was written by #ic4_grabber_device_save_state
	 * @param[in]	data_size	Size of the memory buffer pointed to by @c pData
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * If the memory buffer contains settings for properties that could not be written, the function fails and the error value is set to #IC4_ERROR_INCOMPLETE.
	 *
	 * @see ic4_grabber_device_save_state
	 */
	IC4CORE_API bool ic4_grabber_device_open_from_state(struct IC4_GRABBER* pGrabber, const void* data, size_t data_size);

	/**
	 * @brief Restores the opened device and its settings from a file that was previously written by #ic4_grabber_device_save_state_to_file().
	 *
	 * @param[in]	pGrabber	A grabber object without an opened device
	 * @param[in]	file_path	Path to a file containing device state information
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * If the file contains settings for properties that could not be written, the function fails and the error value is set to #IC4_ERROR_INCOMPLETE.
	 *
	 * @see ic4_grabber_device_save_state_to_file
	 * @see ic4_grabber_device_open_from_state
	 */
	IC4CORE_API bool ic4_grabber_device_open_from_state_file(struct IC4_GRABBER* pGrabber, const char* file_path);
#ifdef _WIN32
	IC4CORE_API bool ic4_grabber_device_open_from_state_fileW(struct IC4_GRABBER* pGrabber, const wchar_t* file_path);
#endif

#ifndef IC4_DOXYGEN_INVISIBLE
	struct IC4_DBG_BUFFER_STATS
	{
		uint64_t num_announced;
		uint64_t num_queued;
		uint64_t num_await_delivery;
	};
	IC4CORE_API bool ic4_dbg_grabber_device_buffer_stats(struct IC4_GRABBER* pGrabber, struct IC4_DBG_BUFFER_STATS* stats);
	IC4CORE_API bool ic4_dbg_grabber_transform_buffer_stats(struct IC4_GRABBER* pGrabber, struct IC4_DBG_BUFFER_STATS* stats);
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

#endif //IC4_C_GRABBER_H_INC_