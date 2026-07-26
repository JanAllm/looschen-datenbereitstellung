
#ifndef IC4_C_DEVICEENUM_H_INC_
#define IC4_C_DEVICEENUM_H_INC_

#include "ic4core_export.h"

#include "C_Properties.h"

#include <stdint.h>
#include <stdbool.h>

/** @defgroup devenum Device Enumeration
 * 
 * @brief This section describes how to enumerate and query information from connected video capture devices.
 *
 * Call ic4_devenum_create() to create a device enumerator object (#IC4_DEVICE_ENUM).
 *
 * The enumerator provides access to a list of #IC4_DEVICE_INFO objects representing the video capture devices
 * which are present in the system.
 *
 * #IC4_DEVICE_INFO object allow querying device information such as the device's model name and serial number.
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

	/** \struct IC4_DEVICE_ENUM
	 * Device enumerator type.
	 * 
	 * Call ic4_devenum_create() to create a device enumerator object.
	 * 
	 * This type is reference-counted. Call ic4_devenum_ref() to increase the internal reference count,
	 * or ic4_devenum_unref() to decrease it. If the reference count reaches zero, the object is destroyed.
	 * 
	 * After ic4_devenum_update_device_list() has been called, ic4_devenum_get_device_count() returns the number
	 * of detected devices.
	 * 
	 * Use ic4_devenum_get_devinfo() to get a #IC4_DEVICE_INFO object providing information about one of the
	 * devices.
	 * 
	 * @see ic4_devenum_create
	 */
	struct IC4_DEVICE_ENUM;

	/**
	 * Creates a new device enumerator
	 * 
	 * @param[out] ppEnumerator A pointer to receive a pointer to the new device enumerator.\n
	 *                          When the enumerator is no longer required, release the object reference
	 *                          using ic4_devenum_unref().
	 * 
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 * 
     * @see ic4_devenum_unref
	 */
	IC4CORE_API bool ic4_devenum_create(struct IC4_DEVICE_ENUM** ppEnumerator);

	/**
	 * Increases the device enumerator's internal reference count by one.
	 * 
	 * @param[in] pEnumerator A pointer to a device enumerator
	 * 
	 * @return The pointer passed via \a pEnumerator
	 * 
	 * @remarks If \a pEnumerator is \c NULL, the function returns \c NULL. An error value is not set.
	 * 
	 * @see ic4_devenum_unref
	 */
	IC4CORE_API struct IC4_DEVICE_ENUM* ic4_devenum_ref(struct IC4_DEVICE_ENUM* pEnumerator);

	/**
	 * Decreases the device enumerator's internal reference count by one.
	 * 
	 * If the reference count reaches zero, the object is destroyed.
	 * 
	 * @param[in] pEnumerator A pointer to a device enumerator
	 * 
	 * @remarks If \a pEnumerator is \c NULL, the function does nothing. An error value is not set.
	 * 
	 * @see ic4_devenum_ref
	 */
	IC4CORE_API void ic4_devenum_unref(struct IC4_DEVICE_ENUM* pEnumerator);

	/**
	 * Searches for video capture devices and populates the enumerator's internal device list.
	 * 
	 * @param[in] pEnumerator A pointer to a device enumerator
	 * 
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_devenum_get_device_count
	 * @see ic4_devenum_get_devinfo
	 */
	IC4CORE_API bool ic4_devenum_update_device_list(struct IC4_DEVICE_ENUM* pEnumerator);

	/**
	 * Returns the number of devices discovered by the previous call to ic4_devenum_update_device_list().
	 * 
	 * @param[in] pEnumerator A pointer to a device enumerator
	 * 
	 * @return The number of devices in the enumerator's internal device list\n
	 *         If an error occurs, the function returns 0. ic4_get_last_error() can query error information.
	 * 
	 * @see ic4_devenum_get_devinfo
	 */
	IC4CORE_API int ic4_devenum_get_device_count(const struct IC4_DEVICE_ENUM* pEnumerator);

	/** \struct IC4_DEVICE_INFO
	 * Device information type.
	 * 
	 * Instances of this type are created by calling ic4_devenum_get_devinfo().
	 * 
	 * This type is reference-counted. Call ic4_devinfo_ref() to increase the internal reference count,
	 * or ic4_devinfo_unref() to decrease it. If the reference count reaches zero, the object is destroyed.
	 */
	struct IC4_DEVICE_INFO;

	/**
	 * Returns a #IC4_DEVICE_INFO object describing one of the discovered video capture devices.
	 * 
	 * @param[in] pEnumerator	A pointer to a device enumerator
	 * @param[in] index			List position of the device whose information is to be retrieved
	 * @param[out] ppInfo		A pointer to receive a pointer to the a #IC4_DEVICE_INFO object.\n
     *							When the device information object is no longer required, release the reference
	 *							using ic4_devenum_unref().
	 * 
	 * @return \c true on success, otherwise \c false.
	 * 
	 * @remarks ic4_devenum_update_device_list() has to be called before this function can return anything useful.
	 *          Use ic4_devenum_get_device_count() to determine the maximum valid value for \a index.
	 * 
	 * @see ic4_devinfo_unref
	 */
	IC4CORE_API bool ic4_devenum_get_devinfo(const struct IC4_DEVICE_ENUM* pEnumerator, int index, struct IC4_DEVICE_INFO** ppInfo);


	/**
	 * Searches for interfaces and populates the enumerator's internal interface list.
	 * 
	 * @param[in] pEnumerator A pointer to a device enumerator
	 *
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks Using the interface enumeration is entirely optional; for many use cases searching for devices
	 *	via ic4_devenum_update_device_list() is sufficient.
	 *
	 * @see ic4_devenum_get_interface_count
	 * @see ic4_devenum_get_devitf
	 */
	IC4CORE_API bool ic4_devenum_update_interface_list(struct IC4_DEVICE_ENUM* pEnumerator);

	/**
	 * Returns the number of interfaces discovered by the previous call to ic4_devenum_update_interface_list().
	 *
	 * @param[in] pEnumerator A pointer to a device enumerator
	 *
	 * @return The number of interfaces in the enumerator's internal interface list\n
	 *         If an error occurs, the function returns 0. ic4_get_last_error() can query error information.
	 *
	 * @see ic4_devenum_get_devitf
	 */
	IC4CORE_API int ic4_devenum_get_interface_count(const struct IC4_DEVICE_ENUM* pEnumerator);

	/** \struct IC4_INTERFACE
	 * Device interface type.
	 * 
	 * Interfaces represent physical connections for cameras to the computer, e.g. network adapters or USB controllers.
	 * 
	 * Instances of this type are created by calling ic4_devenum_get_devitf().
	 * 
	 * This type is reference-counted. Call ic4_devitf_ref() to increase the internal reference count,
	 * or ic4_devitf_unref() to decrease it. If the reference count reaches zero, the object is destroyed.	
	 */
	struct IC4_INTERFACE;

	/**
	 * Returns a #IC4_INTERFACE object describing one of the discovered interfaces.
	 *
	 * @param[in] pEnumerator	A pointer to a device enumerator
	 * @param[in] index			List position of the interface to be opened
	 * @param[out] ppInterface	A pointer to receive a pointer to the a #IC4_INTERFACE object.\n
	 *                          When the interface object is no longer required, release the reference
	 *                          using ic4_devitf_unref().
	 *
	 * @return \c true on success, otherwise \c false.
	 *
	 * @remarks ic4_devenum_update_interface_list() has to be called before this function can return anything useful.
	 *          Use ic4_devenum_get_interface_count() to determine the maximum valid value for \a index.
	 *
	 * @see ic4_devinfo_unref
	 */
	IC4CORE_API bool ic4_devenum_get_devitf(const struct IC4_DEVICE_ENUM* pEnumerator, int index, struct IC4_INTERFACE** ppInterface);

	/**
	 * Function pointer for the device-list-changed handler
	 *
	 * @param[in] pDevEnum	Pointer to the device enumerator on which the callback was registered
	 * @param[in] user_ptr	User data that was specified when calling #ic4_devenum_event_add_device_list_changed()
	 */
	typedef void (*ic4_devenum_device_list_change_handler)(struct IC4_DEVICE_ENUM* pDevEnum, void* user_ptr);
	/**
	 * Function pointer for cleanup of the device-list-changed user data
	 *
	 * @param[in] user_ptr	User data that was specified when calling #ic4_devenum_event_add_device_list_changed()
	 */
	typedef void (*ic4_devenum_device_list_change_deleter)(void* user_ptr);

	/**
	 * Registers a function to be called when the list of available video capture devices has (potentially) changed
	 * 
	 * @param[in] pEnumerator	The device enumerator for which the callback is registered
	 * @param[in] handler		The function to be called when the list of available video capture devices has changed
	 * @param[in] user_ptr		User data to be passed in calls to \a handler.
	 * @param[in] deleter		A function to be called when the handler was unregistered and the user_ptr will no longer be required.\n
	 *							The deleter function can be used to release data associated with \a user_ptr.\n
	 *							The \a deleter function will be called when the device-list-changed handler is unregistered,
	 *							or the device enumerator object itself is destroyed.
	 * 
     * @note
	 * To unregister a device-list-changed handler, call #ic4_devenum_event_remove_device_list_changed().\n
	 * It is not guaranteed that every call to @a handler correlates to an actual change in the device list.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_devenum_event_add_device_list_changed(struct IC4_DEVICE_ENUM* pEnumerator, ic4_devenum_device_list_change_handler handler, void* user_ptr, ic4_devenum_device_list_change_deleter deleter);
	/**
	 * Unregisters a device-list-changed handler that was previously registered using #ic4_devenum_event_add_device_list_changed().
	 *
	 * @param[in] pEnumerator	The device enumerator for which the callback is currently registered
	 * @param[in] handler		Pointer to the function to be unregistered
	 * @param[in] user_ptr		User data that the function was previously registered with
	 *
	 * @note
	 * The pair of \a handler and \a user_ptr has to be an exact match to the parameters used in the call to #ic4_devenum_event_add_device_list_changed().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_devenum_event_remove_device_list_changed(struct IC4_DEVICE_ENUM* pEnumerator, ic4_devenum_device_list_change_handler handler, void* user_ptr);

	/**
	 * Increases the device interface's internal reference count by one.
	 *
	 * @param[in] pInterface A pointer to a device interface
	 *
	 * @return The pointer passed via \a pInterface
	 *
	 * @remarks If \a pInterface is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_devitf_unref
	 */
	IC4CORE_API struct IC4_INTERFACE* ic4_devitf_ref(struct IC4_INTERFACE* pInterface);

	/**
	 * Decreases the device interface's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pInterface A pointer to a device interface
	 *
	 * @remarks If \a pInterface is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_devitf_ref
	 */
	IC4CORE_API void ic4_devitf_unref(struct IC4_INTERFACE* pInterface);

	/**
	 * Returns the name of the device interface.
	 * 
	 * @param[in] pInterface A pointer to a device interface
	 * 
	 * @return	A null-terminated string containing the device interface's name.\n
	 *			The memory pointed to by the returned pointer is valid as long as the interface object exists.\n
	 *			If an error occurs, the function returns @c NULL. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API const char* ic4_devitf_get_display_name(const struct IC4_INTERFACE* pInterface);

	/**
	 * @brief Returns the name of the transport layer that provides this interface object.
	 * 
	 * This string can be interpreted as a name for the driver providing access to devices on the interface.
	 * 
	 * @param[in] pInterface	A pointer to a device interface
	 * 
	 * @return	A null-terminated string containing the transport layer name.\n
	 *			The memory pointed to by the returned pointer is valid as long as the interface object exists.\n
	 *			If an error occurs, the function returns @c NULL. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API const char* ic4_devitf_get_tl_name(const struct IC4_INTERFACE* pInterface);

	/**
	 * @brief Returns the version of the transport layer that provides this interface object.
	 *
	 * This string can be interpreted as driver version for the driver providing access devices on the interface.
	 *
	 * @param[in] pInterface	A pointer to a device interface
	 *
	 * @return	A null-terminated string containing the transport layer verision.\n
	 *			The memory pointed to by the returned pointer is valid as long as the interface object exists.\n
	 *			If an error occurs, the function returns @c NULL. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API const char* ic4_devitf_get_tl_version(const struct IC4_INTERFACE* pInterface);

	/**
	 * @brief Contains the possible transport layer types.
	 */
	enum IC4_TL_TYPE
	{
		IC4_TLTYPE_UNKNOWN,		///< Other or unknown transport layer type
		IC4_TLTYPE_GIGEVISION,	///< The transport layer uses the GigE Vision standard
		IC4_TLTYPE_USB3VISION	///< The transport layer uses the USB3 Vision standard
	};

	/**
	 * @brief Returns the type of the transport layer used by this interface.
	 * 
	 * @param[in] pInterface	A pointer to a device interface
	 * 
	 * @return	A #IC4_TL_TYPE value describing the type of the transport layer.
	*/
	IC4CORE_API enum IC4_TL_TYPE ic4_devitf_get_tl_type(const struct IC4_INTERFACE* pInterface);

	/**
	 * Opens the property map for the specified device interface.
	 * 
	 * The property map can be used to query advanced interface information or configure the interface and its attached devices.
	 *
	 * @param[in] pInterface A pointer to a device interface
	 * @param[out] ppMap     A pointer to a pointer to a #IC4_PROPERTY_MAP object.\n
	 *                       When the property map is no longer required, release the reference using ic4_propmap_unref().
	 * 
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_devitf_get_property_map(const struct IC4_INTERFACE* pInterface, struct IC4_PROPERTY_MAP** ppMap);

	/**
	 * Searches for video capture devices and populates the device interfaces's internal device list.
	 *
	 * @param[in] pInterface A pointer to a device interface
	 *
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 *
	 * @see ic4_devitf_get_device_count
	 * @see ic4_devitf_get_devinfo
	 */
	IC4CORE_API bool ic4_devitf_update_device_list(struct IC4_INTERFACE* pInterface);

	/**
	 * Returns the number of devices discovered by the previous call to ic4_devitf_update_device_list().
	 *
	 * @param[in] pInterface A pointer to a device interface
	 *
	 * @return The number of devices in the inferface's internal device list\n
	 *         If an error occurs, the function returns 0. ic4_get_last_error() can query error information.
	 *
	 * @see ic4_devitf_get_devinfo
	 */
	IC4CORE_API int ic4_devitf_get_device_count(const struct IC4_INTERFACE* pInterface);

	/**
	 * Returns a #IC4_DEVICE_INFO object describing one of the discovered video capture devices.
	 *
	 * @param[in] pInterface	A pointer to a device interface
	 * @param[in] index			List position of the device whose information is to be retrieved
	 * @param[out] ppInfo		A pointer to receive a pointer to the a #IC4_DEVICE_INFO object.\n
	 *							When the device information object is no longer required, release the reference
	 *							using ic4_devenum_unref().
	 *
	 * @return \c true on success, otherwise \c false.
	 *
	 * @remarks ic4_devitf_update_device_list() has to be called before this function can return anything useful.
	 *          Use ic4_devitf_get_device_count() to determine the maximum valid value for \a index.
	 *
	 * @see ic4_devinfo_unref
	 */
	IC4CORE_API bool ic4_devitf_get_devinfo(const struct IC4_INTERFACE* pInterface, int index, struct IC4_DEVICE_INFO** ppInfo);

	/**
	 * Checks whether two device interface objects refer to the same interface.
	 *
	 * @param[in] pInterface1 First interface object
	 * @param[in] pInterface2 Second interface object
	 *
	 * @return
	 *    \c true if the device interface objects refer to the same interface, otherwise \c false.\n
	 *    If both pointers are NULL, the function returns \c true.
	 */
	IC4CORE_API bool ic4_devitf_equals(const struct IC4_INTERFACE* pInterface1, const struct IC4_INTERFACE* pInterface2);

	/**
	 * Increases the device information's internal reference count by one.
	 *
	 * @param[in] pInfo A pointer to a device information object
	 *
	 * @return The pointer passed via \a pInfo
	 * 
	 * @remarks If \a pInfo is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_devinfo_unref
	 */
	IC4CORE_API struct IC4_DEVICE_INFO* ic4_devinfo_ref(struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Decreases the device information's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pInfo A pointer to a device information object
	 * 
	 * @remarks If \a pInfo is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_devenum_ref
	 */
	IC4CORE_API void ic4_devinfo_unref(struct IC4_DEVICE_INFO* pInfo);


	/**
	 * Get the model name from a device information object.
	 * 
	 * @param[in] pInfo A pointer to a device information object
	 * 
	 * @return
	 *    A pointer to a null-terminated string containing the device's model name, or \c NULL if an error occured.\n
	 *    Use ic4_get_last_error() to query error information.\n
	 *    The memory pointed to by the return value is valid as long as the device information object exists.
	 */
	IC4CORE_API const char* ic4_devinfo_get_model_name(const struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Get the textual representation of the serial number from a device information object.
	 *
	 * @param[in] pInfo A pointer to a device information object
	 *
	 * @return
	 *    A pointer to a null-terminated string containing the device's serial number, or \c NULL if an error occured.\n
	 *    Use ic4_get_last_error() to query error information.\n
	 *    The memory pointed to by the return value is valid as long as the device information object exists.\n
	 *	  The format of the serial number string is device-specific.
	 * 
	 */
	IC4CORE_API const char* ic4_devinfo_get_serial(const struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Get the device version from a device information object.
	 *
	 * @param[in] pInfo A pointer to a device information object
	 *
	 * @return
	 *    A pointer to a null-terminated string containing the device's version information, or \c NULL if an error occured.\n
	 *    Use ic4_get_last_error() to query error information.\n
	 *    The memory pointed to by the return value is valid as long as the device information object exists.\n
	 *	  The format of the device version is device-specific.
	 */
	IC4CORE_API const char* ic4_devinfo_get_version(const struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Get the device's user-defined identifier from a device information object.
	 * 
	 * @param[in] pInfo A pointer to a device information object
	 * 
	 * @return
	 *    A pointer to a null-terminated string containing the device's user-defined identifier, or \c NULL if an error occured.\n
	 *    Use ic4_get_last_error() to query error information.\n
	 *    The memory pointed to by the return value is valid as long as the device information object exists.
	 * 
	 * @remarks
	 *    If supported by the device, the device's user-defined identifier can be configured through the @c DeviceUserID feature
	 *    in the device's property map.
	 */
	IC4CORE_API const char* ic4_devinfo_get_user_id(const struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Get the device's unique name from a device information object.
	 *
	 * @param[in] pInfo A pointer to a device information object
	 *
	 * @return
	 *    A pointer to a null-terminated string containing the device's unique name, or \c NULL if an error occured.\n
	 *    Use ic4_get_last_error() to query error information.\n
	 *    The memory pointed to by the return value is valid as long as the device information object exists.\n
	 *	  The unique name consists of an identifier for the device driver and the device's serial number,
	 *    allowing devices to be uniquely identified by a single string.
	 */
	IC4CORE_API const char* ic4_devinfo_get_unique_name(const struct IC4_DEVICE_INFO* pInfo);

	/**
	 * Checks whether two device information objects refer to the same video capture device.
	 * 
	 * @param[in] pInfo1 First device info
	 * @param[in] pInfo2 Second device info
	 * 
	 * @return
	 *    \c true if the device information objects refer to the same video capture device, otherwise \c false.\n
	 *    If both pointers are NULL, the function returns \c true.
	 */
	IC4CORE_API bool ic4_devinfo_equals(const struct IC4_DEVICE_INFO* pInfo1, const struct IC4_DEVICE_INFO* pInfo2);

	/**
	 * @brief Returns the interface the device represented by the device information object is attached to.
	 * 
	 * @param[in] pInfo			A device information object
	 * @param[out] ppInterface	A pointer to receive a pointer to the a #IC4_INTERFACE object.\n
	 *							When the interface object is no longer required, release the reference
	 *							using ic4_devitf_unref().
	 * 
	 * @return \c true on success, otherwise \c false. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_devinfo_get_devitf(const struct IC4_DEVICE_INFO* pInfo, struct IC4_INTERFACE** ppInterface);
	

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

#endif //IC4_C_DEVICEENUM_H_INC_