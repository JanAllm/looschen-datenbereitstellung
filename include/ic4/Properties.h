
#ifndef IC4_PROPERTIES_H_INC_
#define IC4_PROPERTIES_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Properties.h"
#include "C_Display.h"

#include "Error.h"
#include "ImageBuffer.h"
#include "PropId.h"
#include "HandleRef.h"
#include "StringUtil.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <utility>

namespace ic4
{
	/**
	 * @brief Defines the possible property types
	 * 
	 * The property type defines the possible operations on a property and its data type.
	 */
	enum class PropType
	{
		Invalid = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_INVALID,			///< Not a valid property type, indicates an error
		Integer = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_INTEGER,			///< Integer property
		Float = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_FLOAT,				///< Float property
		Enumeration = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_ENUMERATION,	///< Enumeration property
		Boolean = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_BOOLEAN,			///< Boolean property
		String = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_STRING,			///< String property
		Command = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_COMMAND,			///< Command property
		Category = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_CATEGORY,		///< Category property
		Register = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_REGISTER,		///< Register property
		Port = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_PORT,				///< Port property
		EnumEntry = c_interface::IC4_PROPERTY_TYPE::IC4_PROPTYPE_ENUMENTRY,		///< Enumeration entry property
	};

	/**
	 * @brief Defines the possible property visibilities
	 *
	 * Each property has a visibility hint that can be used to create user interfaces for different user types.
	 */
	enum class PropVisibility
	{
		Beginner = c_interface::IC4_PROPERTY_VISIBILITY::IC4_PROPVIS_BEGINNER,	///< Beginner visibility
		Expert = c_interface::IC4_PROPERTY_VISIBILITY::IC4_PROPVIS_EXPERT,		///< Expert visibility
		Guru = c_interface::IC4_PROPERTY_VISIBILITY::IC4_PROPVIS_GURU,			///< Guru visibility
		Invisible = c_interface::IC4_PROPERTY_VISIBILITY::IC4_PROPVIS_INVISIBLE,///< Invisible
	};

	/**
	 * @brief Defines the possible integer property representations
	 *
	 * Each integer property has a representation hint that can help creating more useful user interfaces.
	 */
	enum class PropIntRepresentation
	{
		Linear = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_LINEAR,				///< Suggest a slider to edit the value
		Logarithmic = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_LOGARITHMIC,		///< Suggest a slider with logarithmic mapping
		Boolean = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_BOOLEAN,				///< Suggest a checkbox
		PureNumber = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_PURENUMBER,		///< Suggest displaying a decimal number
		HexNumber = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_HEXNUMBER,			///< Suggest displaying a hexadecimal number
		IPV4Address = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_IPV4ADDRESS,		///< Suggest treating the integer as a IPV4 address
		MACAddress = c_interface::IC4_PROPERTY_INT_REPRESENTATION::IC4_PROPINTREP_MACADDRESS,		///< Suggest treating the integer as a MAC address
	};

	/**
	 * @brief Defines the possible float property representations
	 *
	 * Each float property has a representation hint that can help creating more useful user interfaces.
	 */
	enum class PropFloatRepresentation
	{
		Linear = c_interface::IC4_PROPERTY_FLOAT_REPRESENTATION::IC4_PROPFLOATREP_LINEAR,			///< Suggest a slider to edit the value
		Logarithmic = c_interface::IC4_PROPERTY_FLOAT_REPRESENTATION::IC4_PROPFLOATREP_LOGARITHMIC,	///< Suggest a slider with logarithmic mapping
		PureNumber = c_interface::IC4_PROPERTY_FLOAT_REPRESENTATION::IC4_PROPFLOATREP_PURENUMBER,	///< Suggest displaying a number
	};

	/**
	 * @brief Defines the possible float property display notations
	 *
	 * Each float property has a display notation hint that can help creating more useful user interfaces.
	 */
	enum class PropDisplayNotation
	{
		Automatic = c_interface::IC4_PROPERTY_DISPLAY_NOTATION::IC4_PROPDISPNOTATION_AUTOMATIC,		///< Use an automatic mechanism to determine the best display notation
		Fixed = c_interface::IC4_PROPERTY_DISPLAY_NOTATION::IC4_PROPDISPNOTATION_FIXED,				///< Suggest fixed point notation
		Scientific = c_interface::IC4_PROPERTY_DISPLAY_NOTATION::IC4_PROPDISPNOTATION_SCIENTIFIC,	///< Suggest scientific notation
	};

	/**
	 * @brief Defines the possible property increment modes for Integer and Float properties
	 */
	enum class PropIncrementMode
	{
		/**
		 * @brief The property used a fixed step between valid values.
		 *
		 * Use @ref PropInteger::increment() or @ref PropFloat::increment() to get the property's step size.
		 */
		Increment = c_interface::IC4_PROPERTY_INCREMENT_MODE::IC4_PROPINCMODE_INCREMENT,
		/**
		 * @brief The property defines a set of valid values.
		 *
		 * Use @ref PropInteger::validValueSet() or @ref PropFloat::validValueSet() to query the set of valid values.
		 */
		ValueSet = c_interface::IC4_PROPERTY_INCREMENT_MODE::IC4_PROPINCMODE_VALUESET,
		/**
		 * @brief The property allows setting all values between its minimum and maximum value.
		 *
		 * This mode is only valid for Float properties.
		 *
		 * Integer properties report increment 1 if they allow every possible value between their minimum and maximum value.
		 */
		None = c_interface::IC4_PROPERTY_INCREMENT_MODE::IC4_PROPINCMODE_NONE,
	};

	class PropCommand;
	class PropInteger;
	class PropBoolean;
	class PropFloat;
	class PropString;
	class PropEnumeration;
	class PropEnumEntry;
	class PropRegister;
	class PropCategory;

	namespace detail
	{
		template<typename T>
		T prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		T prop_wrap(c_interface::IC4_PROPERTY* p);

		template<typename T, typename TObj>
		std::vector<T> returnPropertyList(TObj* p, bool get_list(TObj*, c_interface::IC4_PROPERTY_LIST**), Error& err)
		{
			std::vector<T> result;

			c_interface::IC4_PROPERTY_LIST* list = nullptr;
			if (!get_list(p, &list))
				return updateFromLastErrorReturn(err, result);

			size_t size = 0;
			if (!c_interface::ic4_proplist_size(list, &size))
			{
				c_interface::ic4_proplist_unref(list);
				return updateFromLastErrorReturn(err, result);
			}

			for (size_t i = 0; i < size; ++i)
			{
				c_interface::IC4_PROPERTY* entry = nullptr;
				if (c_interface::ic4_proplist_at(list, i, &entry))
				{
					result.push_back(prop_attach<T>(entry));
				}
			}

			c_interface::ic4_proplist_unref(list);
			return clearReturn(err, result);
		}
	}

	/**
	 * @brief Represents a property of a component, usually a video capture device.
	 * 
	 * All property types are derived from Property, e.g. @ref PropInteger, @ref PropCommand or @ref PropEnumeration.
	 * 
	 * PropEnumEntry is also derived from Property, even though they are not part of the category tree.
	 * Nevertheless, enumeration entries support all standard property operations.
	 * 
	 * To convert a @ref Property to one of the specialized classes, use one of the designated conversion methods
	 * such as @ref Property::asInteger() or @ref Property::asFloat().
	 * 
	 * All Property objects are copyable value types, and comparable using @ref operator==, @ref operator!= and @ref operator<.
	 * 
	 * Property objects are created in multiple ways:
	 * - By calling @ref PropertyMap::find() or one of its typed sibling functions like @ref PropertyMap::findInteger().
	 * - By calling @ref PropCategory::features() to get all properties from a category.
	 * - By calling @ref PropEnumeration::entries() to get the enumeration entries of an enumeration property.
	 * - By calling @ref PropertyMap::all() to get all properties in a property map's category tree.
	 * - By calling @ref Property::selectedProperties() to get the properties selected by a property.
	 * - By calling one of the conversion function, e.g. @ref Property::asInteger()
	 * 
	 * The direct get functions and conversion functions can return an invalid property object in case of an error.
	 * Check @ref Property::is_valid() or the function's @a err output parameter.
	 * Default-constructed Property objects are also invalid.
	 * 
	 * Property objects are copyable.
	 */
	class Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		friend class PropEnumeration;

		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

	protected:
#ifndef IC4_DOXYGEN_INVISIBLE
		detail::CopyableHandleRef<c_interface::IC4_PROPERTY, c_interface::ic4_prop_ref, c_interface::ic4_prop_unref> ptr_;

		Property(struct c_interface::IC4_PROPERTY* prop, bool add_ref) noexcept
			: ptr_(add_ref ? ic4_prop_ref(prop) : prop)
		{
		}
#endif

	public:
		/**
		 * @brief Creates an invalid object.
		 */
		Property() = default;
		/**
		 * @brief	Checks whether this property is a valid object.
		 *
		 * If there is an error in the function creating this property object, and function was not configured to throw on error,
		 * an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this property object was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the creating function's error parameter for details.
		 *
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Checks whether a property object refers to the same property as another property object.
		 * 
		 * @param[in] other		A property object
		 * 
		 * @return @c true, if this property refers to the same property as @a other, otherwise @c false.
		 */
		bool operator==(const Property& other) const noexcept	{ return ptr_ == other.ptr_; }
		/**
		 * @brief Checks whether a property object refers to a different property as another property object.
		 *
		 * @param[in] other		A property object
		 *
		 * @return @c true, if this property refers to a different as @a other, otherwise @c false.
		 */
		bool operator!=(const Property & other) const noexcept	{ return !(*this == other); }
		/**
		 * @brief Provides an ordering of property objects.
		 * 
		 * The ordering is arbitrary but stable.
		 * 
		 * @param other			A property object
		 * 
		 * @return A @c bool indicating the ordering relation between @c this and @c other.
		*/
		bool operator<(const Property& other)  const noexcept	{ return ptr_ < other.ptr_; }
	public:
		/**
		 * @brief Returns the type of the property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The type of the property, or @ref PropType::Invalid if an error occurred.\n
		 *			Check the @c err output parameter for details.
		 */
		PropType type(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropType>(c_interface::ic4_prop_get_type(ptr_)));
		}

		/**
		 * @brief Returns the name of the property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The name of the property, or an empty string if an error occurred.\n
		 *			Check the @c err output parameter for details.
		 */
		std::string name(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_prop_get_name(ptr_));
		}

		/**
		 * @brief Checks whether a property is currently available.
		 *
		 * If a property is not available, attempts to read or write its value will fail.
		 * 
		 * A property may become unavailable, if its value does not have a meaning in the current state of the device.
		 * The property's availability status can change upon writing to another property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true, if the property is currently available, otherwise @c false.
		 *			If an error occurred, the function returns @c false.
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see Property::isLocked()
		 * @see Property::isReadOnly()
		 */
		bool isAvailable(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_is_available(ptr_));
		}
		/**
		 * @brief Checks whether a property is currently locked.
		 *
		 * A locked property can be read, but attempts to write its value will fail.
		 *
		 * A property's locked status may change upon writing to another property.
		 *
		 * Common examples for locked properties are @c ExposureTime or @c Gain if @c ExposureAuto or @c GainAuto are enabled.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true, if the property is currently locked, otherwise @c false.\n
		 *			If an error occurred, the function returns @c false.
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see Property::isAvailable()
		 * @see Property::isReadOnly()
		 * @see Property::isLikelyLockedByStream()
		 */
		bool isLocked(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_is_locked(ptr_));
		}
		/**
		 * @brief Tries to determine whether a property is locked because a data stream is active.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true, if the property is currently locked, and will likely be unlocked if the data stream is stopped.\n
		 *			@c false, if the property is not currently locked, or stopping the data stream will probably not lead to
		 *			the property being unlocked.\n
		 *			If there is an error, this function returns \c false. Check the @a err output parameter for error code and error message.
		 * 
		 * @remarks	For technical reasons, this function cannot always accurately predict the future.
		 * 
		 * @see Property::isLocked()
		 */
		bool isLikelyLockedByStream(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_is_likely_locked_by_stream(ptr_));
		}
		/**
		 * @brief Checks whether a property is read-only.
		 * 
		 * A read-only property will never be writable, the read-only status will never change.
		 *
		 * A Common examples for a read-only property is @c DeviceTemperature.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true, if the property is read-only, otherwise @c false.\n
		 *			If an error occurred, the function returns @c false.
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see Property::isAvailable()
		 * @see Property::isLocked()
		 */
		bool isReadOnly(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_is_readonly(ptr_));
		}

		/**
		 * @brief Returns a visibility hint for the property.
		 * 
		 * The visibility hint can be used to create user interfaces with different complexities. The most commonly used properties
		 * have the beginner visibility, while rarely used or diagnostic features might be tagged guru or even invisible.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The visibility hint for the property.\n
		 *			If there is an error, the function returns @ref PropVisibility::Invisible. Check the @c err output parameter for details.
		 */
		PropVisibility visibility(Error& err = Error::Default()) const
		{
			return static_cast<PropVisibility>(detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_get_visibility(ptr_)));
		}

		/**
		 * @brief Returns the display name of the property.
		 * 
		 * A property's display name is a text representation of the property that is meant to be displayed in user interfaces.
		 * For example, the display name of the @c ExposureTime property usually is <i>Exposure Time</i>.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The display name of the property, or an empty string if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		std::string displayName(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_prop_get_display_name(ptr_));
		}
		/**
		 * @brief Returns a tooltip for the property.
		 *
		 * A property's tooltip is a text that can be used when a tooltip is required by a user interface.
		 * Usually, the tooltip is a very short description of the property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A tooltip for the property, or an empty string if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		std::string tooltip(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_prop_get_tooltip(ptr_));
		}
		/**
		 * @brief Returns a description for the property.
		 *
		 * A property's description is a short text that describes the property, usually in more detail than the tooltip.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A description text for the property, or an empty string if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		std::string description(Error& err = Error::Default()) const
		{
			return detail::return_string(err, c_interface::ic4_prop_get_description(ptr_));
		}

		/**
		 * @brief Converts this property into a PropCommand.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	This property converted into a PropCommand. \n
		 *			If this property is not a command property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCommand asCommand(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropInteger.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropInteger. \n
		 *			If this property is not an integer property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropInteger asInteger(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropBoolean.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropBoolean. \n
		 *			If this property is not a boolean property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropBoolean asBoolean(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropFloat.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropFloat. \n
		 *			If this property is not a float property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropFloat asFloat(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropString.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropString. \n
		 *			If this property is not a string property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropString asString(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropEnumeration.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropEnumeration. \n
		 *			If this property is not an enumeration property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumeration asEnumeration(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropEnumEntry.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropEnumEntry. \n
		 *			If this property is not an enumeration entry property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumEntry asEnumEntry(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropRegister.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropRegister. \n
		 *			If this property is not a register property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropRegister asRegister(Error& err = Error::Default()) const;
		/**
		 * @brief Converts this property into a @ref PropCategory.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	This property converted into a @ref PropCategory. \n
		 *			If this property is not a category property, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCategory asCategory(Error& = Error::Default()) const;

	private:
		struct NotificationFunction
		{
			std::function<void(Property&)> func;
		};

		static void handle_notification(c_interface::IC4_PROPERTY* prop_ptr, void* user_ptr)
		{
			auto* fn = static_cast<NotificationFunction*>(user_ptr);

			Property prop(prop_ptr, true);
			fn->func(prop);
		}

		static void delete_notification(void* user_ptr)
		{
			auto* fn = static_cast<NotificationFunction*>(user_ptr);
			delete fn;
		}

	public:
		/**
		 * @brief Function prototype for property notification event handlers.
		 *
		 * @param[in] prop	The property on which the event handler was registered.
		 */
		using NotificationHandler = std::function<void(Property& prop)>;
		/**
		 * @brief Represents a registered callback.
		 *
		 * When a callback function is registered using @ref Property::eventAddNotification, a token is returned.
		 *
		 * The token can then be used to remove the callback using @ref Property::eventRemoveNotification at a later time.
		 */
		using NotificationToken = void*;

		/**
		 * @brief Registers a new property notification event handler.
		 * 
		 * The property notification handler is called whenever an aspect of the property changes, for example its value or locked status.
		 *
		 * @param[in] cb	Callback function to be called when an aspect of the property has changed.
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A token that can be used to unregister the callback using @ref Property::eventRemoveNotification().\n
		 *			If an error occurrs, the function returns @c nullptr.
		 *
		 * @see Property::eventRemoveNotification
		 */
		NotificationToken eventAddNotification(NotificationHandler cb, Error& err = Error::Default())
		{
			auto* fn = new NotificationFunction{ cb };

			if (!c_interface::ic4_prop_event_add_notification(ptr_, handle_notification, fn, delete_notification))
			{
				delete fn;
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, fn);
		}

		/**
		 * @brief Unregisters a previously registered property notification event handler.
		 *
		 * @param[in] token		A token that was returned when registering an event handler using @ref Property::eventAddNotification().
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 *
		 * @see Property::eventAddNotification
		 */
		bool eventRemoveNotification(NotificationToken token, Error& err = Error::Default())
		{
			auto* fn = static_cast<NotificationFunction*>(token);

			return detail::returnUpdateFromLastError(err, c_interface::ic4_prop_event_remove_notification(ptr_, handle_notification, fn));
		}

	public:
		/**
		 * @brief Indicates whether this property's value changes the meaning and/or value of other properties.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true if this property is a selector, otherwise @c false.\n
		 *			If an error occurred, the function returns @c false.
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool isSelector(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, c_interface::ic4_prop_is_selector(ptr_));
		}
		/**
		 * @brief Returns the list of properties whose values' meaning depend on this property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	A @c std::vector containing the selected properties.\n
		 *			If an error occurred, the function returns an empty vector.
		 *			Check the @a err output parameter for error code and error message.
		 */
		std::vector<Property> selectedProperties(Error& err = Error::Default()) const
		{
			return detail::returnPropertyList<Property, c_interface::IC4_PROPERTY>(ptr_, c_interface::ic4_prop_get_selected_props, err);
		}
	};

	/**
	 * @brief Command properties represent an action that can be performed by the device.
	 * 
	 * A common example for a command property is \c TriggerSoftware.
	 * 
	 * PropCommand instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asCommand()
	 * - By directly querying a typed known property using @ref PropertyMap::findCommand().
	 */
	class PropCommand : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);		
#endif

		using Property::Property;
	public:
		/**
		 * @brief Executes the command.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool execute(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, ic4::c_interface::ic4_prop_command_execute(ptr_));
		}
		/**
		 * @brief Checks whether a command has finished executing.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true, if the command is completed. @c false, if the command is still executing.\n
		 *			If an error occurs, the function returns @c true. Check the @a err output parameter for error code and error message.
		 * 
		 * @remarks
		 * If the command was never executed before, the function returns @c true.
		 */
		bool isDone(Error& err = Error::Default()) const
		{
			bool result = true;
			if (!ic4::c_interface::ic4_prop_command_is_done(ptr_, &result))
			{
				return detail::updateFromLastErrorReturn(err, true);
			}
			return detail::clearReturn(err, result);
		}
	};

	/**
	 * @brief Integer properties represent a feature whose value is an integer number.
	 * 
	 * Common examples for a integer properties are @c Width or @c Height.
	 *
	 * An integer property can limit the range of valid values.
	 * The range of possible values can be queried by calling @ref PropInteger::minimum() and @ref PropInteger::maximum().
	 * 
	 * The possible values can be further restricted by an increment value or a set of value values.
	 * Check PropInteger::incrementMode(), PropInteger::increment() and PropInteger::validValueSet() for details.
	 *
	 * In integer property supplies hints that can be useful when creating a user interface:
	 *	- A representation (@ref PropInteger::representation())
	 *	- A unit (@ref PropInteger::unit())
	 * 
	 * PropInteger instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asInteger()
	 * - By directly querying a typed known property using @ref PropertyMap::findInteger().
	 */
	class PropInteger : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Returns the suggested representation for this integer property.
		 * 
		 * The representation can be used as a hint when creating user interfaces.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The suggested representation of this property.\n
		 *			If an error occurred, this function returns a default representation.
		 *			Check the @a err output parameter for error code and error message.
		 */
		PropIntRepresentation representation(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropIntRepresentation>(ic4_prop_integer_get_representation(ptr_)));
		}
		/**
		 * @brief Returns the unit of this integer property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The unit of this property. The unit can be an empty string, if there is no unit for the property.\n
		 *			If an error occurred, this function returns an empty string.
		 *			Check the @a err output parameter for error code and error message.
		 */
		std::string unit(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_prop_integer_get_unit(ptr_));
		}
		/**
		 * @brief Changes the value of this property.
		 * 
		 * @param[in] value		The new value to set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropInteger::getValue()
		*/
		bool setValue(int64_t value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, ic4_prop_integer_set_value(ptr_, value));
		}
		/**
		 * @brief Reads the current value of this property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The current value value, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropInteger::setValue()
		 */
		int64_t getValue(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_integer_get_value);
		}
		/**
		 * @brief Returns the minimum value accepted by this property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The minimum value, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropInteger::maximum()
		 */
		int64_t minimum(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_integer_get_min);
		}
		/**
		 * @brief Returns the maximum value accepted by this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The maximum value, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 *
		 * @see PropInteger::minimum()
		 */
		int64_t maximum(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_integer_get_max);
		}
		/**
		 * @brief Returns how this integer property restricts which values are valid between its minimum and maximum value.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@ref PropIncrementMode::Increment, if the property has a fixed step size between valid values.\n
		 *			@ref PropIncrementMode::ValueSet if the property has a set of valid values.\n
		 *			If an error occurs, the function returns @ref PropIncrementMode::Increment.
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropInteger::increment()
		 * @see PropInteger::validValueSet()
		 */
		PropIncrementMode incrementMode(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropIncrementMode>(ic4_prop_integer_get_inc_mode(ptr_)));
		}
		/**
		 * @brief Returns the step size for valid values accepted by this integer property.
		 * 
		 * The increment restricts the set of valid values for an integer property.
		 * For example, if the property's minimum value is \c 0, the maximum is \c 10, and the increment is \c 2, \c 5 is not a valid value for the property
		 * and will be rejected when trying to write it.
		 * 
		 * This function will fail if @ref PropInteger::incrementMode() doest not return @ref PropIncrementMode::Increment.
		 * 
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The step size for this property, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropInteger::incrementMode()
		 */
		
		int64_t increment(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_integer_get_inc);
		}
		/**
		 * @brief Returns the valid value set for this property.
		 * 
		 * This function will fail if @ref PropInteger::incrementMode() does not return @ref PropIncrementMode::ValueSet.
		 * 
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A vector containing the set of valid values for this property.\n
		 *			If an error occurs, the vector is empty. Check the @a err output parameter for details.
		 * 
		 * @see PropInteger::incrementMode()
		 */
		std::vector<int64_t> validValueSet(Error& err = Error::Default()) const
		{
			std::vector<int64_t> result;

			size_t num_values = 0;
			if (!ic4_prop_integer_get_valid_value_set(ptr_, nullptr, &num_values))
				return detail::updateFromLastErrorReturn(err, result);

			result.resize(num_values);
			if (!ic4_prop_integer_get_valid_value_set(ptr_, result.data(), &num_values))
			{
				result.clear();
				return detail::updateFromLastErrorReturn(err, result);
			}
			
			return detail::clearReturn(err, result);
		}
	};

	/**
	 * @brief Float properties represent a feature whose value is a floating-point number.
	 * 
	 * Common examples for a float properties are @c AcquisitionFrameRate, @c ExposureTime or @c Gain.
	 *
	 * A float property can limit the range of valid values.
	 * The range of possible values can be queried by calling @ref PropFloat::minimum() and @ref PropFloat::maximum().
	 * 
	 * The possible values can be further restricted by an increment value or a set of value values.
	 * Check PropFloat::incrementMode(), PropFloat::increment() and PropFloat::validValueSet() for details.
	 *
	 * A float property supplies hints that can be useful when creating a user interface:
	 *	- A representation (@ref PropFloat::representation())
	 *	- A unit (@ref PropFloat::unit())
	 *	- Display notation and precision (@ref PropFloat::displayNotation(), @ref PropFloat::displayPrecision())
	 *
	 * PropFloat instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asFloat()
	 * - By directly querying a typed known property using @ref PropertyMap::findFloat().
	 */
	class PropFloat : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Returns the suggested representation for this float property.
		 *
		 * The representation can be used as a hint when creating user interfaces.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The suggested representation of this property.\n
		 *			If an error occurred, this function returns a default representation. Check the @a err output parameter for details.
		 */
		PropFloatRepresentation representation(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropFloatRepresentation>(ic4_prop_float_get_representation(ptr_)));
		}
		/**
		 * @brief Returns the unit of this float property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The unit of this property. The unit can be an empty string, if there is no unit for the property.\n
		 *			If an error occurred, this function returns an empty string. Check the @a err output parameter for details.
		 */
		std::string unit(Error& err = Error::Default()) const
		{
			return detail::return_string(err, ic4_prop_float_get_unit(ptr_));
		}
		/**
		 * @brief Returns a suggested display notation to use when displaying the float property's value.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The suggested display notation for this property.\n
		 *			If an error occurred, this function returns a default representation. Check the @a err output parameter for details.
		 */
		PropDisplayNotation displayNotation(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropDisplayNotation>(ic4_prop_float_get_display_notation(ptr_)));
		}
		/**
		 * @brief Returns a suggested number of significant digits to use when displaying the float property's value.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The suggested display notation for this property.\n
		 *			If an error occurred, this function returns a default precision. Check the @a err output parameter for details.
		 */
		int64_t displayPrecision(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, ic4_prop_float_get_display_precision(ptr_));
		}
		/**
		 * @brief Changes the value of this property.
		 *
		 * @param[in] value		The new value to set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropFloat::getValue()
		*/
		bool setValue(double value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, ic4_prop_float_set_value(ptr_, value));
		}
		/**
		 * @brief Reads the current value of this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The current value.\n
		 *			If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropFloat::setValue()
		 */
		double getValue(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<double>(err, ptr_, c_interface::ic4_prop_float_get_value);
		}
		/**
		 * @brief Returns the minimum value accepted by this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The minimum value.\n
		 *			If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropFloat::maximum()
		 */
		double minimum(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<double>(err, ptr_, c_interface::ic4_prop_float_get_min);
		}
		/**
		 * @brief Returns the maximum value accepted by this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The maximum value.\n
		 *			If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropFloat::minimum()
		 */
		double maximum(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<double>(err, ptr_, c_interface::ic4_prop_float_get_max);
		}
		/**
		 * @brief Returns how this float property restricts which values are valid between its minimum and maximum value.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@ref PropIncrementMode::Increment, if the property has a fixed step size between valid values.\n
		 *			@ref PropIncrementMode::ValueSet if the property has a set of valid values.\n
		 *			@ref PropIncrementMode::None if the property has no restrictions.\n
		 *			If an error occurs, the function returns @ref PropIncrementMode::None. Check the @a err output parameter for details.
		 *
		 * @see PropFloat::increment()
		 * @see PropFloat::validValueSet()
		 */
		PropIncrementMode incrementMode(Error& err = Error::Default()) const
		{
			return detail::updateFromLastErrorReturn(err, static_cast<PropIncrementMode>(ic4_prop_float_get_inc_mode(ptr_)));
		}
		/**
		 * @brief Returns the step size for valid values accepted by this float property.
		 *
		 * The increment restricts the set of valid values for a float property.
		 * For example, if the property's minimum value is \c 0, the maximum is \c 10, and the increment is \c 0.5, \c 0.25 is not a valid value for the property
		 * and will be rejected when trying to write it.
		 *
		 * This function will fail if @ref PropFloat::incrementMode() does not return @ref PropIncrementMode::Increment.
		 *
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The step size.\n
		 *			If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropInteger::incrementMode()
		 */
		double increment(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<double>(err, ptr_, c_interface::ic4_prop_float_get_inc);
		}
		/**
		 * @brief Returns the valid value set for this property.
		 *
		 * This function will fail if @ref PropFloat::incrementMode() does not return @ref PropIncrementMode::ValueSet.
		 *
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A vector containing the set of valid values.\n
		 *			If an error occurs, the function returns an empty vector.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropFloat::incrementMode()
		 */
		std::vector<double> validValueSet(Error& err = Error::Default()) const
		{
			std::vector<double> result;

			size_t num_values = 0;
			if (!ic4_prop_float_get_valid_value_set(ptr_, nullptr, &num_values))
				return detail::updateFromLastErrorReturn(err, result);

			result.resize(num_values);
			if (!ic4_prop_float_get_valid_value_set(ptr_, result.data(), &num_values))
			{
				result.clear();
				return detail::updateFromLastErrorReturn(err, result);
			}
			
			return detail::clearReturn(err, result);
		}
	};

	/**
	 * @brief Boolean properties represent a feature whose value is a simple on/off switch.
	 * 
	 * PropBoolean instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asBoolean()
	 * - By directly querying a typed known property using @ref PropertyMap::findBoolean().
	 */
	class PropBoolean : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Changes the value of this property.
		 *
		 * @param[in] value		The new value to set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropBoolean::getValue()
		 */
		bool setValue(bool value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, ic4_prop_boolean_set_value(ptr_, value));
		}
		/**
		 * @brief Reads the current value of this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The current value.\n
		 *			If an error occurs, the function returns @c false.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropBoolean::setValue()
		 */
		bool getValue(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<bool>(err, ptr_, c_interface::ic4_prop_boolean_get_value);
		}
	};

	/**
	 * @brief String properties represent features whose value is a text.
	 * 
	 * The maximum length of the text is indicated by @ref PropString::maxLength().
	 * 
	 * PropString instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asString()
	 * - By directly querying a typed known property using @ref PropertyMap::findString().
	 */
	class PropString : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Changes the value of this property.
		 *
		 * @param[in] value		The new value to set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropString::getValue()
		 */
		bool setValue(const std::string& value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, ic4_prop_string_set_value(ptr_, value.data(), value.length()));
		}
		/**
		 * @brief Reads the current value of this property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The current value.\n
		 *			If an error occurs, the function returns an empty string.
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropString::setValue()
		 */
		std::string getValue(Error& err = Error::Default()) const
		{
			size_t required_size = 128;

			std::string result(required_size, '\0');
			if (!ic4_prop_string_get_value(ptr_, &result[0], &required_size))
			{
				result.resize(required_size);

				if (!ic4_prop_string_get_value(ptr_, &result[0], &required_size))
				{
					return detail::updateFromLastErrorReturn(err, std::string{});
				}
			}
			if (required_size > 0) {
				result.resize(required_size - 1);
			}
			return detail::clearReturn(err, result);
		}
		/**
		 * @brief Returns the maximum length of the string that can be stored in this property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The maximum string length.\n
		 *			If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 */
		uint64_t maxLength(ic4::Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<uint64_t>(err, ptr_, c_interface::ic4_prop_string_get_max_len);
		}
	};

	/**
	 * @brief Represents an entry in a @ref PropEnumeration.
	 * 
	 * Enumeration entries are derived from @ref Property, since they also have
	 * most property aspects like a name, display name, tooltip and visibility flag.
	 * 
	 * In addition to those common attributes, they have a constant numeric value that can be queried using @ref PropEnumEntry::intValue().
	 * 
	 * PropEnumEntry instances are created in multiple ways:
	 * - By querying an enumeration property for its entries using @ref PropEnumeration::entries().
	 * - By calling PropEnumeration::selectedEntry() to get the currently selected entry.
	 * - By calling PropEnumeration::findEntry() to find the entry matching a given value or name.
	 */
	class PropEnumEntry : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Returns the value of the enumeration entry.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The value of the enumeration entry.\n
					If an error occurs, the function returns @c 0.
		 *			Check the @a err output parameter for details.
		 * 
		 * @see PropEnumeration::setValue()
		 */
		int64_t intValue(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_enumentry_get_int_value);
		}
	};

	/**
	 * @brief Enumeration properties represent a feature whose value is selected from a list of named entries.
	 * 
	 * Common examples for an enumeration properties are @c PixelFormat, @c TriggerMode or @c ExposureAuto.
	 *
	 * The value of an enumeration property can be get or set by both a enumeration entry's name or value.
	 *
	 * Enumeration entries are represented by @ref PropEnumEntry objects;
	 * a call to @ref PropEnumeration::entries() returns the list of possible entries for an enumeration.
	 *
	 * PropEnumeration instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asEnumeration()
	 * - By directly querying a typed known property using @ref PropertyMap::findEnumeration().
	 */
	class PropEnumeration : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Returns the list of entries in this enumeration property.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A @c std::vector containing the enumeration entries.\n
		 *			If an error occurred, the function returns an empty vector. Check the @a err output parameter for details.
		 */
		std::vector<PropEnumEntry> entries(Error& err = Error::Default()) const
		{
			return detail::returnPropertyList<PropEnumEntry, c_interface::IC4_PROPERTY>(ptr_, c_interface::ic4_prop_enum_get_entries, err);
		}

		/**
		 * @brief Returns the currently selected entry of this enumeration property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The currently selected enumeration entry.\n
		 *			If an error occurred, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumEntry selectedEntry(Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* entry = nullptr;

			if (!c_interface::ic4_prop_enum_get_selected_entry(ptr_, &entry))
				return detail::updateFromLastErrorReturn(err, PropEnumEntry{});

			return detail::clearReturn(err, detail::prop_attach<PropEnumEntry>(entry));
		}

		/**
		 * @brief Returns the name of the currently selected entry of this enumeration property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	The name of the currently selected enumeration entry.\n
		 *			If an error occurred, an empty string is returned.\n
		 *			Check the the @a err output parameter for details.
		 */
		std::string getValue(Error& err = Error::Default()) const
		{
			auto entry = selectedEntry(err);
			if (!entry.is_valid())
				return {};

			return entry.name(err);
		}

		/**
		 * @brief Finds the enumeration entry with a specified name.
		 *
		 * @param[in] name		The name of the requested enumeration entry
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The enumeration entry whose name is equal to @a name.\n
		 *			If no matching entry is found, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumEntry findEntry(const std::string& name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* entry;

			if (!c_interface::ic4_prop_enum_find_entry_by_name(ptr_, name.c_str(), &entry))
				return detail::updateFromLastErrorReturn(err, PropEnumEntry{});

			return detail::clearReturn(err, detail::prop_attach<PropEnumEntry>(entry));
		}

		/**
		 * @brief Finds the enumeration entry with a specified value.
		 *
		 * @param[in] value		The value of the requested enumeration entry
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The enumeration entry whose value is equal to @a value.\n
		 *			If no matching entry is found, an invalid property object is returned.\n
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumEntry findEntry(int64_t value, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* entry = nullptr;

			if (!c_interface::ic4_prop_enum_find_entry_by_value(ptr_, value, &entry))
				return detail::updateFromLastErrorReturn(err, PropEnumEntry{});

			return detail::clearReturn(err, detail::prop_attach<PropEnumEntry>(entry));
		}

		/**
		 * @brief Selects an enumeration entry by its value.
		 *
		 * @param[in] entry_value	The value of the entry to select
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropEnumeration::intValue()
		 */
		bool setIntValue(int64_t entry_value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_prop_enum_set_int_value(ptr_, entry_value));
		}

#ifndef IC4_DOXYGEN_INVISIBLE
		bool setValue(ic4::PixelFormat fmt, Error& err = Error::Default())
		{
			return setIntValue(static_cast<int64_t>(fmt), err);
		}
#endif
		/**
		 * @brief Reads the value of the currently selected entry of this enumeration property.
		 *
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The value of the currently selected entry, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropEnumeration::setIntValue()
		 */
		int64_t getIntValue(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<int64_t>(err, ptr_, c_interface::ic4_prop_enum_get_int_value);
		}

		/**
		 * @brief Selects an enumeration entry by its name.
		 *
		 * @param[in] entry_name	The name of the entry to to select
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *		 
		 * @see PropEnumeration::getValue()
		 * @see PropEnumeration::setSelectedEntry()
		 * @see PropEnumeration::setIntValue()
		 */
		bool setValue(const std::string& entry_name, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_prop_enum_set_value(ptr_, entry_name.c_str()));
		}

		/**
		 * @brief Selects a specified enumeration entry.
		 *
		 * @param[in] entry		The enumeration entry to to select
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		 *
		 * @see PropEnumeration::selectedEntry()
		 */
		bool selectEntry(const PropEnumEntry& entry, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_prop_enum_set_selected_entry(ptr_, entry.ptr_));
		}
	};

	/**
	 * @brief Register properties have a value represented by raw bytes.
	 * 
	 * PropRegister instances are created in two ways:
	 * - By converting a @ref Property known to be a command property using @ref Property::asRegister()
	 * - By directly querying a typed known property using @ref PropertyMap::findRegister().
	 */
	class PropRegister : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Queries the size of a register property.
		 * 
		 * The size of a register property is not necessarily constant; it can change depending on the value of other properties.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The size of the register property, or @c 0, if an error occurred.\n
		 *			Check the @a err output parameter for details.
		*/
		uint64_t size(Error& err = Error::Default()) const
		{
			return detail::return_prop_attr<uint64_t>(err, ptr_, c_interface::ic4_prop_register_get_size);
		}

		/**
		 * @brief Reads data from a register property.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The current value of the register.\n
		 *			If an error occurs, the returned vector is empty.
		 *			Check the @a err output parameter for details.
		*/
		std::vector<uint8_t> getValue(Error& err = Error::Default())
		{
			std::vector<uint8_t> result;

			uint64_t size = 0;
			if (!c_interface::ic4_prop_register_get_size(ptr_, &size))
			{
				return detail::updateFromLastErrorReturn(err, result);
			}

			result.resize(size);
			if (!c_interface::ic4_prop_register_get_value(ptr_, result.data(), result.size()))
			{
				result.clear();
				return detail::updateFromLastErrorReturn(err, result);
			}

			return detail::clearReturn(err, result);
		}

		/**
		 * @brief Reads data to a register property.
		 *
		 * @param[in] buffer	A @c std::vector containing the data to be written to the property
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for details.
		*/
		bool setValue(const std::vector<uint8_t>& buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_prop_register_set_value(ptr_, buffer.data(), buffer.size()));
		}
	};

	/**
	 * @brief Category properties define a tree-relationship between all properties in a property map.
	 * 
	 * The root of the tree is always the category property with the name \c Root.
	 *
	 * To find which properties are linked from a category, use @ref PropCategory::features().
	 *
	 * Categories can contain other categories recursively. A very simple category tree might look like this:
	 *  - \c Root (category)
	 *		- \c AcquisitionControl (category)
	 *			- \c AcquisitionStart (command)
	 *			- \c AcquisitionStop (command)
	 *			- \c AcquisitionFrameRate (float)
	 *		- \c ImageFormatControl (category)
	 *			- \c Width (integer)
	 *			- \c Height (integer)
	 * 
	 * PropCategory instances are created in two ways:
	 * - By converting a property known to be a category property using @ref Property::asCategory()
	 * - By directly querying a typed known property using @ref PropertyMap::findCategory().
	 */
	class PropCategory : public Property
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		template<typename T>
		friend T detail::prop_attach(c_interface::IC4_PROPERTY* p);
		template<typename T>
		friend T detail::prop_wrap(c_interface::IC4_PROPERTY* p);
#endif

		using Property::Property;
	public:
		/**
		 * @brief Retrieves the list of properties in a property category
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	A @c std::vector containing the properties in this category.\n
		 *			If an error occurred, the function returns an empty vector. Check the @a err output parameter for details.
		 */
		std::vector<Property> features(Error& err = Error::Default()) const
		{
			return detail::returnPropertyList<Property, c_interface::IC4_PROPERTY>(ptr_, c_interface::ic4_prop_category_get_features, err);
		}
	};

	inline PropCommand Property::asCommand(Error& err) const
	{
		if (type() == PropType::Command)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropCommand>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Command", PropCommand{});
		}
	}
	inline PropInteger Property::asInteger(Error& err) const
	{
		if (type() == PropType::Integer)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropInteger>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Integer", PropInteger{});
		}
	}
	inline PropBoolean Property::asBoolean(Error& err) const
	{
		if (type() == PropType::Boolean)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropBoolean>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Boolean", PropBoolean{});
		}
	}
	inline PropFloat Property::asFloat(Error& err) const
	{
		if (type() == PropType::Float)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropFloat>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Float", PropFloat{});
		}
	}
	inline PropString Property::asString(Error& err) const
	{
		if (type() == PropType::String)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropString>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type String", PropString{});
		}
	}
	inline PropEnumeration Property::asEnumeration(Error& err) const
	{
		if (type() == PropType::Enumeration)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropEnumeration>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Enumeration", PropEnumeration{});
		}
	}
	inline PropEnumEntry Property::asEnumEntry(Error& err) const
	{
		if (type() == PropType::EnumEntry)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropEnumEntry>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type EnumEntry", PropEnumEntry{});
		}
	}
	inline PropRegister Property::asRegister(Error& err) const
	{
		if (type() == PropType::Register)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropRegister>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Register", PropRegister{});
		}
	}
	inline PropCategory Property::asCategory(Error& err) const
	{
		if (type() == PropType::Category)
		{
			return detail::clearReturn(err, detail::prop_wrap<PropCategory>(ptr_));
		}
		else
		{
			return detail::updateReturn(err, ErrorCode::GenICamTypeMismatch, "Property is not of type Category", PropCategory{});
		}
	}

	class PropertyMap;

	namespace detail
	{
		c_interface::IC4_PROPERTY_MAP* propmap_ptr(const PropertyMap& map);
		PropertyMap propmap_attach(c_interface::IC4_PROPERTY_MAP* map);
		PropertyMap propmap_wrap(c_interface::IC4_PROPERTY_MAP* map);
		PropertyMap propmap_invalid();
	}

	/**
	 * @brief Represents the property interface of a component, usually a video capture device.
	 * 
	 * A property map offers quick access to known properties as well as functions to enumerate all features through the category tree.
	 * 
	 * There is a plethora of overloaded functions to access properties with a known name and type. For example, to find a known integer property,
	 * use:
	 * - @ref PropertyMap::operator[](const PropId::PropIdInteger&) const
	 * - @ref PropertyMap::findInteger(const std::string&, Error&) const
	 * - @ref PropertyMap::find(const PropId::PropIdInteger&, Error&) const
	 * 
	 * To find a property with a known name, but unknown type, use one of the untyped functions:
	 * - @ref PropertyMap::find(const char*, Error&) const
	 * - @ref PropertyMap::find(const std::string&, Error&) const
	 * - @ref PropertyMap::operator[](const char*) const
	 * - @ref PropertyMap::operator[](const std::string&) const
	 * 
	 * Property values for known properties can also be set directly, for example by calling:
	 * - @ref PropertyMap::setValue(const char*, int64_t, Error&)
	 * - @ref PropertyMap::setValue(const std::string&, double, Error&)
	 * - @ref PropertyMap::setValue(const PropId::PropIdBoolean&, bool, Error&)
	 * 
	 * Additionally, property values for known properties can be queried directly, for example by calling:
	 * - @ref PropertyMap::getValueInt64(const char*, Error&) const
	 * - @ref PropertyMap::getValueDouble(const std::string&, Error&) const
	 * - @ref PropertyMap::getValueString(const PropId::PropIdString&, Error&) const
	 * 
	 * To get a flat list of all properties in the property map's category tree, call @ref PropertyMap::all().
	 * 
	 * The current values of all properties in a property map can be saved to a file or a memory buffer using @ref PropertyMap::serialize().
	 * To restore the settings at a later time, call @ref PropertyMap::deSerialize().
	 * 
	 * An image buffer containing chunk data can be connected to a property map using @ref PropertyMap::connectChunkData(). Doing so lets
	 * the property map uses the image buffer as the data source for chunk property read operations.
	 * 
	 * PropertyMap instances are created by their respective component when queried, for example by calling @ref Grabber::devicePropertyMap()
	 * or @ref VideoWriter::propertyMap().
	 * 
	 * When a function creating a property map fails, or the default constructor is used, an invalid object is created. For
	 * invalid objects, @ref PropertyMap::is_valid() will return false, and all other member functions will fail.
	 * 
	 * PropertyMap objects are copyable.
	 */
	class PropertyMap
	{
		detail::CopyableHandleRef<c_interface::IC4_PROPERTY_MAP, c_interface::ic4_propmap_ref, c_interface::ic4_propmap_unref> ptr_;

#ifndef IC4_DOXYGEN_INVISIBLE
		friend c_interface::IC4_PROPERTY_MAP* detail::propmap_ptr(const PropertyMap& map);
		friend PropertyMap detail::propmap_attach(c_interface::IC4_PROPERTY_MAP* map);
		friend PropertyMap detail::propmap_wrap(c_interface::IC4_PROPERTY_MAP* map);
		friend PropertyMap detail::propmap_invalid();
#endif

		PropertyMap(struct c_interface::IC4_PROPERTY_MAP* map, bool add_ref)
			: ptr_(add_ref ? c_interface::ic4_propmap_ref(map) : map)
		{
		}

	public:
		/**
		 * @brief Creates an invalid property map.
		 */
		PropertyMap() = default;

		/**
		 * @brief	Checks whether this property map is a valid object.
		 *
		 * If there is an error in the function creating this property map and function was not configured to throw on error,
		 * or the default constructor is used, an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this property object was constructed successfully, otherwise @c false.\n
		 *			In case of an error, check the creating function's error parameter for details.
		 *
		 * @see @ref technical_article_error_handling
		 */
		bool is_valid() const noexcept
		{
			return ptr_ != nullptr;
		}

	public:
		/**
		 * @brief Finds the property with a specified name in the property map.
		 * 
		 * @param[in] prop_name	The name of the property to find
		 * 
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 * 
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		Property operator[](const char* prop_name) const
		{
			return find(prop_name, Error::Default());
		}
		/**
		 * @brief Finds the property with a specified name in the property map.
		 *
		 * @param[in] prop_name	The name of the property to find
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		Property operator[](const std::string& prop_name) const
		{
			return find(prop_name, Error::Default());
		}
		/**
		 * @brief Finds the integer property specified by the given property id in the property map.
		 *
		 * @param[in] integer_id	Identifier of the property to find
		 *
		 * @return	The integer property matching @a integer_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropInteger operator[](const PropId::PropIdInteger& integer_id) const
		{
			return find(integer_id, Error::Default());
		}
		/**
		 * @brief Finds the float property specified by the given property id in the property map.
		 *
		 * @param[in] float_id	Identifier of the property to find
		 *
		 * @return	The float property matching @a float_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropFloat operator[](const PropId::PropIdFloat& float_id) const
		{
			return find(float_id, Error::Default());
		}
		/**
		 * @brief Finds the command property specified by the given property id in the property map.
		 *
		 * @param[in] command_id	Identifier of the property to find
		 *
		 * @return	The float property matching @a command_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropCommand operator[](const PropId::PropIdCommand& command_id) const
		{
			return find(command_id, Error::Default());
		}
		/**
		 * @brief Finds the enumeration property specified by the given property id in the property map.
		 *
		 * @param[in] enumeration_id	Identifier of the property to find
		 *
		 * @return	The enumeration property matching @a enumeration_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropEnumeration operator[](const PropId::PropIdEnumeration& enumeration_id) const
		{
			return find(enumeration_id, Error::Default());
		}
		/**
		 * @brief Finds the boolean property specified by the given property id in the property map.
		 *
		 * @param[in] boolean_id	Identifier of the property to find
		 *
		 * @return	The boolean property matching @a boolean_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropBoolean operator[](const PropId::PropIdBoolean& boolean_id) const
		{
			return find(boolean_id, Error::Default());
		}
		/**
		 * @brief Finds the string property specified by the given property id in the property map.
		 *
		 * @param[in] string_id	Identifier of the property to find
		 *
		 * @return	The string property matching @a string_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropString operator[](const PropId::PropIdString& string_id) const
		{
			return find(string_id, Error::Default());
		}
		/**
		 * @brief Finds the register property specified by the given property id in the property map.
		 *
		 * @param[in] register_id	Identifier of the property to find
		 *
		 * @return	The register property matching @a register_id.\n
		 *			If no matching property is found, an invalid property object is returned. Check @ref Property::is_valid().
		 *
		 * @remarks
		 * This operator implicitly uses the default error handler (@ref Error::Default()) and will throw @ref IC4Exception in case of an error,
		 * if the default error handler was configured to do so.
		 */
		PropRegister operator[](const PropId::PropIdRegister& register_id) const
		{
			return find(register_id, Error::Default());
		}

	public:
		/**
		 * @brief Returns a list of all properties reachable from the property map's "Root" category.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A @c std::vector containing all properties in the property map.\n
		 *			If an error occurred, the function returns an empty vector. Check the @a err output parameter for details.
		*/
		std::vector<Property> all(Error& err = Error::Default()) const
		{
			return detail::returnPropertyList<Property, c_interface::IC4_PROPERTY_MAP>(ptr_, c_interface::ic4_propmap_get_all, err);
		}		

	public:
		/**
		 * @brief Executes a command with a known name.
		 * 
		 * @param[in] cmd_name	Name of a command property in this property map
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		*/
		bool executeCommand(const char* cmd_name, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_execute_command(ptr_, cmd_name));
		}
		/**
		 * @brief Executes a command with a known name.
		 *
		 * @param[in] cmd_name	Name of a command property in this property map
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		*/
		bool executeCommand(const std::string& cmd_name, Error& err = Error::Default())
		{
			return executeCommand(cmd_name.c_str(), err);
		}
		/**
		 * @brief Executes a command with a specified identifier.
		 *
		 * @param[in] command_id	Identifier of a command property in this property map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		*/
		bool executeCommand(const PropId::PropIdCommand& command_id, Error& err = Error::Default())
		{
			return executeCommand(command_id.prop_name, err);
		}

		/**
		 * @brief Set the value of a property with a known name to the passed integer value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is set directly.
		 * - For float properties, the value is set directly.
		 * - For boolean properties, if the value is @c 1 or @c 0, it is set to @c true or @c false respectively. Other values result in an error.
		 * - For enumeration properties, the value is set if the property is @c "PixelFormat".
		 * - For command properties, the command is executed if @a value is @c 1.
		 * - For all other property types, the call results in an error.
		 * 
		 * @param[in] integer_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const char* integer_name, int64_t value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_int64(ptr_, integer_name, value));
		}
		/**
		 * @brief Set the value of a property with a known name to the passed integer value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is set directly.
		 * - For float properties, the value is set directly.
		 * - For boolean properties, if the value is @c 1 or @c 0, it is set to @c true or @c false respectively. Other values result in an error.
		 * - For enumeration properties, the value is set if the property is @c "PixelFormat".
		 * - For command properties, the command is executed if @a value is @c 1.
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] integer_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const std::string& integer_name, int64_t value, Error& err = Error::Default())
		{
			return setValue(integer_name.c_str(), value, err);
		}
		/**
		 * @brief Set the value of an integer property with specified identifier to the passed integer value.
		 *
		 * @param[in] integer_id	Identifier of an integer property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdInteger& integer_id, int64_t value, Error& err = Error::Default())
		{
			return setValue(integer_id.prop_name, value, err);
		}

		/**
		 * @brief Set the value of a property with a known name to the passed double value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is rounded to the nearest integer.
		 * - For float properties, the value is set directly.
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] float_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const char* float_name, double value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_double(ptr_, float_name, value));
		}
		/**
		 * @brief Set the value of a property with a known name to the passed double value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is rounded to the nearest integer.
		 * - For float properties, the value is set directly.
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] float_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const std::string& float_name, double value, Error& err = Error::Default())
		{
			return setValue(float_name.c_str(), value, err);
		}
		/**
		 * @brief Set the value of a float property with specified identifier to the passed double value.
		 *
		 * @param[in] float_id	Identifier of a float property in this property map
		 * @param[in] value		New value to be set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdFloat& float_id, double value, Error& err = Error::Default())
		{
			return setValue(float_id.prop_name, value, err);
		}

		/**
		 * @brief Set the value of a property with a known name to the passed bool value.
		 *
		 * The behavior depends on the type of the property:
		 * - For boolean properties, the value is set directly.
		 * - For enumeration properties, it selects the entry with a name that unambiguously suggests to represent @c true or @c false, if available.
		 * - For command properties, the command is executed if @a value is @c true.
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] boolean_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const char* boolean_name, bool value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_bool(ptr_, boolean_name, value));
		}
		/**
		 * @brief Set the value of a property with a known name to the passed bool value.
		 *
		 * The behavior depends on the type of the property:
		 * - For boolean properties, the value is set directly.
		 * - For enumeration properties, it selects the entry with a name that unambiguously suggests to represent @c true or @c false, if available.
		 * - For command properties, the command is executed if @a value is @c true.
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] boolean_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const std::string& boolean_name, bool value, Error& err = Error::Default())
		{
			return setValue(boolean_name.c_str(), value, err);
		}
		/**
		 * @brief Set the value of a boolean property with specified identifier to the passed bool value.
		 *
		 * @param[in] boolean_id	Identifier of a boolean property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdBoolean& boolean_id, bool value, Error& err = Error::Default())
		{
			return setValue(boolean_id.prop_name, value, err);
		}
		/**
		 * @brief Selects an entry in the enumeration property with specified identifier matching the passed bool value.
		 * 
		 * An entry is only selected if there is an entry with a name that unambiguously suggests to represent @c true or @c false.
		 * 
		 * @param enumeration_id	Identifier of an enumeration property in this property map
		 * @param value				New value to be set
		 * @param err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdEnumeration& enumeration_id, bool value, Error& err = Error::Default())
		{
			return setValue(enumeration_id.prop_name, value, err);
		}

		/**
		 * @brief Set the value of a property with a known name to the passed string value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the string is parsed, and the found integer value is set.
		 * - For float properties, the string is parsed, and the found floating-point value is set.
		 * - For boolean properties, a value is set if the string can be unambiguously identified to represent @c true or @c false.
		 * - For enumeration properties, the entry with a name or display name matching the value is selected.
		 * - For string properties, the value is set directly.
		 * - For command properties, the command is executed if @a value is @c "1", @c "true" or @c "execute".
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] string_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const char* string_name, const char* value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_string(ptr_, string_name, value));
		}
		/**
		 * @brief Set the value of a property with a known name to the passed string value.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the string is parsed, and the found integer value is set
		 * - For float properties, the string is parsed, and the found floating-point value is set
		 * - For boolean properties, a value is set if the string can be unambiguously identified to represent @c true or @c false.
		 * - For enumeration properties, the entry with a name or display name matching the value is set.
		 * - For string properties, the value is set directly.
		 * - For command properties, the command is executed if @a value is @c "1", @c "true" or @c "execute".
		 * - For all other property types, the call results in an error.
		 *
		 * @param[in] string_name	Name of a property in this property map
		 * @param[in] value			New value to be set
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const std::string& string_name, const std::string& value, Error& err = Error::Default())
		{
			return setValue(string_name.c_str(), value.c_str(), err);
		}
		/**
		 * @brief Set the value of a string property with a specified identifier to the passed string value.
		 *
		 * @param[in] string_id	Identifier of a string property in this property map
		 * @param[in] value		New value to be set
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdString& string_id, const char* value, Error& err = Error::Default())
		{
			return setValue(string_id.prop_name, value, err);
		}
		/**
		 * @brief Set the value of an enumeration property with a specified identifier to the passed string value.
		 *
		 * The entry with a name matching the passed value is selected.
		 *
		 * @param[in] enumeration_id	Identifier of an enumeration property in this property map
		 * @param[in] value				New value to be set
		 * @param[out] err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdEnumeration& enumeration_id, const char* value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_string(ptr_, enumeration_id.prop_name, value));
		}
		/**
		 * @brief Set the value of a boolean property with a specified identifier to the passed string value.
		 * 
		 * The value is set if the string can be unambiguously identified to represent @c true or @c false.
		 * 
		 * @param boolean_id			Identifier of a boolean property in this property map
		 * @param value					New value to be set
		 * @param err					Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool setValue(const PropId::PropIdBoolean& boolean_id, const char* value, Error& err = Error::Default())
		{
			return setValue(boolean_id.prop_name, value, err);
		}

#ifndef IC4_DOXYGEN_INVISIBLE
		// Don't clutter the documentation with these
		bool setValue(const char* integer_name, int value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_int64(ptr_, integer_name, static_cast<int64_t>(value)));
		}
		bool setValue(const std::string& integer_name, int value, Error& err = Error::Default())
		{
			return setValue(integer_name.c_str(), value, err);
		}
		bool setValue(const PropId::PropIdInteger& integer_id, int value, Error& err = Error::Default())
		{
			return setValue(integer_id.prop_name, value, err);
		}
		bool setValue(const char* integer_name, uint32_t value, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_int64(ptr_, integer_name, static_cast<int64_t>(value)));
		}
		bool setValue(const std::string& integer_name, uint32_t value, Error& err = Error::Default())
		{
			return setValue(integer_name.c_str(), value, err);
		}
		bool setValue(const PropId::PropIdInteger& integer_id, uint32_t value, Error& err = Error::Default())
		{
			return setValue(integer_id.prop_name, value, err);
		}
		bool setValue(const char* enumeration_name, PixelFormat fmt, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_set_value_int64(ptr_, enumeration_name, static_cast<int64_t>(fmt)));
		}
		bool setValue(const std::string& enumeration_name, PixelFormat fmt, Error& err = Error::Default())
		{
			return setValue(enumeration_name.c_str(), fmt, err);
		}
		bool setValue(const PropId::PropIdEnumeration& enumeration_id, PixelFormat fmt, Error& err = Error::Default())
		{
			return setValue(enumeration_id.prop_name, fmt, err);
		}
#endif

		/**
		 * @brief Get the value of a property with a known name interpreted as an integer.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is returned directly.
		 * - For boolean properties, the value returned is @c 1 or @c 0.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		int64_t getValueInt64(const char* prop_name, Error& err = Error::Default()) const
		{
			return detail::return_propmap_typed_value<int64_t>(err, ptr_, prop_name, c_interface::ic4_propmap_get_value_int64);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a double.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is converted to @c double.
		 * - For float properties, the value is returned directly.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		double getValueDouble(const char* prop_name, Error& err = Error::Default()) const
		{
			return detail::return_propmap_typed_value<double>(err, ptr_, prop_name, c_interface::ic4_propmap_get_value_double);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a bool.
		 *
		 * The behavior depends on the type of the property:
		 * - For boolean properties, the value is returned directly.
		 * - For enumeration properties, a value is returned if the name of the currently selected entry unambiguously suggests to represent @c true or @c false.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		bool getValueBool(const char* prop_name, Error& err = Error::Default()) const
		{
			return detail::return_propmap_typed_value<bool>(err, ptr_, prop_name, c_interface::ic4_propmap_get_value_bool);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a string.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is converted to a string
		 * - For float properties, the value is converted to a string
		 * - For boolean properties, the value is converted to the string @c "true" or @c "false".
		 * - For enumeration properties, the name of the currently selected entry is returned.
		 * - For string properties, the value is returned directly.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const char* prop_name, Error& err = Error::Default()) const
		{
			char buffer[128];

			size_t required_size = sizeof(buffer);
			if (ic4_propmap_get_value_string(ptr_, prop_name, buffer, &required_size))
			{
				if (required_size > 0)
					required_size -= 1;

				return detail::clearReturn(err, std::string(buffer, required_size));
			}

			std::string result;
			result.resize(required_size);
			if (ic4_propmap_get_value_string(ptr_, prop_name, &result[0], &required_size))
			{
				if (required_size > 0)
					result.resize(required_size - 1);

				return detail::clearReturn(err, result);
			}

			result.clear();
			return detail::updateFromLastErrorReturn(err, result);
		}

		/**
		 * @brief Get the value of a property with a known name interpreted as an integer.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is returned directly.
		 * - For boolean properties, the value returned is @c 1 or @c 0.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		int64_t getValueInt64(const std::string& prop_name, Error& err = Error::Default()) const
		{
			return getValueInt64(prop_name.c_str(), err);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a double.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is converted to @c double.
		 * - For float properties, the value is returned directly.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		double getValueDouble(const std::string& prop_name, Error& err = Error::Default()) const
		{
			return getValueDouble(prop_name.c_str(), err);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a bool.
		 *
		 * The behavior depends on the type of the property:
		 * - For boolean properties, the value is returned directly.
		 * - For enumeration properties, a value is returned if the name of the currently selected entry unambiguously suggests to represent @c true or @c false.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		bool getValueBool(const std::string& prop_name, Error& err = Error::Default()) const
		{
			return getValueBool(prop_name.c_str(), err);
		}
		/**
		 * @brief Get the value of a property with a known name interpreted as a string.
		 *
		 * The behavior depends on the type of the property:
		 * - For integer properties, the value is converted to a string
		 * - For float properties, the value is converted to a string
		 * - For boolean properties, the value is converted to the string @c "true" or @c "false".
		 * - For enumeration properties, the name of the currently selected entry is returned.
		 * - For string properties, the value is returned directly.
		 * - For all other property types, the call results in an error (@ref ErrorCode::GenICamTypeMismatch).
		 *
		 * @param[in] prop_name		Name of a property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const std::string& prop_name, Error& err = Error::Default()) const
		{
			return getValueString(prop_name.c_str(), err);
		}

		/**
		 * @brief Get the value of an integer property with a known name.
		 *
		 * @param[in] integer_id	Id of an integer property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		int64_t getValueInt64(const PropId::PropIdInteger& integer_id, Error& err = Error::Default()) const
		{
			return getValueInt64(integer_id.prop_name, err);
		}
		/**
		 * @brief Get the value of a boolean property with a known name interpreted as an integer.
		 * 
		 * The value returned is @c 1 or @c 0.
		 *
		 * @param[in] boolean_id	Id of a boolean property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		int64_t getValueInt64(const PropId::PropIdBoolean& boolean_id, Error& err = Error::Default()) const
		{
			return getValueInt64(boolean_id.prop_name, err);
		}
		/**
		 * @brief Get the value of a float property with a known name.
		 *
		 * @param[in] float_id		Id of a float property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		double getValueDouble(const PropId::PropIdFloat& float_id, Error& err = Error::Default()) const
		{
			return getValueDouble(float_id.prop_name, err);
		}
		/**
		 * @brief Get the value of an integer property with a known name interpreted as a double.
		 *
		 * @param[in] integer_id	Id of an integer property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		double getValueDouble(const PropId::PropIdInteger& integer_id, Error& err = Error::Default()) const
		{
			return getValueDouble(integer_id.prop_name, err);
		}
		/**
		 * @brief Get the value of a boolean property with a known name.
		 *
		 * @param[in] boolean_id	Id of a boolean property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		bool getValueBool(const PropId::PropIdBoolean& boolean_id, Error& err = Error::Default()) const
		{
			return getValueBool(boolean_id.prop_name, err);
		}
		/**
		 * @brief Get the value of an enumeration property with a known name interpreted as bool.
		 * 
		 * A boolean value is returned if the name of the currently selected entry unambiguously suggests to represent @c true or @c false.
		 *
		 * @param[in] enumeration_id	Id of an enumeration property inside @c map
		 * @param[out] err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		bool getValueBool(const PropId::PropIdEnumeration& enumeration_id, Error& err = Error::Default()) const
		{
			return getValueBool(enumeration_id.prop_name, err);
		}
		/**
		 * @brief Get the value of a string property with a known name.
		 *
		 * @param[in] string_id		Id of a string property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const PropId::PropIdString& string_id, Error& err = Error::Default()) const
		{
			return getValueString(string_id.prop_name, err);
		}
		/**
		 * @brief Get the value of an enumeration property with a known name.
		 *
		 * @param[in] enumeration_id	Id of an enumeration property inside @c map
		 * @param[out] err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const PropId::PropIdEnumeration& enumeration_id, Error& err = Error::Default()) const
		{
			return getValueString(enumeration_id.prop_name, err);
		}
		/**
		 * @brief Get a string representation of the value of a boolean property with a known name.
		 * 
		 * The returned string is either @c "true" or @c "false".
		 *
		 * @param[in] boolean_id	Id of a boolean property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const PropId::PropIdBoolean& boolean_id, Error& err = Error::Default()) const
		{
			return getValueString(boolean_id.prop_name, err);
		}
		/**
		 * @brief Get a string representation of the value of an integer property with a known name.
		 *
		 * @param[in] integer_id	Id of an integer property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const PropId::PropIdInteger& integer_id, Error& err = Error::Default()) const
		{
			return getValueString(integer_id.prop_name, err);
		}
		/**
		 * @brief Get a string representation of the value of a float property with a known name.
		 *
		 * @param[in] float_id		Id of a float property inside @c map
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	If successful, the value of the property is returned. The return value is undefined in case of an error.\n
		 *			Always check use @a err output parameter or enable exceptions to detect possible errors.
		 */
		std::string getValueString(const PropId::PropIdFloat& float_id, Error& err = Error::Default()) const
		{
			return getValueString(float_id.prop_name, err);
		}

#ifndef IC4_DOXYGEN_INVISIBLE
		PixelFormat getValueInt64(const PropId::PropIdEnumeration& enumeration_id, Error& err = Error::Default()) const
		{
			return static_cast<PixelFormat>(getValueInt64(enumeration_id.prop_name, err));
		}
#endif

	public:
		/**
		 * @brief Finds the property with a specified name in the property map.
		 *
		 * @param[in] prop_name	The name of the property to find
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		Property find(const char* prop_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find(ptr_, prop_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, Property());
			}

			return detail::clearReturn(err, detail::prop_attach<Property>(prop));
		}
		/**
		 * @brief Finds the property with a specified name in the property map.
		 *
		 * @param[in] prop_name	The name of the property to find
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		Property find(const std::string& prop_name, Error& err = Error::Default()) const
		{
			return find(prop_name.c_str(), err);
		}

		/**
		 * @brief Finds the command property with a specified identifier in the property map.
		 *
		 * @param[in] command_id	The identifier of the command property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCommand find(const PropId::PropIdCommand& command_id, Error& err = Error::Default()) const
		{
			return findCommand(command_id.prop_name, err);
		}
		/**
		 * @brief Finds the command property with a specified name in the property map.
		 *
		 * @param[in] command_name	The name of the command property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCommand findCommand(const std::string& command_name, Error& err = Error::Default()) const
		{
			return findCommand(command_name.c_str(), err);
		}
		/**
		 * @brief Finds the command property with a specified name in the property map.
		 *
		 * @param[in] command_name	The name of the command property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCommand findCommand(const char* command_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_command(ptr_, command_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropCommand());
			}

			return detail::clearReturn(err, detail::prop_attach<PropCommand>(prop));
		}

		/**
		 * @brief Finds the integer property with a specified identifier in the property map.
		 *
		 * @param[in] integer_id	The identifier of the integer property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropInteger find(const PropId::PropIdInteger& integer_id, Error& err = Error::Default()) const
		{
			return findInteger(integer_id.prop_name, err);
		}
		/**
		 * @brief Finds the integer property with a specified name in the property map.
		 *
		 * @param[in] integer_name	The name of the integer property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropInteger findInteger(const std::string& integer_name, Error& err = Error::Default()) const
		{
			return findInteger(integer_name.c_str(), err);
		}
		/**
		 * @brief Finds the integer property with a specified name in the property map.
		 *
		 * @param[in] integer_name	The name of the integer property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropInteger findInteger(const char* integer_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_integer(ptr_, integer_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropInteger());
			}

			return detail::clearReturn(err, detail::prop_attach<PropInteger>(prop));
		}

		/**
		 * @brief Finds the float property with a specified identifier in the property map.
		 *
		 * @param[in] float_id	The identifier of the float property to find
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropFloat find(const PropId::PropIdFloat& float_id, Error& err = Error::Default()) const
		{
			return findFloat(float_id.prop_name, err);
		}
		/**
		 * @brief Finds the float property with a specified name in the property map.
		 *
		 * @param[in] float_name	The name of the float property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropFloat findFloat(const std::string& float_name, Error& err = Error::Default()) const
		{
			return findFloat(float_name.c_str(), err);
		}
		/**
		 * @brief Finds the float property with a specified name in the property map.
		 *
		 * @param[in] float_name	The name of the float property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropFloat findFloat(const char* float_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_float(ptr_, float_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropFloat());
			}

			return detail::clearReturn(err, detail::prop_attach<PropFloat>(prop));
		}

		/**
		 * @brief Finds the boolean property with a specified identifier in the property map.
		 *
		 * @param[in] boolean_id	The identifier of the boolean property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropBoolean find(const PropId::PropIdBoolean& boolean_id, Error& err = Error::Default()) const
		{
			return findBoolean(boolean_id.prop_name, err);
		}
		/**
		 * @brief Finds the boolean property with a specified name in the property map.
		 *
		 * @param[in] boolean_name	The name of the boolean property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropBoolean findBoolean(const std::string& boolean_name, Error& err = Error::Default()) const
		{
			return findBoolean(boolean_name.c_str(), err);
		}
		/**
		 * @brief Finds the boolean property with a specified name in the property map.
		 *
		 * @param[in] boolean_name	The name of the boolean property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropBoolean findBoolean(const char* boolean_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_boolean(ptr_, boolean_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropBoolean());
			}

			return detail::clearReturn(err, detail::prop_attach<PropBoolean>(prop));
		}

		/**
		 * @brief Finds the string property with a specified identifier in the property map.
		 *
		 * @param[in] string_id	The identifier of the string property to find
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropString find(const PropId::PropIdString& string_id, Error& err = Error::Default()) const
		{
			return findString(string_id.prop_name, err);
		}
		/**
		 * @brief Finds the string property with a specified name in the property map.
		 *
		 * @param[in] string_name	The name of the string property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropString findString(const std::string& string_name, Error& err = Error::Default()) const
		{
			return findString(string_name.c_str(), err);
		}
		/**
		 * @brief Finds the string property with a specified name in the property map.
		 *
		 * @param[in] string_name	The name of the string property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropString findString(const char* string_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_string(ptr_, string_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropString());
			}

			return detail::clearReturn(err, detail::prop_attach<PropString>(prop));
		}

		/**
		 * @brief Finds the enumeration property with a specified identifier in the property map.
		 *
		 * @param[in] enumeration_id	The identifier of the enumeration property to find
		 * @param[out] err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumeration find(const PropId::PropIdEnumeration& enumeration_id, Error& err = Error::Default()) const
		{
			return findEnumeration(enumeration_id.prop_name, err);
		}
		/**
		 * @brief Finds the enumeration property with a specified name in the property map.
		 *
		 * @param[in] enumeration_name	The name of the enumeration property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumeration findEnumeration(const std::string& enumeration_name, Error& err = Error::Default()) const
		{
			return findEnumeration(enumeration_name.c_str(), err);
		}
		/**
		 * @brief Finds the enumeration property with a specified name in the property map.
		 *
		 * @param[in] enumeration_name	The name of the enumeration property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropEnumeration findEnumeration(const char* enumeration_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_enumeration(ptr_, enumeration_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropEnumeration());
			}

			return detail::clearReturn(err, detail::prop_attach<PropEnumeration>(prop));
		}

		/**
		 * @brief Finds the register property with a specified identifier in the property map.
		 *
		 * @param[in] register_id	The identifier of the register property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name matches @a identifier.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropRegister find(const PropId::PropIdRegister& register_id, Error& err = Error::Default()) const
		{
			return findRegister(register_id.prop_name, err);
		}
		/**
		 * @brief Finds the register property with a specified name in the property map.
		 *
		 * @param[in] register_name	The name of the register property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropRegister findRegister(const std::string& register_name, Error& err = Error::Default()) const
		{
			return findRegister(register_name.c_str(), err);
		}
		/**
		 * @brief Finds the register property with a specified name in the property map.
		 *
		 * @param[in] register_name	The name of the register property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropRegister findRegister(const char* register_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_register(ptr_, register_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropRegister());
			}

			return detail::clearReturn(err, detail::prop_attach<PropRegister>(prop));
		}

		/**
		 * @brief Finds the category property with a specified name in the property map.
		 *
		 * @param[in] category_name	The name of the category property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCategory findCategory(const std::string& category_name, Error& err = Error::Default()) const
		{
			return findCategory(category_name.c_str(), err);
		}
		/**
		 * @brief Finds the category property with a specified name in the property map.
		 *
		 * @param[in] category_name	The name of the category property to find
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	The property whose name is equal to @a prop_name.\n
		 *			If no matching property is found, an invalid property object is returned.
		 *			Check @ref Property::is_valid() or the @a err output parameter.
		 */
		PropCategory findCategory(const char* category_name, Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY* prop = nullptr;
			if (!c_interface::ic4_propmap_find_category(ptr_, category_name, &prop))
			{
				return detail::updateFromLastErrorReturn(err, PropCategory{});
			}

			return detail::clearReturn(err, detail::prop_attach<PropCategory>(prop));
		}		

	public:
		/**
		 * @brief Enables the use of the chunk data in the passed @ref ImageBuffer as a backend for chunk properties in the property map.
		 * 
		 * @param[in] image_buffer	An image buffer with chunk data.\n
		 *							This parameter may be @c nullptr to disconnect the previously connected buffer.
		 * 
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @remarks
		 * The property map takes a reference to the passed image buffer, extending its lifetime and preventing automatic reuse.
		 * The reference is released when a new image buffer is connected to the property map, or @c nullptr is passed in the @a image_buffer argument.
		 */
		bool connectChunkData(const std::shared_ptr<ic4::ImageBuffer>& image_buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_connect_chunkdata(ptr_, detail::buffer_ptr(image_buffer)));
		}

	public:
		/**
		 * @brief Saves the state of the properties in this property map in a memory buffer.
		 * 
		 * @param[out] buffer		A reference to a @c std::vector to receive the serialized property data
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropertyMap::deSerialize(const std::vector<uint8_t>&, Error&)
		 */
		bool serialize(std::vector<uint8_t>& buffer, Error& err = Error::Default()) const
		{
			void* data = nullptr;
			size_t data_size = 0;
			if (!c_interface::ic4_propmap_serialize_to_memory(ptr_, std::malloc, &data, &data_size))
			{
				return detail::returnUpdateFromLastError(err, false);
			}

			buffer.resize(data_size);
			memcpy(buffer.data(), data, data_size);
			std::free(data);

			return detail::clearReturn(err, true);
		}
		/**
		 * @brief Saves the state of the properties in this property map into a file.
		 * 
		 * @param[in] file_path		A path to a file that the property state is written to
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropertyMap::deSerialize(const std::string&, Error&)
		 */
		bool serialize(const std::string& file_path, Error& err = Error::Default()) const
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_serialize_to_file(ptr_, file_path.c_str()));
		}

		/**
		 * @brief Restores the state of the properties in this property map from a memory buffer containing data that was previously written by @ref PropertyMap::serialize().
		 * 
		 * @param[in] buffer		A @c std::vector containing the serialized property data
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 * 
		 * @see PropertyMap::serialize(std::vector<uint8_t>&, Error&) const
		 */
		bool deSerialize(const std::vector<uint8_t>& buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_deserialize_from_memory(ptr_, buffer.data(), buffer.size()));
		}
		/**
		 * @brief Restores the state of the properties in this property map from a file that was previously written by @ref PropertyMap::serialize().
		 * 
		 * @param[in] file_path	Path to a file containing the serialized property data
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 *
		 * @see PropertyMap::serialize(const std::string&, Error&) const
		 */
		bool deSerialize(const std::string& file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_deserialize_from_file(ptr_, file_path.c_str()));
		}
#ifdef _WIN32
		bool serialize(const std::wstring& file_path, Error& err = Error::Default()) const
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_serialize_to_fileW(ptr_, file_path.c_str()));
		}
		bool deSerialize(const std::wstring& file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_propmap_deserialize_from_fileW(ptr_, file_path.c_str()));
		}
#endif
	};

	namespace detail
	{
		inline c_interface::IC4_PROPERTY_MAP* propmap_ptr(const PropertyMap& map)
		{
			return map.ptr_;
		}
		inline PropertyMap propmap_attach(c_interface::IC4_PROPERTY_MAP* map)
		{
			return PropertyMap(map, false);
		}
		inline PropertyMap propmap_wrap(c_interface::IC4_PROPERTY_MAP* map)
		{
			return PropertyMap(map, true);
		}
		inline PropertyMap propmap_invalid()
		{
			return PropertyMap();
		}

		template<typename T>
		inline T prop_attach(c_interface::IC4_PROPERTY* p)
		{
			return T(p, false);
		}
		template<typename T>
		inline T prop_wrap(c_interface::IC4_PROPERTY* p)
		{
			return T(p, true);
		}
	}
}

#endif //IC4_PROPERTIES_H_INC_