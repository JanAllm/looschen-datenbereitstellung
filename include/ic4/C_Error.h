
#ifndef IC4_C_ERROR_H_INC_
#define IC4_C_ERROR_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * @defgroup error Error Handling
 *
 * @brief This section describes error handling in this library.
 *
 * Most functions of this library return a \c bool indicating success.
 * Some have a designated error return value, e.g. as part of an enumeration type.
 *
 * If a function fails, it sets a thread-local error value and error message. The error value and error message can
 * be retrieved by calling the #ic4_get_last_error() function.
 *
 * If a function succeeds, the error value and message are cleared.
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
	 * @brief Contains the possible error codes
	 */
	enum IC4_ERROR
	{
		IC4_ERROR_NOERROR = 0,							    ///< No error occurred, the operation was successful.
		IC4_ERROR_UNKNOWN = 1,							    ///< An unknown error occurred.
		IC4_ERROR_INTERNAL = 2,								///< An internal error (bug) occurred.
		IC4_ERROR_INVALID_OPERATION = 3,					///< The operation is not valid in the current state.
		IC4_ERROR_OUT_OF_MEMORY = 4,						///< Out of memory.
		IC4_ERROR_LIBRARY_NOT_INITIALIZED = 5,				///< InitLibrary has not been not called.
		IC4_ERROR_DRIVER_ERROR = 6,							///< Device driver behaved unexpectedly.
		IC4_ERROR_INVALID_PARAM_VAL = 7,					///< An invalid parameter was passed in.
		IC4_ERROR_CONVERSION_NOT_SUPPORTED = 8,				///< The operation would require an image format conversion that is not supported.
		IC4_ERROR_NO_DATA = 9,								///< The requested data is not available

		IC4_ERROR_GENICAM_FEATURE_NOT_FOUND = 101,			///< No matching GenICam feature found.
		IC4_ERROR_GENICAM_DEVICE_ERROR = 102,				///< Error occured writing to device.
		IC4_ERROR_GENICAM_TYPE_MISMATCH = 103,				///< Attempted an operation on the wrong node type, e.g. command_execute on an integer.
		IC4_ERROR_GENICAM_ACCESS_DENIED = 106,				///< Tried to access a camera feature that is currently not accessible.
		IC4_ERROR_GENICAM_NOT_IMPLEMENTED = 107,			///< Tried to access a feature that is not implemented by the current camera.
		IC4_ERROR_GENICAM_VALUE_ERROR = 108,				///< Tried to set an invalid value, e.g. out of range.
		IC4_ERROR_GENICAM_CHUNKDATA_NOT_CONNECTED = 109,	///< Tried to read a value that is only available if chunk data is connected to the property map.

		IC4_ERROR_BUFFER_TOO_SMALL = 50,					///< A supplied buffer was too small to receive all available data.
		IC4_ERROR_SINK_TYPE_MISMATCH = 52,					///< Tried to call a sink type-specific function on an instance of a different sink type.
		IC4_ERROR_SNAP_ABORTED = 53,						///< A snap operation was not completed, because the camera was stopped before all requested frames could be captured.

		IC4_ERROR_FILE_FAILED_TO_WRITE_DATA = 201,          ///< Failed to write data to a file.
		IC4_ERROR_FILE_ACCESS_DENIED = 202,					///< Failed to write to a file, because the location was not writable.
		IC4_ERROR_FILE_PATH_NOT_FOUND = 203,				///< Failed to write to a file, because the path was invalid.
		IC4_ERROR_FILE_FAILED_TO_READ_DATA = 204,           ///< Failed to read data from a file.

		IC4_ERROR_DEVICE_INVALID = 13,					    ///< The device has become invalid (e. g. it was unplugged).
		IC4_ERROR_DEVICE_NOT_FOUND = 16,					///< The device was not found.
		IC4_ERROR_DEVICE_ERROR = 17,						///< The device behaved unexpectedly.
		IC4_ERROR_AMBIGUOUS = 18,							///< The parameter did not uniquely identify an item.

		IC4_ERROR_PARSE_ERROR = 21,							///< There was an error parsing the parameter or file.
		IC4_ERROR_TIMEOUT = 27,								///< The requested operation could not be completed before the timeout expired.

		IC4_ERROR_INCOMPLETE = 34,							///< The operation was only partially successful, e.g. not all properties of the grabber could be restored.

		IC4_ERROR_SINK_NOT_CONNECTED = 38,                  ///< Sink is not yet connected.
		IC4_ERROR_IMAGETYPE_MISMATCH = 39,                  ///< The passed buffer does not have the expected ImageType.
		IC4_ERROR_SINK_ALREADY_ATTACHED = 40,               ///< The sink passed in is already attached to another graph.
		IC4_ERROR_SINK_CONNECT_ABORTED = 41,				///< The sink's connect handler signaled an error.

		IC4_ERROR_HANDLER_ALREADY_REGISTERED = 60,			///< Attempted to register the same notification handler twice.
		IC4_ERROR_HANDLER_NOT_FOUND = 61,					///< Attempted to use a non-existing notification handler.
	};

	/**
	 * @brief Query information about the error of the previous library function call
	 *
	 * @param[out] pError Pointer to a #IC4_ERROR value to receive the error code.
	 * @param[out] message Pointer to a character array to receive an error message.<br>
	 * This parameter is optional and may be \c NULL.
	 * @param[in,out] message_length Pointer to a \c size_t describing the length of the array pointed to by \a message.<br>
	 * If \a message is not \c NULL, this parameter is required.<br>
	 * The function always writes the actual number of characters required to store the error message.
	 *
	 * @return \c true on success.
	 * @return If \a pError is \c NULL, the function fails and returns \c false.
	 * @return If \a message is not \c NULL and \a message_length is \c NULL, the function fails and returns \c false.
	 * @return If \a *message_length is lower than the number of characters required to store the error message, the function fails and returns \c false.
	 *
	 * @note
	 * The last error information is stored in a thread-local way. A call to \c ic4_get_last_error
	 * returns error information about the previous function call that happened on the same thread
	 * that \c ic4_get_last_error is called from.
	 *
	 * @note
	 * An error while calling \c ic4_get_last_error does not update the internally stored last error.
	 */
	IC4CORE_API bool ic4_get_last_error(enum IC4_ERROR* pError, char* message, size_t* message_length);

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

#endif //IC4_C_ERROR_H_INC_
