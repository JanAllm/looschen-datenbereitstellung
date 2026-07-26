
#ifndef IC4_ERROR_H_INC_
#define IC4_ERROR_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Error.h"

#include <string>

namespace ic4
{
	/**
	 * @enum ErrorHandlerBehavior
	 * @brief Specifies the default error handler behavior
	 */
	enum class ErrorHandlerBehavior
	{
		Ignore,	///< Ignores errors passed to #ic4::Error::Default()
		Throw,	///< Throws errors passed to #ic4::Error::Default() as #ic4::IC4Exception
	};

	/**
	 * @brief Contains the possible error codes
	 */
	enum class ErrorCode
	{
		NoError = c_interface::IC4_ERROR_NOERROR,												///< No error occurred, the operation was successful.
		Unknown = c_interface::IC4_ERROR_UNKNOWN,												///< An unknown error occurred.
		Internal = c_interface::IC4_ERROR_INTERNAL,												///< An internal error (bug) occurred.
		InvalidOperation = c_interface::IC4_ERROR_INVALID_OPERATION,							///< The operation is not valid in the current state.
		OutOfMemory = c_interface::IC4_ERROR_OUT_OF_MEMORY,										///< Out of memory.
		LibraryNotInitialized = c_interface::IC4_ERROR_LIBRARY_NOT_INITIALIZED,					///< initLibrary has not been not called.
		DriverError = c_interface::IC4_ERROR_DRIVER_ERROR,										///< Device driver behaved unexpectedly.
		InvalidParameter = c_interface::IC4_ERROR_INVALID_PARAM_VAL,							///< An invalid parameter was passed in.
		ConversionNotSupported = c_interface::IC4_ERROR_CONVERSION_NOT_SUPPORTED,				///< The operation would require an image format conversion that is not supported.
		NoData = c_interface::IC4_ERROR_NO_DATA,												///< The requested data is not available

		GenICamFeatureNotFound = c_interface::IC4_ERROR_GENICAM_FEATURE_NOT_FOUND,				///< No matching GenICam feature found.
		GenICamDeviceError = c_interface::IC4_ERROR_GENICAM_DEVICE_ERROR,						///< Error occured writing to device.
		GenICamTypeMismatch = c_interface::IC4_ERROR_GENICAM_TYPE_MISMATCH,						///< Attempted an operation on the wrong node type, e.g. command_execute on an integer.
		GenICamAccessDenied = c_interface::IC4_ERROR_GENICAM_ACCESS_DENIED,						///< Tried to access a camera feature that is currently not accessible.
		GenICamNotImplemented = c_interface::IC4_ERROR_GENICAM_NOT_IMPLEMENTED,					///< Tried to access a feature that is not implemented by the current camera.
		GenICamValueError = c_interface::IC4_ERROR_GENICAM_VALUE_ERROR,							///< Tried to set an invalid value, e.g. out of range.
		GenICamChunkdataNotConnected = c_interface::IC4_ERROR_GENICAM_CHUNKDATA_NOT_CONNECTED,	///< Tried to read a value that is only available if chunk data is connected to the property map.

		BufferTooSmall = c_interface::IC4_ERROR_BUFFER_TOO_SMALL,								///< A supplied buffer was too small to receive all available data.
		SinkTypeMismatch = c_interface::IC4_ERROR_SINK_TYPE_MISMATCH,							///< Tried to call a sink type-specific function on an instance of a different sink type.
		SnapAborted  = c_interface::IC4_ERROR_SNAP_ABORTED,										///< A snap operation was not completed, because the camera was stopped before all requested frames could be captured.

		FileWriteError = c_interface::IC4_ERROR_FILE_FAILED_TO_WRITE_DATA,						///< Failed to write data to a file.
		FileAccessDenied = c_interface::IC4_ERROR_FILE_ACCESS_DENIED,							///< Failed to write to a file, because the location was not writable.
		FilePathNotFound = c_interface::IC4_ERROR_FILE_PATH_NOT_FOUND,							///< Failed to write to a file, because the path was invalid.
		FileReadError = c_interface::IC4_ERROR_FILE_FAILED_TO_READ_DATA,						///< Failed to read data from a file.
		
		DeviceInvalid = c_interface::IC4_ERROR_DEVICE_INVALID,									///< The device has become invalid (e. g. it was unplugged).
		DeviceNotFound = c_interface::IC4_ERROR_DEVICE_NOT_FOUND,								///< The device was not found.
		DeviceError = c_interface::IC4_ERROR_DEVICE_ERROR,										///< The device behaved unexpectedly.
		Ambiguous = c_interface::IC4_ERROR_AMBIGUOUS,											///< The parameter did not uniquely identify an item.

		ParseError = c_interface::IC4_ERROR_PARSE_ERROR,										///< There was an error parsing the parameter or file.
		Timeout = c_interface::IC4_ERROR_TIMEOUT,												///< The requested operation could not be completed before the timeout expired.

		Incomplete = c_interface::IC4_ERROR_INCOMPLETE,											///< The operation was only partially successful, e.g. not all properties of the grabber could be restored.

		SinkNotConnected = c_interface::IC4_ERROR_SINK_NOT_CONNECTED,							///< Sink is not yet connected.
		ImageTypeMismatch = c_interface::IC4_ERROR_IMAGETYPE_MISMATCH,							///< The passed buffer does not have the expected ImageType.
		SinkAlreadyAttached = c_interface::IC4_ERROR_SINK_ALREADY_ATTACHED,						///< The sink passed in is already attached to another graph.
		SinkConnectAborted = c_interface::IC4_ERROR_SINK_CONNECT_ABORTED,						///< The sink's connect handler signaled an error.

		HandlerAlreadyRegistered = c_interface::IC4_ERROR_HANDLER_ALREADY_REGISTERED,			///< Attempted to register the same notification handler twice.
		HandlerNotFound = c_interface::IC4_ERROR_HANDLER_NOT_FOUND,								///< Attempted to use a non-existing notification handler.
	};

	class Error;

	namespace detail
	{
		bool returnUpdateFromLastError(Error& err, bool status_success);
		template<typename T> T updateFromLastErrorReturn(Error& err, T&& val);
		template<typename T> T updateReturn(Error& err, ErrorCode code, std::string&& msg, T&& val);
		void clear(Error& err);
		void force_clear(Error& err);
		void configureDefaultErrorHandlerBehavior(ErrorHandlerBehavior behavior);
		void throwError(const Error& err);
	}

	/**
	 * @brief This class to capture error information from library function calls.
	 * 
	 * See @ref technical_article_error_handling for a general overview of how error handling works in this library.
	 */
	class Error
	{
		ErrorCode code_ = ErrorCode::NoError;
		std::string message_;

#ifndef IC4_DOXYGEN_INVISIBLE
		friend bool detail::returnUpdateFromLastError(Error& err, bool status_success);
		template<typename T> friend T detail::updateFromLastErrorReturn(Error& err, T&& val);
		template<typename T> friend T detail::updateReturn(Error& err, ErrorCode code, std::string&& msg, T&& val);
		friend void detail::clear(Error& err);
		friend void detail::force_clear(Error& err);
		friend void detail::configureDefaultErrorHandlerBehavior(ErrorHandlerBehavior behavior);		
#endif

	public:
		/**
		 * @brief An error handler that ignores all errors.
		 *
		 * If an error occurs during a function call its @c argument set to @ref Ignore(), errors are ignored.
		 *
		 * The return value of the function can still signal a possible error condition.
		 * 
		 * @return An error handler that ignores all errors.
		 * 
		 * @see @ref technical_article_error_handling
		 */
		static Error& Ignore();
		/**
		 * @brief An error handler that throws all errors.
		 *
		 * If an error occurs during a function call with its @c err argument set to @ref Throw(), @ref IC4Exception is thrown.
		 * 
		 * @return An error handler that throws all errors as a @ref IC4Exception.
		 * 
		 * @see @ref technical_article_error_handling
		 */
		static Error& Throw();
		/**
		 * @brief The default error handler.
		 *
		 * The @c err parameter of all library functions is set to @ref Default() by default.
		 * 
		 * If a function that got @ref Default() passed as its @c err parameter, the behavior is as follows:
		 * - If the library was initialized by passing @ref ErrorHandlerBehavior::Throw to @ref initLibrary, a @ref IC4Exception is thrown.
		 * - If the library was initialized with @ref ErrorHandlerBehavior::Ignore, the error is ignored
		 * 
		 * @return An error handler following the default procedure described above.
		 * 
		 * @see @ref technical_article_error_handling
		 */
		static Error& Default();

	protected:
#ifndef IC4_DOXYGEN_INVISIBLE
		enum class Behavior
		{
			Ignore,
			Capture,
			Default,
			Throw
		};

		Behavior behavior_ = Behavior::Capture;
		Error(Behavior behavior) noexcept : behavior_(behavior) {}
#endif

	public:
		Error() = default;

		/**
		 * @brief Checks whether this object is in an error state.
		 * 
		 * @return @c true, if this error's code is not equal to @ref ErrorCode::NoError, otherwise @c false.
		 * 
		 * @see isError
		 * @see isSuccess
		 */
		explicit	operator bool() const noexcept { return isError(); }
		/**
		 * @brief	Checks whether this object is in an error state.
		 * @return	@c true, if this error's code is not equal to @ref ErrorCode::NoError, otherwise @c false.
		 * 
		 * @see isSuccess
		 */
		bool		isError() const noexcept { return code_ != ErrorCode::NoError; }
		/**
		 * @brief	Checks whether this object is in a success state.
		 * @return	@c true, if this error's code is equal to @ref ErrorCode::NoError, otherwise @c false.
		 *
		 * @see isError
		 */
		bool		isSuccess() const noexcept { return !isError(); }
		/**
		 * @brief	Returns the error code
		 * 
		 * @return	The error code of the operation that filled this error object.
		 */
		ErrorCode	code() const noexcept { return code_; }
		/**
		 * @brief	Returns the error message
		 * 
		 * @return	The error message generated by the failed operation that filled this error object.\n
		 *			If no error occurred, an empty string is returned.
		 */
		std::string message() const
		{
			if (code_ == ErrorCode::NoError)
				return {};

			return message_;
		}
		
	private:
		void clear()
		{
			code_ = ErrorCode::NoError;
			message_.clear();
		}

		void update(ErrorCode code, std::string&& msg)
		{
			code_ = code;
			message_ = std::move(msg);
		}

		static ErrorCode queryLastErrorCode()
		{
			c_interface::IC4_ERROR code = c_interface::IC4_ERROR_NOERROR;

			ic4_get_last_error(&code, nullptr, nullptr);

			return ErrorCode(code);
		}

		void updateFromLastError()
		{
			c_interface::IC4_ERROR code;

			char buffer[256];
			size_t len = sizeof(buffer);

			if (ic4_get_last_error(&code, buffer, &len))
			{
				if (len > 0)
					len -= 1;

				update(ErrorCode(code), std::string(buffer, len));
			}
			else
			{
				std::string sbuffer;
				sbuffer.resize(len);

				ic4_get_last_error(&code, &sbuffer[0], &len);
				update(ErrorCode(code), std::move(sbuffer));
			}
		}

		bool should_ignore(ErrorCode ec) const noexcept
		{
			if (behavior_ == Behavior::Default)
			{
				if (ec == ErrorCode::LibraryNotInitialized)
				{
					return false;
				}

				return default_behavior() == ErrorHandlerBehavior::Ignore;
			}

			return (behavior_ == Behavior::Ignore);
		}
		bool should_throw(ErrorCode ec) const noexcept
		{
			if (behavior_ == Behavior::Default)
			{
				if (ec == ErrorCode::LibraryNotInitialized)
				{
					return true;
				}

				return default_behavior() == ErrorHandlerBehavior::Throw;
			}

			return (behavior_ == Behavior::Throw);
		}

		static ErrorHandlerBehavior& default_behavior() noexcept
		{
			static ErrorHandlerBehavior v = ErrorHandlerBehavior::Ignore;
			return v;
		}
	};

	/**
	 * @brief Exception thrown if an error occurs and exceptions are enabled for the failing function call.
	 * 
	 * This exception is thrown, if a function call fails, and
	 * - @ref Error::Throw() is passed to the function's @c err parameter.
	 * - @ref initLibrary was called with @c defaultErrorHandlerBehavior set to @ref ErrorHandlerBehavior::Throw.
	 * 
	 * See @ref technical_article_error_handling for a general overview of how error handling works in this library.
	 */
	class IC4Exception : public std::exception
	{
		Error err_;
		mutable std::string message_;

#ifndef IC4_DOXYGEN_INVISIBLE
		friend void detail::throwError(const Error& err);

	protected:
		IC4Exception(const Error& err)
			: err_(err)
		{
		}
#endif

	public:
		/**
		 * @brief Returns the error code of the error that caused this exception
		 * 
		 * @return The error code
		*/
		ErrorCode code() const noexcept
		{
			return err_.code();
		}

		/**
		 * @brief Returns the error message of the error that caused this exception
		 * 
		 * @return The error message
		*/
		const char* what() const noexcept final
		{
			message_ = err_.message();
			return message_.c_str();
		}
	};

#ifndef IC4_DOXYGEN_INVISIBLE
	class IC4LibraryNotInitializedException : public IC4Exception
	{
	public:
		IC4LibraryNotInitializedException(const Error& err)
			: IC4Exception(err)
		{
		}
	};
#endif

	namespace detail
	{
		inline void throwError(const Error& err)
		{
#ifndef NDEBUG
			const ErrorCode code = err.code();
			const std::string message = err.message();

			// Oh no, an exception occurred!
			// Don't panic; if you are in a debugger, you can easily inspect these values to figure out what is going on:

			// The error code
			(void)code;
			// The error message
			(void)message;
#endif

			if (err.code() == ErrorCode::LibraryNotInitialized)
			{
				throw IC4LibraryNotInitializedException(err);
			}

			throw IC4Exception(err);
		}

		inline bool returnUpdateFromLastError(Error& err, bool status_success)
		{
			if (!status_success)
			{
				if (Error::queryLastErrorCode() == ErrorCode::LibraryNotInitialized)
				{
					err.updateFromLastError();
					throwError(err);
				}
			}

			if( !err.should_ignore(err.code()) )
			{
				if( status_success )
				{
					err.clear();
				}
				else
				{
					err.updateFromLastError();
				}

				if (err.should_throw(err.code()) && err.isError())
				{
					throwError(err);
				}
			}
			return status_success;
		}

		template<typename T>
		T updateReturn(Error& err, ErrorCode code, std::string&& msg, T&& val)
		{
			if (!err.should_ignore(code))
			{
				err.update(code, std::move(msg));

				if (err.should_throw(code) && err.isError())
				{
					throwError(err);
				}
			}
			return std::forward<T>(val);
		}

		inline void clear(Error& err)
		{
			if (!err.should_ignore(err.code()))
				err.clear();
		}

		inline void force_clear(Error& err)
		{
			err.clear();
		}

		template<typename T>
		T clearReturn(Error& err, T&& val)
		{
			clear(err);
			return std::forward<T>( val );
		}

		template<typename T>
		T updateFromLastErrorReturn(Error& err, T&& val)
		{
			if (!err.should_ignore(err.code()))
			{
				err.updateFromLastError();

				if (err.should_throw(err.code()) && err.isError())
				{
					throwError(err);
				}
			}
			return std::forward<T>(val);
		}

		inline void configureDefaultErrorHandlerBehavior(ErrorHandlerBehavior behavior)
		{
			Error::default_behavior() = behavior;
		}
	}

#ifndef IC4_DOXYGEN_INVISIBLE
	namespace detail
	{
		struct ErrorThrow : Error
		{
			ErrorThrow() : Error(Behavior::Throw) {}
		};
		struct ErrorIgnore : Error
		{
			ErrorIgnore() : Error(Behavior::Ignore) {}
		};
		struct ErrorDefault : Error
		{
			ErrorDefault() : Error(Behavior::Default) {}
		};

		static ErrorThrow staticThrowError;
		static ErrorIgnore staticIgnoreError;
		static ErrorDefault staticDefaultError;
	}
#endif

	inline Error& Error::Throw()
	{
		static detail::ErrorThrow* throwErrorPtr = &detail::staticThrowError;
		return *throwErrorPtr;
	}
	inline Error& Error::Ignore()
	{
		static detail::ErrorIgnore* ignoreErrorPtr = &detail::staticIgnoreError;
		return *ignoreErrorPtr;
	}
	inline Error& Error::Default()
	{
		static detail::ErrorDefault* defaultErrorPtr = &detail::staticDefaultError;
		return *defaultErrorPtr;
	}
}

#endif // IC4_GRABBER_H_INC_