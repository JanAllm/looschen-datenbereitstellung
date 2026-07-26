
#ifndef IC4_CPP_DEVICEENUM_H_INC_
#define IC4_CPP_DEVICEENUM_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_DeviceEnum.h"

#include "Error.h"
#include "Properties.h"
#include "HandleRef.h"
#include "StringUtil.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ic4
{
	class Interface;

	class DeviceInfo;

	namespace detail
	{
		DeviceInfo devinfo_attach(c_interface::IC4_DEVICE_INFO* devinfo) noexcept;
		c_interface::IC4_DEVICE_INFO* devinfo_ptr(const DeviceInfo& grabber) noexcept;
	}

	/**
	 * @brief Contains information about a video capture device.
	 * 
	 * The information provided includes the device's model name, serial number and firmware version.
	 * 
	 * Instances of this class are created by calling @ref DeviceEnum::enumDevices(), @ref Interface::enumDevices(), or
	 * @ref Grabber::deviceInfo().
	 * 
	 * Objects returned from @ref Grabber::deviceInfo() are potentially invalid, for example if no device was opened.
	 * Check the @c err output parameter or @ref DeviceInfo::is_valid().
	 * Default-constructed DeviceInfo objects are also invalid.
	 * 
	 * DeviceInfo objects are copyable and comparable using @c operator== and @c operator!=.
	 */
	class DeviceInfo
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		detail::CopyableHandleRef<c_interface::IC4_DEVICE_INFO, c_interface::ic4_devinfo_ref, c_interface::ic4_devinfo_unref> ptr_;

		friend class Grabber;
		friend class DeviceEnum;
		friend class Interface;
		friend c_interface::IC4_DEVICE_INFO* detail::devinfo_ptr(const DeviceInfo& grabber) noexcept;
		friend DeviceInfo detail::devinfo_attach(c_interface::IC4_DEVICE_INFO* devinfo) noexcept;

		DeviceInfo(c_interface::IC4_DEVICE_INFO* info)
			: ptr_(info)
		{
		}
#endif

	public:
		/**
		 * @brief Creates an invalid object.
		 */
		DeviceInfo() = default;

		/**
		 * @brief	Checks whether this device information is a valid object.
		 *
		 * If #ic4::Grabber::deviceInfo() could not create a valid device information object, and the passed @c err was not set to #ic4::Error::Throw(),
		 * an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this device information object was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the error parameter of the source function for details.
		 *
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Checks whether two device information objects refer to the same video capture device.
		 * 
		 * @param[in] other		A second device information object.
		 * 
		 * @return @c true if the two device information objects refer to the same device, otherwise @c false.
		 */
		bool operator==(const DeviceInfo& other) const
		{
			return ic4_devinfo_equals(ptr_, other.ptr_);
		}
		/**
		 * @brief Checks whether two device information objects refer to different devices.
		 *
		 * @param[in] other		A second device information object.
		 *
		 * @return @c true if the two device information objects refer to different devices, otherwise @c false.
		 */
		bool operator!=(const DeviceInfo& other) const
		{
			return !(*this == other);
		}

	public:
		/**
		 * @brief Gets the model name for this device.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return The device's model name, or an empty string if an error occurred.
		 */
		std::string modelName(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_devinfo_get_model_name(ptr_));
		}
		/**
		 * @brief Gets the unique name for this device.
		 * 
		 * The unique name consists of an identifier for the device driver and the device's serial number,
		 * allowing devices to be uniquely identified by a single string.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The device's unique name, or an empty string if an error occurred.
		 */
		std::string uniqueName(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_devinfo_get_unique_name(ptr_));
		}
		/**
		 * @brief Gets the serial of this device.
		 * 
		 * The serial is a string uniquely identifying the device. Usually this is a number that is also printed on the device's housing.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return The device's serial, or an empty string if an error occurred.
		 */
		std::string serial(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_devinfo_get_serial(ptr_));
		}
		/**
		 * @brief Gets the version of this device.
		 *
		 * The format of the version string is device-specific.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The device's version, or an empty string if an error occurred.
		 */
		std::string version(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_devinfo_get_version(ptr_));
		}
		/**
		 * @brief Gets the user-defined identifier of this device.
		 * 
		 * If supported by the device, the device's user-defined identifier can be configured through
		 * the @c DeviceUserID feature in the device's property map.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return The device's user-defined name, or an empty string if an error occurred.
		*/
		std::string userID(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_devinfo_get_user_id(ptr_));
		}

		/**
		 * @brief Returns the interface this device is attached to.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return An @ref Interface object representing the interface this device is attached to.
		*/
		Interface getInterface(Error& err = Error::Default()) const;
	};

	/**
	 * @brief Contains the possible transport layer types.
	*/
	enum class TransportLayerType
	{
		Unknown = c_interface::IC4_TLTYPE_UNKNOWN,			///< Other or unknown transport layer type
		GigEVision = c_interface::IC4_TLTYPE_GIGEVISION,	///< The transport layer uses the GigE Vision standard
		USB3Vision = c_interface::IC4_TLTYPE_USB3VISION,	///< The transport layer uses the USB3 Vision standard
	};

	/**
	 * @brief Represents a device interface.
	 * 
	 * Interfaces represent physical connections for cameras to the computer, e.g. network adapters or USB controllers.
	 *
	 * Instances of this class are created by calling @ref DeviceEnum::enumInterfaces() or @ref DeviceInfo::getInterface().
	 *
	 * Objects returned from @ref DeviceInfo::getInterface() are potentially invalid, for example if the interface no longer exists.
	 * Check the @c err output parameter or @ref Interface::is_valid().
	 * Default-constructed Interface objects are also invalid.
	 *
	 * Interface objects are copyable and comparable using @c operator== and @c operator!=.
	 */
	class Interface
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		friend class DeviceEnum;
		friend class DeviceInfo;

		detail::CopyableHandleRef<c_interface::IC4_INTERFACE, c_interface::ic4_devitf_ref, c_interface::ic4_devitf_unref> ptr_;

		Interface(c_interface::IC4_INTERFACE* p)
			: ptr_(p)
		{
		}
#endif

	public:
		/**
		 * @brief Creates an invalid object.
		 */
		Interface() = default;

		/**
		 * @brief	Checks whether this interface is a valid object.
		 *
		 * If @ref DeviceInfo::getInterface() could not create a valid interface object, and the passed @c err was not set to #ic4::Error::Throw(),
		 * an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this interface object was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the error parameter of the source function for details.
		 *
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Checks whether two interface objects refer to the same interface.
		 *
		 * @param[in] other		A second interface object.
		 *
		 * @return @c true if the two interface objects refer to the same interface, otherwise @c false.
		 */
		bool operator==(const Interface& other) const
		{
			return c_interface::ic4_devitf_equals(ptr_, other.ptr_);
		}
		/**
		 * @brief Checks whether two interface objects refer to different interfaces.
		 *
		 * @param[in] other		A second interface object.
		 *
		 * @return @c true if the two interface objects refer to different interfaces, otherwise @c false.
		 */
		bool operator!=(const Interface& other) const
		{
			return !(*this == other);
		}

	public:
		/**
		 * @brief Opens the property map for the interface.
		 * 
		 * The property map can be used to query advanced interface information or configure the interface and its attached devices.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A property map. If the function fails, the property map is invalid.\n
		 *			Check the @a err output parameter or @ref PropertyMap::is_valid().
		 */
		PropertyMap			interfacePropertyMap(Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY_MAP* map;
			if (c_interface::ic4_devitf_get_property_map(ptr_, &map))
			{
				return detail::clearReturn(err, detail::propmap_attach(map));
			}
			return detail::updateFromLastErrorReturn(err, detail::propmap_invalid());
		}

		/**
		 * @brief Returns the name of the device interface.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return The interface's name, or an empty string if an error occurred.
		 */
		std::string			interfaceDisplayName(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_devitf_get_display_name(ptr_));
		}

		/**
		 * @brief Returns the name of the transport layer that provides this interface object.
		 * 
		 * This string can be interpreted as a name for the driver providing access to devices on the interface.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The name of this interface's transport layer, or an empty string if an error occurred.
		 */
		std::string			transportLayerName(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_devitf_get_tl_name(ptr_));
		}
		/**
		 * @brief Returns the type of the transport layer used by this interface.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return The transport layer type of this interface.
		 */
		TransportLayerType	transportLayerType(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<TransportLayerType>(c_interface::ic4_devitf_get_tl_type(ptr_)));
		}
		/**
		 * @brief Returns the version of the transport layer that provides this interface object.
		 * 
		 * This string can be interpreted as driver version for the driver providing access devices on the interface.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The version of this interface's transport layer, or an empty string if an error occurred.
		 */
		std::string			transportLayerVersion(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_devitf_get_tl_version(ptr_));
		}

		/**
		 * @brief Get a list of the devices currently attached to this interface.
		 *
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A \c std::vector containing #ic4::DeviceInfo objects providing information about the detected video capture devices.\n
		 *			If an error occurs, the vector is empty.
		 */
		std::vector<DeviceInfo> enumDevices(Error& err = Error::Default()) const
		{
			std::vector<DeviceInfo> result;

			if (!ic4_devitf_update_device_list(ptr_))
			{
				return detail::updateFromLastErrorReturn(err, result);
			}

			int count = ic4_devitf_get_device_count(ptr_);

			for (int i = 0; i < count; ++i)
			{
				c_interface::IC4_DEVICE_INFO* di;
				if (ic4_devitf_get_devinfo(ptr_, i, &di))
				{
					result.push_back(DeviceInfo(di));
				}
			}

			return detail::clearReturn(err, result);
		}
	};

	inline Interface DeviceInfo::getInterface(Error& err) const
	{
		c_interface::IC4_INTERFACE* itf = nullptr;
		if (!ic4_devinfo_get_devitf(ptr_, &itf))
		{
			return detail::updateFromLastErrorReturn(err, Interface());
		}

		return detail::clearReturn(err, Interface(itf));
	}

	/**
	 * @brief Device enumerator.
	 * 
	 * To query the devices attached to or the interfaces of the system, use the static functions @ref DeviceEnum::enumDevices and @ref DeviceEnum::enumInterfaces.
	 * 
	 * Device enumerator instances are only useful to register an event handler listening for changes to the detected device list.
	 */
	class DeviceEnum
	{
		detail::MovableHandleRef<c_interface::IC4_DEVICE_ENUM, c_interface::ic4_devenum_unref> ptr_;

	private:
		DeviceEnum(c_interface::IC4_DEVICE_ENUM* p)
			: ptr_(c_interface::ic4_devenum_ref(p))
		{
		}
	public:
		/**
		 * @brief	Creates a new device enumerator.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @note If the constructor fails, the device enumerator object is in an invalid state. All member function calls will fail.
		 * 
		 * @see DeviceEnum::is_valid
		 */
		DeviceEnum(Error& err = Error::Default())
		{
			if (!ic4_devenum_create(&ptr_))
			{
				detail::returnUpdateFromLastError(err, false);
			}
			else
			{
				detail::clear(err);
			}
		}

	public:
		/**
		 * @brief	Checks whether this device enumerator is a valid object.
		 * 
		 * If there is an error in the constructor, and function was not configured to throw on error,
		 * an invalid object is created. All member function calls will fail.
		 * 
		 * @return	@c true, if this device enumerator was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the constructor's error parameter for details.
		 * 
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Get a list of the devices currently attached to the system.
		 * 
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A \c std::vector containing #ic4::DeviceInfo objects providing information about the detected video capture devices.\n
		 *			If an error occurs, the vector is empty.
		 */
		static std::vector<DeviceInfo> enumDevices(Error& err = Error::Default())
		{
			detail::force_clear(err);

			DeviceEnum de(err);
			if (err.isError())
				return {};

			return de.getVideoCaptureDeviceList(err);
		}
		/**
		 * @brief Get a list of the interfaces present in the system.
		 * 
		 * Interfaces represent physical connections for cameras to the computer, e.g. network adapters or USB controllers.
		 *
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A \c std::vector containing #ic4::Interface objects providing information about the detected interfaces.\n
		 *			If an error occurs, the vector is empty.
		 */
		static std::vector<Interface> enumInterfaces(Error& err = Error::Default())
		{
			detail::force_clear(err);

			DeviceEnum de(err);
			if (err.isError())
				return {};

			return de.getInterfaceList(err);
		}

	private:
		struct DeviceListChangedNotificationFunction
		{
			std::function<void(DeviceEnum&)> func;
		};

		static void handle_devicelistchanged_notification(c_interface::IC4_DEVICE_ENUM* enum_ptr, void* user_ptr)
		{
			auto* fn = static_cast<DeviceListChangedNotificationFunction*>(user_ptr);

			DeviceEnum enumerator(enum_ptr);
			fn->func(enumerator);
		}

		static void delete_devicelistchanged_notification(void* user_ptr)
		{
			auto* fn = static_cast<DeviceListChangedNotificationFunction*>(user_ptr);
			delete fn;
		}

	public:
		/**
		 * @brief Function prototype for device-list-changed event handlers.
		 * 
		 * @param[in] device_enum	The device enumerator on which the event handler was registered.
		 */
		using DeviceListChangedHandler = std::function<void(DeviceEnum& device_enum)>;
		/**
		 * @brief Represents a registered callback.
		 *
		 * When a callback function is registered using @ref DeviceEnum::eventAddDeviceListChanged, a token is returned.
		 *
		 * The token can then be used to remove the callback using @ref DeviceEnum::eventRemoveDeviceListChanged at a later time.
		 */
		using NotificationToken = void*;

		/**
		 * @brief Registers a new device-list-changed event handler.
		 * 
		 * @param[in] cb	Callback function to be called when a new device has been detected, or a device was detached.
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	A token that can be used to unregister the callback using @ref DeviceEnum::eventRemoveDeviceListChanged().\n
		 *			If an error occurrs, the function returns @c nullptr.
		 * 
		 * @see DeviceEnum::eventRemoveDeviceListChanged
		 */
		NotificationToken	eventAddDeviceListChanged(DeviceListChangedHandler cb, Error& err = Error::Default())
		{
			auto* fn = new DeviceListChangedNotificationFunction{ cb };

			if (!c_interface::ic4_devenum_event_add_device_list_changed(ptr_, handle_devicelistchanged_notification, fn, delete_devicelistchanged_notification))
			{
				delete fn;
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, fn);
		}
		/**
		 * @brief Unregisters a device-list-changed event handler.
		 * 
		 * @param[in] token		A token that was returned when registering an event handler using @ref DeviceEnum::eventAddDeviceListChanged().
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @see DeviceEnum::eventAddDeviceListChanged
		 */
		bool				eventRemoveDeviceListChanged(NotificationToken token, Error& err = Error::Default())
		{
			auto* fn = static_cast<DeviceListChangedNotificationFunction*>(token);

			return detail::returnUpdateFromLastError(err, c_interface::ic4_devenum_event_remove_device_list_changed(ptr_, handle_devicelistchanged_notification, fn));
		}

	private:
		std::vector<DeviceInfo> getVideoCaptureDeviceList(Error& err = Error::Default())
		{
			std::vector<DeviceInfo> result;

			if (!ic4_devenum_update_device_list(ptr_))
			{
				return detail::updateFromLastErrorReturn(err, result);
			}

			int count = ic4_devenum_get_device_count(ptr_);

			for (int i = 0; i < count; ++i)
			{
				c_interface::IC4_DEVICE_INFO* di;
				if (ic4_devenum_get_devinfo(ptr_, i, &di))
				{
					result.push_back(DeviceInfo(di));
				}
			}

			return detail::clearReturn(err, result);
		}

		std::vector<Interface> getInterfaceList(Error& err = Error::Default())
		{
			std::vector<Interface> result;

			if (!ic4_devenum_update_interface_list(ptr_))
			{
				return detail::updateFromLastErrorReturn(err, result);
			}

			int count = ic4_devenum_get_interface_count(ptr_);

			for (int i = 0; i < count; ++i)
			{
				c_interface::IC4_INTERFACE* itf;
				if (ic4_devenum_get_devitf(ptr_, i, &itf))
				{
					result.push_back(Interface(itf));
				}
			}

			return detail::clearReturn(err, result);
		}
	};

	namespace detail
	{
		inline c_interface::IC4_DEVICE_INFO* devinfo_ptr(const DeviceInfo& devinfo) noexcept
		{
			return devinfo.ptr_;
		}
		inline DeviceInfo devinfo_attach(c_interface::IC4_DEVICE_INFO* devinfo) noexcept
		{
			return DeviceInfo(devinfo);
		}
	}
}

#endif //IC4_CPP_DEVICEENUM_H_INC_