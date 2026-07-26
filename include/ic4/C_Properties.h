
#ifndef IC4_C_PROPERTIES_H_INC_
#define IC4_C_PROPERTIES_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** @defgroup properties Properties
 *
 * @brief Functions used to query, read or change properties of a component, usually the video capture device.
 * 
 * Properties are accessed through a #IC4_PROPERTY_MAP. For example, to get the property map for the device
 * properties of the video capture device opened by a grabber, call #ic4_grabber_device_get_property_map().
 * 
 * Each property is identified by a string identifier, for example \c ExposureTime or \c Gain.
 * Many property identifiers are defined by the <em>GenICam Standard Feature Naming Convention</em> (SFNC),
 * but devices can also have additional device-specific properties.
 * 
 * Each property has a display name, a description, a type, and visibility and availability information.
 * The display name can be used in user interfaces to show a friendly name for the property, while the description can
 * contain additional information about the property's meaning and/or usage.
 * 
 * There are functions to directly interact with properties, such as #ic4_propmap_set_value_int64() or
 * #ic4_propmap_set_value_double() that can be used to quickly configure the value
 * of a property with a known type and value.
 * 
 * Detailed property information and configuration is achieved through an object-oriented interface.
 * To get create a property object (#IC4_PROPERTY), call #ic4_propmap_find(), passing the property's known identifier.
 *
 * A property object can be interogated for generic property attributes, for example
 *	- Its type (#ic4_prop_get_type())
 *	- Its display name (#ic4_prop_get_display_name())
 *	- Its description (#ic4_prop_get_description())
 * 
 * Each type has different methods of interaction and can provide additional type-dependent information:
 *	- Integer properties\n
 *		These are used to configure settings that are represented by whole numbers, such as the \c Width or \c Height of the ROI
 *		that is being read out from the image sensor.\n
 *		An integer property defines a minimum and maximum value as well as a stepping for valid values.\n
 *		They can define additional additional attributes such as a, a unit or display hints.\n
 *		To interact with integer properties, call one of the \c ic4_prop_integer_* functions.
 * 
 *	- Float properties\n
 *		These are used to configure settings that are represented by floating-point numbers, such as the \c ExposureTime or \c AcquisitionFrameRate.\n
 *		A float property defines a minimum and maximum value as well as a stepping for valid values.\n
 *		They can define additional additional attributes such as a unit or display hints.\n
 *		To interact with float properties, call one of the \c ic4_prop_float_* functions.
 * 
 *	- Command properties\n
 *		A command property represents an action that a device can execute.\n
 *		To interact with command properties, call one of the \c ic4_prop_command_* functions.
 * 
 *	- Boolean properties\n
 *		A boolean property is a simple on/off switch.
 *		To interact with boolean properties, call one of the \c ic4_prop_boolean_* functions.
 * 
 *	- Enumeration properties\n
 *		Enumeration properties let the user choose from a set of valid values identified by a string or numeric value.\n
 *		They are used to configure camera settings such as the \c PixelFormat.
 *		To interact with enumeration properties, call one of the \c ic4_prop_enum_* functions.
 *		
 *	- String properties\n
 *		String properties are mostly used to provide device information in text form.
 *		To interact with string properties, call one of the \c ic4_prop_string_* functions.
 * 
  *	- Register properties\n
 *		Register properties represent fixed-length arrays of raw byte data.
 *		To interact with register properties, call one of the \c ic4_prop_register_* functions.
 * 
 *	- Category properties\n
 *		Category properties are a special properties without a value, but they contain links to other properties.\n
 *		That way, they span a tree of properties starting from the \c Root category. This tree can be used to structure user interfaces.
 *		To interact with category properties, call one of the \c ic4_prop_category_* functions. 
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
	 * @defgroup propmap Property Map
	 * 
	 * @brief Represents the property interface of a component, usually a video capture device.
	 * 
	 * @{
	 */

	/**
	 * @struct IC4_PROPERTY_MAP
	 * 
	 * @brief Represents the property interface of a component, usually a video capture device.
	 * 
	 * A property map offers quick access to known properties as well as functions to enumerate all features through the category tree.
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_PROPERTY_MAP*.
	 * 
	 * Property maps are created by their respective component when asked for, for example by calling #ic4_grabber_device_get_property_map()
	 * or #ic4_videowriter_get_property_map().
	 * 
	 * Property map objects are reference-counted. 
	 * To share a propertyp map between multiple parts of a program, create a new reference by calling #ic4_propmap_ref().
	 * When a reference is no longer required, call #ic4_propmap_unref(). The property map is destroyed when the final reference
	 * is released.
	 * 
	 */
	struct IC4_PROPERTY_MAP;

	/**
	 * @}
	 */


	/**
	 * @defgroup propobj Property Objects
	 *
	 * @brief Represents a property of a component, usually of a video capture device.
	 * 
	 * @{
	 */

	/**
	 * @struct IC4_PROPERTY
	 * 
	 * @brief Represents a property of a component, usually of a video capture device.
	 * 
	 * All property types are referred to by pointers to \c IC4_PROPERTY, there are no specialized types for
	 * different property types.
	 * 
	 * \c IC4_PROPERTY also represents enumeration entries, even though they are not actually property that is part of
	 * the category tree. Nevertheless, enumeration entries support all standard property operations.
	 * 
	 * Property objects are created in multiple ways:
	 *	- By calling @ref ic4_propmap_find() or one of its typed sibling functions like @ref ic4_propmap_find_integer().
	 *	- By calling @ref ic4_proplist_at() to retrieve a property out of a property list, that was previously obtained:
	 *		- By calling @ref ic4_prop_category_get_features() to get all properties from a category.
	 *		- By calling @ref ic4_prop_enum_get_entries() to get the enumeration entries of an enumeration property.
	 *		- By calling @ref ic4_prop_get_selected_props() to get the properties selected by a property.
	 *		- By calling @ref ic4_propmap_get_all() to get all properties in a property map's category tree.
	 * 
	 * Property objects obtained in any of these ways that refer to the same device property will always have the same pointer value.
	 * 
	 * Calling a function expecting a property with a certain property type on a property with a different type will fail
	 * and the error value will be set to @ref IC4_ERROR_GENICAM_TYPE_MISMATCH.
	 */
	struct IC4_PROPERTY;

	/**
	 * @brief Defines the possible property types
	 * 
	 * The property type defines the possible operations on a property and its data type.
	 */
	enum IC4_PROPERTY_TYPE
	{
		IC4_PROPTYPE_INVALID,		///< Not a valid property type, indicates an error

		IC4_PROPTYPE_INTEGER,		///< Integer property
		IC4_PROPTYPE_FLOAT,			///< Float property
		IC4_PROPTYPE_ENUMERATION,	///< Enumeration property
		IC4_PROPTYPE_BOOLEAN,		///< Boolean property
		IC4_PROPTYPE_STRING,		///< String property
		IC4_PROPTYPE_COMMAND,		///< Command property

		IC4_PROPTYPE_CATEGORY,		///< Category property
		IC4_PROPTYPE_REGISTER,		///< Register property
		IC4_PROPTYPE_PORT,			///< Port property

		IC4_PROPTYPE_ENUMENTRY,		///< Enumeration entry property
	};

	/**
	 * @brief Defines the possible property visibilities
	 *
	 * Each property has a visibility hint that can be used to create user interfaces for different user types.
	 */
	enum IC4_PROPERTY_VISIBILITY
	{
		IC4_PROPVIS_BEGINNER,		///< Beginner visibility
		IC4_PROPVIS_EXPERT,			///< Expert visibility
		IC4_PROPVIS_GURU,			///< Guru visibility
		IC4_PROPVIS_INVISIBLE		///< Invisible
	};

	/**
	 * @brief Defines the possible property increment modes for Integer and Float properties
	 */
	enum IC4_PROPERTY_INCREMENT_MODE
	{
		/**
		 * @brief The property used a fixed step between valid values.
		 * 
		 * Use #ic4_prop_integer_get_inc() or #ic4_prop_float_get_inc() to get the property's step size.
		 */
		IC4_PROPINCMODE_INCREMENT,
		/**
		 * @brief The property defines a set of valid values.
		 * 
		 * Use @ref ic4_prop_integer_get_valid_value_set() or @ref ic4_prop_float_get_valid_value_set() to query the set of valid values.
		 */
		IC4_PROPINCMODE_VALUESET,
		/**
		 * @brief The property allows setting all values between its minimum and maximum value.
		 * 
		 * This mode is only valid for Float properties.
		 * 
		 * Integer properties report increment 1 if they want to allow every possible value between their minimum and maximum value.
		 */
		IC4_PROPINCMODE_NONE,
	};

	/**
	 * @}
	 */

	/**
	 * @defgroup proplist Property Lists
	 * 
	 * @brief Property lists represent groups of properties.
	 *
	 * They are returned from functions such as @ref ic4_prop_category_get_features() or @ref ic4_prop_enum_get_entries().
	 * 
	 * Property lists are read-only and can be enumerated by using the @ref ic4_proplist_size() and @ref ic4_proplist_at() functions.
	 * 
	 * @{
	 */

	/**
	 * @struct IC4_PROPERTY_LIST
	 * 
	 * @brief Represents a list of properties.
	 */
	struct IC4_PROPERTY_LIST;

	/**
	 * @}
	 */

	struct IC4_IMAGE_BUFFER;

	/**
	 * @addtogroup propmap
	 *
	 * @{
	 */

	/**
	 * @brief Increases the property map's internal reference count by one.
	 *
	 * @param[in] map A property map
	 *
	 * @return The pointer passed via \a map
	 *
	 * @remarks If \a map is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_propmap_unref
	 */
	IC4CORE_API struct IC4_PROPERTY_MAP* ic4_propmap_ref(struct IC4_PROPERTY_MAP* map);
	/**
	 * @brief Decreases the property map's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] map A property map
	 *
	 * @remarks
	 * If \a map is \c NULL, the function does nothing. An error value is not set.
	 * 
	 * @remarks
	 * #IC4_PROPERTY objects retrieved from this property map will stay valid after the property map has been destroyed.
	 *
	 * @see ic4_propmap_ref
	 */
	IC4CORE_API void ic4_propmap_unref(struct IC4_PROPERTY_MAP* map);

	/**
	 * @brief Execute a command with a known name.
	 * 
	 * @param[in] map		A property map
	 * @param[in] prop_name	Name of a command property inside \c map
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If there is no with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a command property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_execute_command(struct IC4_PROPERTY_MAP* map, const char* prop_name);

	/**
	 * @brief Get the value of a property with a known name as an integer value.
	 * 
	 * The behavior depends on the type of the property:
	 * - For integer properties, the value is returned directly.
	 * - For boolean properties, the value returned is @c 1 or @c 0.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 * 
	 * @param[in] map			A property map
	 * @param[in] prop_name		Name of a property inside @c map
	 * @param[out] pValue		Pointer to an integer to receive the value of the property
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_get_value_int64(struct IC4_PROPERTY_MAP* map, const char* prop_name, int64_t* pValue);
	/**
	 * @brief Get the value of a property with a known name as a double value.
	 *
	 * The behavior depends on the type of the property:
	 * - For integer properties, the value is converted to @c double.
	 * - For float properties, the value is returned directly.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map			A property map
	 * @param[in] prop_name		Name of a property inside @c map
	 * @param[out] pValue		Pointer to a double to receive the value of the property
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_get_value_double(struct IC4_PROPERTY_MAP* map, const char* prop_name, double* pValue);
	/**
	 * @brief Get the value of a property with a known name as a bool value.
	 *
	 * The behavior depends on the type of the property:
	 * - For boolean properties, the value is returned directly.
	 * - For enumeration properties, a value is returned if the name of the currently selected entry unambiguously suggests to represent @c true or @c false.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map			A property map
	 * @param[in] prop_name		Name of a property inside @c map
	 * @param[out] pValue		Pointer to a double to receive the value of the property
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_get_value_bool(struct IC4_PROPERTY_MAP* map, const char* prop_name, bool* pValue);
	/**
	 * @brief Get the value of a property with a known name as a string value.
	 *
	 * The behavior depends on the type of the property:
	 * - For integer properties, the value is converted to a string
	 * - For float properties, the value is converted to a string
	 * - For boolean properties, the value is converted to the string @c "true" or @c "false".
	 * - For enumeration properties, the name of the currently selected entry is returned.
	 * - For string properties, the value is returned directly.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map				A property map
	 * @param[in] prop_name			Name of a property inside \c map
	 * @param[out] buffer			Pointer to a character array to receive the string value.\n
	 *								This parameter can be \c NULL to find out the required space without allocating an initial array.
	 * @param[in, out] buffer_size	Pointer to a \c size_t describing the length of the array pointed to by \a buffer.\n
	 *								The function always writes the actual number of characters required to store the string representation.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_get_value_string(struct IC4_PROPERTY_MAP* map, const char* prop_name, char* buffer, size_t* buffer_size);

	/**
	 * @brief Set the value of a property with a known name to the passed integer value.
	 * 
	 * The behavior depends on the type of the property:
	 * - For integer properties, the value is set directly.
	 * - For float properties, the value is set directly.
	 * - For boolean properties, if the value is @c 1 or @c 0, it is set to @c true or @c false respectively. Other values result in an error.
	 * - For enumeration properties, the numeric value is set if the property is @c PixelFormat. Other properties result in an error.
	 * - For command properties, the command is executed if @a value is @c 1.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map		A property map
	 * @param[in] prop_name	Name of a property inside \c map
	 * @param[in] value		New value to be set for the property identified by \c prop_name
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_set_value_int64(struct IC4_PROPERTY_MAP* map, const char* prop_name, int64_t value);
	/**
	 * @brief Set the value of a property with a known name to the passed double value.
	 * 
	 * The behavior depends on the type of the property:
	 * - For integer properties, the value is rounded to the nearest integer.
	 * - For float properties, the value is set directly.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map		A property map
	 * @param[in] prop_name	Name of a property inside \c map
	 * @param[in] value		New value to be set for the property identified by \c prop_name
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_set_value_double(struct IC4_PROPERTY_MAP* map, const char* prop_name, double value);
	/**
	 * @brief Set the value of a property with a known name to the passed bool value.
	 * 
	 * The behavior depends on the type of the property:
	 * - For boolean properties, the value is set directly.
	 * - For enumeration properties, it selects the entry with a name that unambiguously suggests to represent @c true or @c false, if available.
	 * - For command properties, the command is executed if @a value is @c true.
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map		A property map
	 * @param[in] prop_name	Name of a property inside \c map
	 * @param[in] value		New value to be set for the property identified by \c prop_name
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_set_value_bool(struct IC4_PROPERTY_MAP* map, const char* prop_name, bool value);
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
	 * - For all other property types, the call results in an error (@ref IC4_ERROR_GENICAM_TYPE_MISMATCH).
	 *
	 * @param[in] map		A property map
	 * @param[in] prop_name	Name of a property inside \c map
	 * @param[in] value		New value to be set for the property identified by \c prop_name
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name in \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_set_value_string(struct IC4_PROPERTY_MAP* map, const char* prop_name, const char* value);

	/**
	 * @brief Returns a property object representing the property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 */
	IC4CORE_API bool ic4_propmap_find(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the command property with the passed name.
	 * 
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a command property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a command property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_command(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the integer property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of an integer property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_integer(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the float property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a float property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_float(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the boolean property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a boolean property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a boolean property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_boolean(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the string property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a string property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a string property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_string(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the enumeration property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of an enumeration property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_enumeration(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the register property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a register property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a register property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_register(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);
	/**
	 * @brief Returns a property object representing the category property with the passed name.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	prop_name	The name of a category property inside \c map
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If there is no property with the name \c prop_name inside \c map, the function fails and the error value is set to #IC4_ERROR_GENICAM_FEATURE_NOT_FOUND. \n
	 * If there is a property with the name \c prop_name, but it is not a category property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_propmap_find_category(struct IC4_PROPERTY_MAP* map, const char* prop_name, struct IC4_PROPERTY** ppProperty);

	/**
	 * @brief Returns a list of all properties reachable from the property map's "Root" category.
	 * 
	 * @param[in]	map			A property map
	 * @param[out]	ppList		A pointer to a property list receiving the list of properties.\n
	 *							When the property list is no longer required, release the object reference using #ic4_proplist_unref().
	 *
	 * @note
	 * This generated list does not include category properties.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_propmap_get_all(struct IC4_PROPERTY_MAP* map, struct IC4_PROPERTY_LIST** ppList);

	/**
	 * @brief Enables the use of the chunk data in the passed #IC4_IMAGE_BUFFER as a backend for chunk properties in the property map.
	 *
	 * @param[in]	map				A property map
	 * @param[in]	image_buffer	An image buffer with chunk data.\n
	 *								This parameter may be \c NULL to disconnect the previously connected buffer.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * The property map takes a reference to the passed image buffer, extending its lifetime and preventing automatic reuse.
	 * The reference is released when a new image buffer is connected to the property map, or \c NULL is passed in the \c image_buffer argument.
	 * 
	 */
	IC4CORE_API bool ic4_propmap_connect_chunkdata(struct IC4_PROPERTY_MAP* map, struct IC4_IMAGE_BUFFER* image_buffer);

	/**
	 * @brief Saves the state of the properties in this property map into a file.
	 * 
	 * @param[in]	map		A property map
	 * @param[in]	path	A path to a file that the property state is written to
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note 
	 * The actual set of properties that is stored by this function is controlled by the object the property map belongs to.\n
	 * To restore the properties at a later time, use #ic4_propmap_deserialize_from_file.
	 * 
	 * @see ic4_propmap_deserialize_from_file
	 * @see ic4_propmap_serialize_to_memory
	 */
	IC4CORE_API bool ic4_propmap_serialize_to_file(struct IC4_PROPERTY_MAP* map, const char* path);
#ifdef _WIN32
	IC4CORE_API bool ic4_propmap_serialize_to_fileW(struct IC4_PROPERTY_MAP* map, const wchar_t* path);
#endif

	/**
	 * @brief Callback function called to allocate memory during the call of #ic4_propmap_serialize_to_memory.
	 * 
	 * @param[in]	size	Size of the memory buffer to be allocated.
	 * 
	 * @return		The pointer to the allocated memory buffer, or @c NULL if the allocation was not possible.
	 * 
	 * @note
	 * If this function returns @c NULL, the call to #ic4_propmap_serialize_to_memory will fail.
	 * 
	 * @see ic4_propmap_serialize_to_memory
	 */
	typedef void* (*ic4_serialization_allocator)(size_t size);

	/**
	 * @brief Saves the state of the properties in this property map in a memory buffer.
	 *
	 * @param[in]		map			A property map
	 * @param[in]		alloc		Pointer to a function that allocates the memory buffer.\n
	 *								For example, @c malloc can be passed here.
	 * @param[out]		ppData		Pointer to a pointer to receive the newly-allocated memory buffer containing the properties.\n
	 *								The caller is responsible for releasing the memory, using a function that can free memory returned by @c alloc.
	 * @param[out]		data_size	Pointer to size_t to receive the size of the memory buffer allocated by the call
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * The actual set of properties that is stored by this function is controlled by the object the property map belongs to.\n
	 * To restore the properties at a later time, use #ic4_propmap_deserialize_from_memory.
	 *
	 * @see ic4_propmap_deserialize_from_memory
	 * @see ic4_propmap_serialize_to_file
	 */
	IC4CORE_API bool ic4_propmap_serialize_to_memory(struct IC4_PROPERTY_MAP* map, ic4_serialization_allocator alloc, void** ppData, size_t* data_size);

	/**
	 * @brief Restores the state of the properties in this property map from a file that was previously written by #ic4_propmap_serialize_to_file.
	 * 
	 * @param[in]	map		A property map
	 * @param[in]	path	Path to a file that was previously written by #ic4_propmap_serialize_to_file
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note
	 * If the file contains settings for properties that could not be written, the function fails and the error value is set to #IC4_ERROR_INCOMPLETE.
	 * 
	 * @see ic4_propmap_serialize_to_file
	 */
	IC4CORE_API bool ic4_propmap_deserialize_from_file(struct IC4_PROPERTY_MAP* map, const char* path);
#ifdef _WIN32
	IC4CORE_API bool ic4_propmap_deserialize_from_fileW(struct IC4_PROPERTY_MAP* map, const wchar_t* path);
#endif

	/**
	 * @brief Restores the state of the properties in this property map from a memory buffer containing data that was previously written by #ic4_propmap_serialize_to_memory.
	 *
	 * @param[in]	map			A property map
	 * @param[in]	pData		Pointer to a memory buffer containing data that was written by #ic4_propmap_serialize_to_memory
	 * @param[in]	data_size	Size of the memory buffer pointed to by @c pData
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * If the memory buffer contains settings for properties that could not be written, the function fails and the error value is set to #IC4_ERROR_INCOMPLETE.
	 *
	 * @see ic4_propmap_serialize_to_memory
	 */
	IC4CORE_API bool ic4_propmap_deserialize_from_memory(struct IC4_PROPERTY_MAP* map, const void* pData, size_t data_size);

	/**
	 * @}
	 */

	/**
	 * @addtogroup propobj
	 *
	 * @{
	 */

	/**
	 * @brief Increases the property's internal reference count by one.
	 *
	 * @param[in] prop A property
	 *
	 * @return The pointer passed via \a prop
	 *
	 * @remarks If \a prop is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_prop_unref
	 */
	IC4CORE_API struct IC4_PROPERTY* ic4_prop_ref(struct IC4_PROPERTY* prop);
	/**
	 * @brief Decreases the property's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] prop A property
	 *
	 * @remarks
	 * If \a prop is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_prop_ref
	 */
	IC4CORE_API void ic4_prop_unref(struct IC4_PROPERTY* prop);

	/**
	 * @brief Returns the type of the passed property
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	The type of the property \c prop, or #IC4_PROPTYPE_INVALID in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @see IC4_PROPERTY_TYPE
	 */
	IC4CORE_API enum IC4_PROPERTY_TYPE ic4_prop_get_type(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the name of the passed property
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	The name of the passed property, or \c NULL in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.
	 * 
	 * @remarks
	 * A property's name is the symbolic name with which it can be found in a @ref propmap, for example \c ExposureTime or \c AcquisitionFrameRate.
	 * 
	 * @see ic4_prop_get_display_name
	 */
	IC4CORE_API const char* ic4_prop_get_name(struct IC4_PROPERTY* prop);
	/**
	 * @brief Checks whether a property is currently available.
	 * 
	 * If a property is not available, attempts to read or write its value will fail.
	 * 
	 * A property may become unavailable, if its value does not have a meaning in the current state of the device.
	 * The property's availability status can change upon writing to another property.
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	\c true, if the property is currently available, otherwise \c false.\n
	 *			If there is an error, this function returns \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_prop_is_locked
	 * @see ic4_prop_is_readonly
	 */
	IC4CORE_API bool ic4_prop_is_available(struct IC4_PROPERTY* prop);
	/**
	 * @brief Checks whether a property is currently locked.
	 * 
	 * A locked property can be read, but attempts to write its value will fail.
	 * 
	 * A property's locked status may change upon writing to another property.
	 * 
	 * Common examples for locked properties are \c ExposureTime or \c Gain if \c ExposureAuto or \c GainAuto are enabled.
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	\c true, if the property is currently locked, otherwise \c false.\n
	 *			If there is an error, this function returns \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_prop_is_available
	 * @see ic4_prop_is_readonly
	 * @see ic4_prop_is_likely_locked_by_stream
	 */
	IC4CORE_API bool ic4_prop_is_locked(struct IC4_PROPERTY* prop);
	/**
	 * @brief Tries to determine whether a property is locked because a data stream is active.
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	@c true, if the property is currently locked, and will likely be unlocked if the data stream is stopped.\n
	 *			@c false, if the property is not currently locked, or stopping the data stream will probably not lead to
	 *			the property being unlocked.\n
	 *			If there is an error, this function returns \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks	For technical reasons, this function cannot always accurately predict the future.
	 * 
	 * @see ic4_prop_is_locked
	 */
	IC4CORE_API bool ic4_prop_is_likely_locked_by_stream(struct IC4_PROPERTY* prop);
	/**
	 * @brief Checks whether a property is read-only.
	 * 
	 * A read-only property will never be writable, the read-only status will never change.
	 * 
	 * A Common examples for read-only property is \c DeviceTemperature.
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	\c true, if the property is read-only, otherwise \c false.\n
	 *			If there is an error, this function returns \c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_prop_is_available
	 * @see ic4_prop_is_locked
	 */
	IC4CORE_API bool ic4_prop_is_readonly(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns a visibility hint for the property.
	 * 
	 * The visibility hint can be used to create user interfaces with different complexities. The most commonly used properties
	 * have the beginner visibility, while rarely used or diagnostic features might be tagged guru or even invisible.
	 * 
	 * @param[in] prop	A property
	 * 
	 * @return	The visibility hint for the property.\n
	 *			If there is an error, this function returns @ref IC4_PROPVIS_INVISIBLE. Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API enum IC4_PROPERTY_VISIBILITY ic4_prop_get_visibility(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the display name of the passed property
	 *
	 * @param[in] prop	A property
	 *
	 * @return	The display name of the passed property, or \c NULL in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.
	 *
	 * @remarks
	 * A property's display name is a text representation of the property that is meant to be displayed in user interfaces.\n
	 * For example, the display name of the \c ExposureTime property usually is <i>Exposure Time</i>.
	 *
	 * @see ic4_prop_get_name
	 */
	IC4CORE_API const char* ic4_prop_get_display_name(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns a tooltip for the passed property
	 *
	 * @param[in] prop	A property
	 *
	 * @return	The tooltip for the passed property, or \c NULL in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.
	 *
	 * @remarks
	 * A property's tooltip is a text that can be used when a tooltip is required by a user interface.\n
	 * Usually, the tooltip is a short description of the property.
	 *
	 * @see ic4_prop_get_description
	 */
	IC4CORE_API const char* ic4_prop_get_tooltip(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns a description text for the passed property
	 *
	 * @param[in] prop	A property
	 *
	 * @return	The description for the passed property, or \c NULL in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.
	 *
	 * @remarks
	 * A property's description is a short text that describes the property, usually in more detail than the tooltip.
	 *
	 * @see ic4_prop_get_tooltip
	 */
	IC4CORE_API const char* ic4_prop_get_description(struct IC4_PROPERTY* prop);

	/**
	 * @brief Property notification handler function pointer
	 * 
	 * @param[in] prop		The property that has changed
	 * @param[in] user_ptr	The user data that was specified when the notification handler was registered
	 * 
	 * @see ic4_prop_event_add_notification
	 */
	typedef void (*ic4_prop_notification)(struct IC4_PROPERTY* prop, void* user_ptr);
	/**
	 * @brief Property notification deleter
	 * 
	 * @param[in] user_ptr	The user data that was specified when the notification deleter was registered
	 * 
	 * @see ic4_prop_event_add_notification
	 * @see ic4_prop_event_remove_notification
	 */
	typedef void (*ic4_prop_notification_deleter)(void* user_ptr);

	/**
	 * @brief Registers a property notification handler
	 * 
	 * The property notification handler is called whenever an aspect of the property changes, for example its value or locked status.
	 * 
	 * @param[in] prop		A property
	 * @param[in] handler	The property notification handler to be called if \a prop changes
	 * @param[in] user_ptr	User data to be passed to the notification handler
	 * @param[in] deleter	The property notification deleter to be called if the notification is removed, or the property is destroyed
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * The \c deleter callback can useful to release resources that the caller passed in into \c user_ptr.
	 * 
	 * @remarks
	 * Multiple notification handlers can be registered for the same property, as long as the \c user_ptr parameter is different.
	 * 
	 * @see ic4_prop_event_remove_notification
	 * @see ic4_prop_notification
	 * @see ic4_prop_notification_deleter
	 */
	IC4CORE_API bool ic4_prop_event_add_notification(struct IC4_PROPERTY* prop, ic4_prop_notification handler, void* user_ptr, ic4_prop_notification_deleter deleter);
	/**
	 * @brief Unregisters a previously registered notification handler
	 * 
	 * @param[in] prop		A property with a registered notification handler
	 * @param[in] handler	The property notification handler to be removed
	 * @param[in] user_ptr	The user data that was specified when the handler was registered
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * The values for \c handler and \c user_ptr must exactly match the arguments of the previous successful call to #ic4_prop_event_add_notification().
	 * 
	 * @see ic4_prop_event_add_notification
	 */
	IC4CORE_API bool ic4_prop_event_remove_notification(struct IC4_PROPERTY* prop, ic4_prop_notification handler, void* user_ptr);

	/**
	 * @brief Indicates whether this property's value changes the meaning and/or value of other properties.
	 * 
	 * @param[in] prop			A property
	 * 
	 * @return @c true, if @c prop is a selector, otherwise @c false.\n
	 *			If an error occurs, the function returns @c false. Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_prop_get_selected
	 */
	IC4CORE_API bool ic4_prop_is_selector(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the list of properties whose values' meaning depend on this property.
	 *
	 * @param[in] prop			A property that is a selector
	 * @param[out] ppSelected	A pointer to a property list receiving the list of properties selected by \c prop.
	 *							When the property list is no longer required, release the object reference using #ic4_proplist_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a boolean property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_is_selector
	 */
	IC4CORE_API bool ic4_prop_get_selected_props(struct IC4_PROPERTY* prop, struct IC4_PROPERTY_LIST** ppSelected);

	/**
	 * @defgroup catprop Category Properties
	 * 
	 * @brief Category properties define a tree-relationship between all properties in a property map.
	 * 
	 * The root of the tree is always the category property with the name \c Root.
	 * 
	 * To find which properties are linked from a category, use #ic4_prop_category_get_features(), which returns a #IC4_PROPERTY_LIST.
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
	 * @{
	 */

	/**
	 * @brief Retrieves the list of properties in a property category.
	 * 
	 * @param[in] prop			A category property
	 * @param[in] ppFeatures	A pointer to a property list receiving the list of properties inside the category \c prop.
	 *							When the property list is no longer required, release the object reference using #ic4_proplist_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not a category property, this function returns \c false and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH.
	 */
	IC4CORE_API bool ic4_prop_category_get_features(struct IC4_PROPERTY* prop, struct IC4_PROPERTY_LIST** ppFeatures);

	/**
	 * @}
	 */

	/**
	 * @defgroup cmdprop Command Properties
	 * 
	 * @brief Command properties represent an action that can be performed by the device.
	 * 
	 * A common example for a command properties is \c TriggerSoftware.
	 *
	 * @{
	 */

	/**
	 * @brief Execute a command property
	 * 
	 * @param[in] prop			A command property
	 * 
	 * @return @c true on success, otherwise @c false.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not a command property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_command_execute(struct IC4_PROPERTY* prop);

	/**
	 * @brief Checks whether a command has finished executing.
	 * 
	 * @param prop				A command property
	 * @param is_done			Output parameter receiving the command's completion status.\n
	 *							@c true, if the command is completed. @c false, if the command is still executing.
	 * 
	 * @return @c true on success, otherwise @c false.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If the command was never executed before, the @a is_done is set to @c false.
	 */
	IC4CORE_API bool ic4_prop_command_is_done(struct IC4_PROPERTY* prop, bool* is_done);
	// #TODO ic4_prop_command_get_polling_time

	/**
	 * @}
	 */

	/**
	 * @defgroup intprop Integer Properties
	 * 
	 * @brief Integer properties represent a feature whose value is an integer number.
	 * 
	 * Common examples for a integer properties are \c Width or \c Height.
	 * 
	 * An integer property can limit the range of valid values.
	 * The range of possible values can be queried by calling #ic4_prop_integer_get_min() and #ic4_prop_integer_get_max().
	 * 
	 * In integer property supplies hints that can be useful when creating a user interface:
	 *	- A representation (#ic4_prop_integer_get_representation())
	 *	- A unit (#ic4_prop_integer_get_unit())
	 *
	 * @{
	 */

	/**
	 * @brief Defines the possible integer property representations
	 *
	 * Each integer property has a representation hint that can help creating more useful user interfaces.
	 */
	enum IC4_PROPERTY_INT_REPRESENTATION
	{
		IC4_PROPINTREP_LINEAR,			///< Suggest a slider to edit the value
		IC4_PROPINTREP_LOGARITHMIC,		///< Suggest a slider with logarithmic mapping
		IC4_PROPINTREP_BOOLEAN,			///< Suggest a checkbox
		IC4_PROPINTREP_PURENUMBER,		///< Suggest displaying a decimal number
		IC4_PROPINTREP_HEXNUMBER,		///< Suggest displaying a hexadecimal number
		IC4_PROPINTREP_IPV4ADDRESS,		///< Suggest treating the integer as a IPV4 address
		IC4_PROPINTREP_MACADDRESS		///< Suggest treating the integer as a MAC address
	};

	/**
	 * @brief Returns the suggested representation for an integer property.
	 * 
	 * The representation can be used as a hint when creating user interfaces.
	 * 
	 * @param[in] prop	An integer property
	 * 
	 * @return	The suggested representation for the property, or a default representation in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see IC4_PROPERTY_INT_REPRESENTATION
	 */
	IC4CORE_API enum IC4_PROPERTY_INT_REPRESENTATION ic4_prop_integer_get_representation(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the unit of an integer property.
	 *
	 * @param[in] prop	An integer property
	 *
	 * @return	The unit of the the property. The unit can be an empty string, if there is no unit for the property.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.\n
	 *			If an error occurs, the function returns @c NULL. Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API const char* ic4_prop_integer_get_unit(struct IC4_PROPERTY* prop);
	/**
	 * @brief Changes the value of an integer property.
	 * 
	 * @param[in] prop			An integer property
	 * @param[in] value			The new value to set
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If the device or component rejected the value, the function fails and the error value is set to #IC4_ERROR_GENICAM_VALUE_ERROR. \n
	 * If the value is currently not writable, the function fails and the error value is set to #IC4_ERROR_GENICAM_ACCESS_DENIED. \n
	 * 
	 * @see ic4_prop_integer_get_min
	 * @see ic4_prop_integer_get_max
	 * @see ic4_prop_integer_get_inc
	 */
	IC4CORE_API bool ic4_prop_integer_set_value(struct IC4_PROPERTY* prop, int64_t value);
	/** 
	 * @brief Reads the current value of an integer property.
	 * 
	 * @param[in] prop			An integer property
	 * @param[out] pValue		Pointer to an integer to receive the current value
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_integer_get_value(struct IC4_PROPERTY* prop, int64_t* pValue);
	/**
	 * @brief Returns the minimum value accepted by an integer property.
	 *
	 * @param[in] prop			An integer property
	 * @param[out] pMinimum		Pointer to an integer to receive the minimum value
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_integer_get_max
	 * @see ic4_prop_integer_get_inc
	 */
	IC4CORE_API bool ic4_prop_integer_get_min(struct IC4_PROPERTY* prop, int64_t* pMinimum);
	/**
	 * @brief Returns the maximum value accepted by an integer property.
	 *
	 * @param[in] prop			An integer property
	 * @param[out] pMaximum		Pointer to an integer to receive the maximum value
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_integer_get_min
	 * @see ic4_prop_integer_get_inc
	 */
	IC4CORE_API bool ic4_prop_integer_get_max(struct IC4_PROPERTY* prop, int64_t* pMaximum);
	/**
	 * @brief Returns the step size for valid values accepted by an integer property.
	 * 
	 * The increment restricts the set of valid values for an integer property.
	 * For example, if the property's minimum value is \c 0, the maximum is \c 10, and the increment is \c 2, \c 5 is not a valid value for the property
	 * and will be rejected when trying to write it.
	 *
	 * @param[in] prop			An integer property
	 * @param[out] pIncrement	Pointer to an integer to receive the increment
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an integer property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_integer_get_min
	 * @see ic4_prop_integer_get_max
	 */
	IC4CORE_API bool ic4_prop_integer_get_inc(struct IC4_PROPERTY* prop, int64_t* pIncrement);
	/**
	 * @brief Returns how this integer property restricts which values are valid between its minimum and maximum value.
	 * 
	 * @param[in] prop	An integer property
	 * 
	 * @return	#IC4_PROPINCMODE_INCREMENT if the property has a fixed step size between valid values.\n
	 *			#IC4_PROPINCMODE_VALUESET, if the property has a set of valid values.\n
	 *			If an error occurs, the function returns #IC4_PROPINCMODE_INCREMENT and the error value is set.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @see ic4_prop_integer_get_inc
	 * @see ic4_prop_integer_get_valid_value_set
	 */
	IC4CORE_API enum IC4_PROPERTY_INCREMENT_MODE ic4_prop_integer_get_inc_mode(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the set of valid values for an integer
	 * 
	 * @param[in] prop				An integer property restricted to a set of values
	 * @param[out] value_set		An array to receive the set of valid values
	 * @param[in, out] array_size	Pointer to a @c size_t indicating the length of @c value_set.\n
	 *								After the call, this contains the number of entries in this property's set of valid values.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @note
	 * If @c value_set is not @c NULL and @a array_size is \c NULL, the function fails and the error value is set to #IC4_ERROR_INVALID_PARAM_VAL.
	 * If @c *array_size is lower than the number of entries in this property's set of valid values, the function fails the error value is set to #IC4_ERROR_BUFFER_TOO_SMALL.
	 * If @c prop is not restricted by a set of valid values, the function fails and the error value is set to #IC4_ERROR_GENICAM_NOT_IMPLEMENTED.
	 * 
	 */
	IC4CORE_API bool ic4_prop_integer_get_valid_value_set(struct IC4_PROPERTY* prop, int64_t* value_set, size_t* array_size);

	/**
	 * @}
	 */

	/**
	 * @defgroup floatprop Float Properties
	 *
	 * @brief Float properties represent a feature whose value is a floating-point number.
	 *
	 * Common examples for a float properties are \c AcquisitionFrameRate, \c ExposureTime or \c Gain.
	 * 
	 * A float property can limit the range of valid values.
	 * The range of possible values can be queried by calling #ic4_prop_float_get_min() and #ic4_prop_float_get_max().
	 *
	 * In float property supplies hints that can be useful when creating a user interface:
	 *	- A representation (#ic4_prop_float_get_representation())
	 *	- A unit (#ic4_prop_float_get_unit())
	 *	- A display notation (#ic4_prop_float_get_display_notation())
	 *	- Display precision (#ic4_prop_float_get_display_precision())
	 * 
	 * @{
	 */

	/**
	 * @brief Defines the possible float property representations
	 *
	 * Each float property has a representation hint that can help creating more useful user interfaces.
	 */
	enum IC4_PROPERTY_FLOAT_REPRESENTATION
	{
		IC4_PROPFLOATREP_LINEAR,		///< Suggest a slider to edit the value
		IC4_PROPFLOATREP_LOGARITHMIC,	///< Suggest a slider with logarithmic mapping
		IC4_PROPFLOATREP_PURENUMBER,	///< Suggest displaying a number
	};

	/**
	 * @brief Defines the possible float property display notations
	 *
	 * Each float property has a display notation hint that can help creating more useful user interfaces.
	 */
	enum IC4_PROPERTY_DISPLAY_NOTATION
	{
		IC4_PROPDISPNOTATION_AUTOMATIC,	///< Use an automatic mechanism to determine the best display notation
		IC4_PROPDISPNOTATION_FIXED,		///< Suggest fixed point notation
		IC4_PROPDISPNOTATION_SCIENTIFIC	///< Suggest scientific notation
	};

	/**
	 * @brief Returns the suggested represenation for a float property.
	 *
	 * The representation can be used as a hint when creating user interfaces.
	 *
	 * @param[in] prop	A float property
	 *
	 * @return	The suggested representation for the property, or a default representation in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see IC4_PROPERTY_FLOAT_REPRESENTATION
	 */
	IC4CORE_API enum IC4_PROPERTY_FLOAT_REPRESENTATION ic4_prop_float_get_representation(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the unit of a float property.
	 *
	 * @param[in] prop	A float property
	 *
	 * @return	The unit of the the property. The unit can be an empty string, if there is no unit for the property.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid as long as the property object exists.
	 *
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API const char* ic4_prop_float_get_unit(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns a suggested display notation to use when displaying the float property's value.
	 * 
	 * @param[in] prop	A float property
	 * 
	 * @return	A display notation suggestion, or @ref IC4_PROPDISPNOTATION_AUTOMATIC if there is an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 * 
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API enum IC4_PROPERTY_DISPLAY_NOTATION ic4_prop_float_get_display_notation(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns a suggested number of significant digits to use when displaying the float property's value.
	 * 
	 * @param[in] prop	A float property
	 * 
	 * @return	The suggested number of significant digits for display, or a default value if there is an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 * 
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API int64_t ic4_prop_float_get_display_precision(struct IC4_PROPERTY* prop);
	/**
	 * @brief Changes the value of a float property.
	 *
	 * @param[in] prop			A float property
	 * @param[in] value			The new value to set
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If the device or component rejected the value, the function fails and the error value is set to #IC4_ERROR_GENICAM_VALUE_ERROR. \n
	 * If the value is currently not writable, the function fails and the error value is set to #IC4_ERROR_GENICAM_ACCESS_DENIED. \n
	 *
	 * @see ic4_prop_float_get_min
	 * @see ic4_prop_float_get_max
	 * @see ic4_prop_float_get_inc
	 */
	IC4CORE_API bool ic4_prop_float_set_value(struct IC4_PROPERTY* prop, double value);
	/**
	 * @brief Reads the current value of a float property.
	 *
	 * @param[in] prop			A float property
	 * @param[out] pValue		Pointer to a double to receive the current value
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_float_get_value(struct IC4_PROPERTY* prop, double* pValue);
	/**
	 * @brief Returns the minimum value accepted by a float property.
	 *
	 * @param[in] prop			A float property
	 * @param[out] pMinimum		Pointer to an double to receive the minimum value
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see ic4_prop_integer_get_max
	 * @see ic4_prop_integer_get_inc
	 */
	IC4CORE_API bool ic4_prop_float_get_min(struct IC4_PROPERTY* prop, double* pMinimum);
	/**
	 * @brief Returns the maximum value accepted by a float property.
	 *
	 * @param[in] prop			A float property
	 * @param[out] pMaximum		Pointer to an double to receive the maximum value
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see ic4_prop_integer_get_min
	 * @see ic4_prop_integer_get_inc
	 */
	IC4CORE_API bool ic4_prop_float_get_max(struct IC4_PROPERTY* prop, double* pMaximum);
	/**
	 * @brief Returns how this float property restricts which values are valid between its minimum and maximum value.
	 *
	 * @param[in] prop	A float property
	 *
	 * @return	@ref IC4_PROPINCMODE_INCREMENT, if the property has a fixed step size between valid values.\n
	 *			@ref IC4_PROPINCMODE_VALUESET, if the property has a set of valid values.\n
	 *			@ref IC4_PROPINCMODE_NONE, if the property can be set to any value between its minimum and maximum.\n
	 *			If an error occurs, the function returns @ref IC4_PROPINCMODE_NONE and the error value is set.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If @c prop is not a float property, the function fails and the error value is set to @ref IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see ic4_prop_float_get_inc
	 * @see ic4_prop_float_get_valid_value_set
	 */
	IC4CORE_API enum IC4_PROPERTY_INCREMENT_MODE ic4_prop_float_get_inc_mode(struct IC4_PROPERTY* prop);
	/**
	 * @brief Returns the step size for valid values accepted by a float property.
	 *
	 * The increment restricts the set of valid values for a float property.
	 * For example, if the property's minimum value is \c 0.0, the maximum is \c 1.0, and the increment is \c 0.5, \c 1.25 is not a valid value for the property
	 * and will be rejected when trying to write it.
	 *
	 * @param[in] prop			A float property
	 * @param[out] pIncrement	Pointer to a double to receive the increment
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a float property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see ic4_prop_float_get_min
	 * @see ic4_prop_float_get_max
	 * @see ic4_prop_float_has_inc
	 */
	IC4CORE_API bool ic4_prop_float_get_inc(struct IC4_PROPERTY* prop, double* pIncrement);
	/**
	 * @brief Returns the set of valid values for a float
	 *
	 * @param[in] prop				A float property restricted to a set of values
	 * @param[out] value_set		An array to receive the set of valid values
	 * @param[in, out] array_size	Pointer to a @c size_t indicating the length of @c value_set.\n
	 *								After the call, this contains the number of entries in this property's set of valid values.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @note
	 * If @c value_set is not @c NULL and @a array_size is \c NULL, the function fails and the error value is set to #IC4_ERROR_INVALID_PARAM_VAL.
	 * If @c *array_size is lower than the number of entries in this property's set of valid values, the function fails the error value is set to #IC4_ERROR_BUFFER_TOO_SMALL.
	 * If @c prop is not restricted by a set of valid values, the function fails and the error value is set to #IC4_ERROR_GENICAM_NOT_IMPLEMENTED.
	 *
	 */
	IC4CORE_API bool ic4_prop_float_get_valid_value_set(struct IC4_PROPERTY* prop, double* value_set, size_t* array_size);

	/**
	 * @}
	 */

	/**
	 * @defgroup boolprop Boolean Properties
	 *
	 * @brief Boolean properties represent a feature whose value is a simple on/off switch.
	 *
	 * @{
	 */

	/**
	 * @brief Changes the value of a boolean property.
	 *
	 * @param[in] prop			An boolean property
	 * @param[in] value			The new value to set
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a boolean property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If the value is currently not writable, the function fails and the error value is set to #IC4_ERROR_GENICAM_ACCESS_DENIED. \n
	 */
	IC4CORE_API bool ic4_prop_boolean_set_value(struct IC4_PROPERTY* prop, bool value);
	/**
	 * @brief Reads the current value of a boolean property.
	 *
	 * @param[in] prop			A boolean property
	 * @param[out] pValue		Pointer to a bool to receive the current value
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a boolean property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_boolean_get_value(struct IC4_PROPERTY* prop, bool* pValue);

	/**
	 * @}
	 */

	/**
	 * @defgroup stringprop String Properties
	 *
	 * String properties represent features whole value is a text.
	 *
	 * The maximum length of the text is indicated by @ref ic4_prop_string_get_max_len().
	 * 
	 * @{
	 */

	 /**
	  * @brief Reads the current value of a string property.
	  *
	  * @param[in] prop				A string property
	  * @param[out] buffer			Pointer to a character array to receive the string value.\n
	  *								This parameter can be \c NULL to find out the required space without allocating an initial array.
	  * @param[in, out] buffer_size	Pointer to a \c size_t describing the length of the array pointed to by \a buffer.\n
	  *								The function always writes the actual number of characters required to store the string representation.
	  *
	  * @return \c true on success, otherwise \c false.\n
	  *			Use ic4_get_last_error() to query error information.
	  *
	  * @remarks
	  * If \c prop is not a string property, the function fails and the error value is set to @ref IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	  * If \c buffer is not \c NULL, and \c *buffer_size is less than the length of the value of this property, the function fails and the error value is set to @ref IC4_ERROR_BUFFER_TOO_SMALL. \n
	  */
	IC4CORE_API bool ic4_prop_string_get_value(struct IC4_PROPERTY* prop, char* buffer, size_t* buffer_size);
	/**
	 * @brief Changes the value of a string property.
	 *
	 * @param[in] prop			A string property
	 * @param[in] buffer		Pointer to a buffer containing the new string value
	 * @param[in] buffer_size	Length of \c buffer.\n
	 *							If \c 0, interpret \c buffer as a null-terminated string.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not a string property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If the value is currently not writable, the function fails and the error value is set to #IC4_ERROR_GENICAM_ACCESS_DENIED. \n
	 */
	IC4CORE_API bool ic4_prop_string_set_value(struct IC4_PROPERTY* prop, const char* buffer, size_t buffer_size);
	/**
	 * @brief Returns the maximum length of the string that can be stored in this property.
	 * 
	 * @param[in] prop			A string property
	 * @param[out] pMaxLength	Pointer to an integer to receive the maximum length of the string value of this property
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not a string property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_string_get_max_len(struct IC4_PROPERTY* prop, uint64_t* pMaxLength);

	/**
	 * @}
	 */
	 
	/**
	 * @defgroup enumprop Enumeration Properties
	 * 
	 * @brief Enumeration properties represent a feature whose value is selected from a list of named entries.
	 * 
	 * Common examples for an enumeration properties are \c PixelFormat, \c TriggerMode or \c ExposureAuto.
	 * 
	 * The value of an enumeration property can be get or set by both a enumeration entry's name or value.
	 * 
	 * Enumeration entries are represented by #IC4_PROPERTY objects;
	 * a call to #ic4_prop_enum_get_entries() returns the list of possible enumeration entries as a #IC4_PROPERTY_LIST.
	 *
	 * @{
	 */

	/**
	 * @brief Returns the list of entries in this enumeration property.
	 * 
	 * @param[in] prop			An enumeration property
	 * @param[out] ppList		A pointer to a property list receiving the list of enumeration entries.\n
	 *							When the property list is no longer required, release the object reference using #ic4_proplist_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 */
	IC4CORE_API bool ic4_prop_enum_get_entries(struct IC4_PROPERTY* prop, struct IC4_PROPERTY_LIST** ppList);
	/**
	 * @brief Finds the enumeration entry with a given name in this enumeration property.
	 * 
	 * @param[in] prop			An enumeration property
	 * @param[in] entry_name	The name of one of this enumeration property's enumeration entries
	 * @param[out] ppEntry		A pointer to a property receiving the requested enumeration entry.\n
	 *							When the enumeration entry is no longer required, release the object reference using #ic4_prop_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_enum_find_entry_by_value
	 */
	IC4CORE_API bool ic4_prop_enum_find_entry_by_name(struct IC4_PROPERTY* prop, const char* entry_name, struct IC4_PROPERTY** ppEntry);
	/**
	 * @brief Finds the enumeration entry with a given value in this enumeration property.
	 *
	 * @param[in] prop			An enumeration property
	 * @param[in] entry_value	The value of one of this enumeration property's enumeration entries
	 * @param[out] ppEntry		A pointer to a property receiving the requested enumeration entry.\n
	 *							When the enumeration entry is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_enum_find_entry_by_name
	 */
	IC4CORE_API bool ic4_prop_enum_find_entry_by_value(struct IC4_PROPERTY* prop, int64_t entry_value, struct IC4_PROPERTY** ppEntry);
	/**
	 * @brief Sets the enumeration's selected entry by name.
	 * 
	 * @param[in] prop			An enumeration property
	 * @param[in] entry_name	The name of an enumeration entry of @c prop.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If @c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If @c entry_name is not the name of an entry of @c prop, the function fails and the error value is set to #IC4_ERROR_GENICAM_VALUE_ERROR. \n
	 * 
	 * @see ic4_prop_enum_set_selected_entry
	 * @see ic4_prop_enum_set_int_value
	 */
	IC4CORE_API bool ic4_prop_enum_set_value(struct IC4_PROPERTY* prop, const char* entry_name);
	/**
	 * @brief Returns the name of the currently selected entry of the enumeration.
	 * 
	 * @param prop				An enumeration property
	 * 
	 * @return	The name of the enumeration's currently selected entry, or \c NULL in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.\n
	 *			The memory pointed to by the return value is valid at least as the property object exists,
	 *			or until the next call to @c ic4_prop_enum_get_value on this enumeration.
	 * 
	 * @remarks
	 * If @c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH.
	 */
	IC4CORE_API const char* ic4_prop_enum_get_value(struct IC4_PROPERTY* prop);
	/**
	 * @brief Sets the enumeration's selected entry.
	 *
	 * @param[in] prop		An enumeration property
	 * @param[in] entry		An enumeration entry of @c prop.
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If @c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If @c entry is not an enumeration entry property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If @c entry is not an enumeration entry of @c prop, the function fails and the error value is set to #IC4_ERROR_GENICAM_VALUE_ERROR. \n
	 *
	 * @see ic4_prop_enum_set_value
	 * @see ic4_prop_enum_set_int_value
	 */
	IC4CORE_API bool ic4_prop_enum_set_selected_entry(struct IC4_PROPERTY* prop, struct IC4_PROPERTY* entry);
	/**
	 * @brief Returns the currently selected entry of this enumeration property.
	 *
	 * @param[in] prop			An enumeration property
	 * @param[out] ppEntry		A pointer to a property receiving the currently selected enumeration entry.\n
	 *							When the enumeration entry is no longer required, release the object reference using #ic4_prop_unref().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_enum_get_int_value
	 */
	IC4CORE_API bool ic4_prop_enum_get_selected_entry(struct IC4_PROPERTY* prop, struct IC4_PROPERTY** ppEntry);
	/**
	 * @brief Selects the currently selected entry of this enumeration property by specifying the entry's value.
	 * 
	 * This method can be useful to directly set a known enumeration entry, for example setting the \c PixelFormat to @ref IC4_PIXEL_FORMAT_Mono8.
	 * 
	 * @param[in] prop			An enumeration property
	 * @param[in] entry_value	The value of an enumeration entry of \c prop.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_enum_set_selected_entry
	 * @see ic4_prop_enum_get_int_value
	 */
	IC4CORE_API bool ic4_prop_enum_set_int_value(struct IC4_PROPERTY* prop, int64_t entry_value);
	/**
	 * @brief Returns the value of the currently selected entry of an enumeration property.
	 * 
	 * @param[in] prop			An enumeration property
	 * @param[out] pValue		A pointer to a double receiving the currently selected enumeration entry's value
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an enumeration property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 *
	 * @see ic4_prop_enum_get_selected_entry
	 * @see ic4_prop_enum_set_int_value
	 */
	IC4CORE_API bool ic4_prop_enum_get_int_value(struct IC4_PROPERTY* prop, int64_t* pValue);

	/**
	 * @brief Returns the value of an enumeration entry.
	 * 
	 * @param[in] prop			An enumeration entry
	 * @param[out] pValue		A pointer to a double receiving the value of the enumeration entry
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * If \c prop is not an enumeration entry, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @see ic4_prop_enum_get_entries
	 */
	IC4CORE_API bool ic4_prop_enumentry_get_int_value(struct IC4_PROPERTY* prop, int64_t* pValue);
	// #TODO ic4_prop_enumentry_get_numeric_value
	// #TODO ic4_prop_enumentry_is_self_clearing

	/**
	 * @}
	 */

	/**
	 * @defgroup regprop Register Properties
	 * 
	 * Register properties have a value represented by raw bytes.
	 *
	 * @{
	 */

	/**
	 * @brief Queries the size of a register property.
	 * 
	 * The size of a register property is not necessarily constant; it can change depending on the value of other properties.
	 * 
	 * @param[in] prop		A register property
	 * @param[out] pSize	Pointer to a @c uint64_t to receive the data size of the register in bytes.
	 * 
	 * @remarks
	 * If \c prop is not a register property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_prop_register_get_size(struct IC4_PROPERTY* prop, uint64_t* pSize);

	/**
	 * @brief Reads data from a register property.
	 * 
	 * @param[in]	prop		A register property
	 * @param[out]	buffer		A buffer receiving the data from the property
	 * @param[in]	buffer_size Size of @c buffer in bytes. Must be equal to the size of the register property.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If \c prop is not a register property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If @c buffer_size is not equal to the size of the property returned from #ic4_prop_register_get_size(), the function fails and the error value is set to #IC4_ERROR_INVALID_PARAM_VAL.\n
	 * 
	 * @see ic4_prop_register_get_size
	 * @see ic4_prop_register_set_value
	 */
	IC4CORE_API bool ic4_prop_register_get_value(struct IC4_PROPERTY* prop, void* buffer, size_t buffer_size);

	/**
	 * @brief Writes data to a register property.
	 * 
	 * @param[in]	prop		A register property
	 * @param[in]	buffer		A buffer containing the data to be written to the property
	 * @param[in]	buffer_size	Size of @c buffer in bytes. Must be equal to the size of the register property.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * If @c prop is not a register property, the function fails and the error value is set to #IC4_ERROR_GENICAM_TYPE_MISMATCH. \n
	 * If @c buffer_size is not equal to the size of the property returned from #ic4_prop_register_get_size(), the function fails and the error value is set to #IC4_ERROR_INVALID_PARAM_VAL.\n
	 * 
	 * @see ic4_prop_register_get_size
	 * @see ic4_prop_register_get_value
	 */
	IC4CORE_API bool ic4_prop_register_set_value(struct IC4_PROPERTY* prop, const void* buffer, size_t buffer_size);

	/**
	 * @}
	 */

	/**
	 * @}
	 */

	/**
	 * @addtogroup proplist
	 * 
	 * @{
	 */

	 /**
	  * @brief Increases the property list's internal reference count by one.
	  *
	  * @param[in] list	A property list
	  *
	  * @return The pointer passed via \c list
	  *
	  * @remarks If \c list is \c NULL, the function returns \c NULL. An error value is not set.
	  *
	  * @see ic4_proplist_unref
	  */
	IC4CORE_API struct IC4_PROPERTY_LIST* ic4_proplist_ref(struct IC4_PROPERTY_LIST* list);
	/**
	 * @brief Decreases the property list's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] list	A property list
	 *
	 * @remarks
	 * If \c list is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_proplist_ref
	 */
	IC4CORE_API void ic4_proplist_unref(struct IC4_PROPERTY_LIST* list);
	/**
	 * @brief Returns the number of properties in a property list.
	 * 
	 * @param[in] list	A property list
	 * @param[out] size	Pointer to a \c size_t to receive the number of properties in \c list.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_proplist_size(struct IC4_PROPERTY_LIST* list, size_t* size);
	/**
	 * @brief Returns a property from a property list.
	 * 
	 * @param[in] list			A property list
	 * @param[in] index			Index of the property to retrieve from \c list
	 * @param[out]	ppProperty	Pointer to a handle receiving the property object.\n
	 *							When the property is no longer required, release the object reference using #ic4_prop_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_proplist_at(struct IC4_PROPERTY_LIST* list, size_t index, struct IC4_PROPERTY** ppProperty);

	/**
	 * @}
	 */

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

#endif // IC4_C_PROPERTIES_H_INC_