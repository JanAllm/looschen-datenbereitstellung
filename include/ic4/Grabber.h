
#ifndef IC4_GRABBER_H_INC_
#define IC4_GRABBER_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Grabber.h"

#include "Sink.h"
#include "DeviceEnum.h"
#include "Properties.h"
#include "Display.h"
#include "Error.h"
#include "QueueSink.h"
#include "SnapSink.h"

#include <memory>
#include <string>
#include <vector>

namespace ic4
{
	/**
	 * @brief Window handle representing parent window for displays or dialogs
	 */
	using WindowHandle = ic4::c_interface::IC4_WINDOW_HANDLE;

	class Grabber;

	namespace detail
	{
		c_interface::IC4_GRABBER* grabber_ptr(const Grabber& grabber) noexcept;
		Grabber grabber_wrap(c_interface::IC4_GRABBER* handle) noexcept;
	}

	/**
	 * @brief Specifies whether @ref ic4::Grabber::streamSetup() should enable acquisition after the stream was set up successfully.
	 */
	enum class StreamSetupOption
	{
		/**
		 * @brief Start acquisition after the stream was set up.
		 * 
		 * This is equivalent to manually calling @ref Grabber::acquisitionStart().
		 */
		AcquisitionStart,
		/**
		 * @brief Do not start acquisition.
		 * 
		 * To acquire images, @ref Grabber::acquisitionStart() must be called at a later time.
		 */
		DeferAcquisitionStart,
	};

	/**
	 * @brief Represents an opened video capture device, allowing device configuration and stream setup.
	 *
	 * The grabber object is the core component used when working with video capture devices.
	 * 
	 * Grabber objects are created using the constructor @ref Grabber::Grabber(Error&).
	 * 
	 * After creation, the most common operation on a grabber is to open a device. Call @ref Grabber::deviceOpen() or one of its siblings.
	 * A device can also be selected and configured by calling @ref Grabber::deviceOpenFromState().
	 * 
	 * To establish a data stream from the opened video capture device, call @ref Grabber::streamSetup(), specifying a sink and/or a display.
	 * 
	 * A @ref Sink is required if the program needs to access, process, or store image data.
	 * There are several sink types available to choose from, which are useful depending on the application, e.g. @ref QueueSink or @ref SnapSink.
	 * 
	 * A @ref Display can be used to automatically display all images received from a video capture device.
	 * 
	 * After the data stream has been set up, call @ref Grabber::acquisitionStart() to begin the transfer of images.
	 *
	 * Graber objects are movable.
	 * 
	 * @note
	 * Some object references, e.g. @ref ImageBuffer, can keep the device and/or driver opened as long as they exist,
	 * since they point into device driver memory. To free all device-related resources, all objects references have to be released.
	 */
	class Grabber
	{
	private:
		detail::MovableHandleRef<c_interface::IC4_GRABBER, c_interface::ic4_grabber_unref> ptr_;

		std::weak_ptr<Sink> current_sink;
		std::weak_ptr<Display> current_display;

#ifndef IC4_DOXYGEN_INVISIBLE
		friend c_interface::IC4_GRABBER* detail::grabber_ptr(const Grabber& grabber) noexcept;
		friend Grabber detail::grabber_wrap(c_interface::IC4_GRABBER* handle) noexcept;
#endif

		Grabber(c_interface::IC4_GRABBER* handle) noexcept
			: ptr_(c_interface::ic4_grabber_ref(handle))
		{
			assert( handle );
		}
	public:
		/**
		 * @brief	Creates a new grabber.
		 *
		 * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @note If @c err is not configured to throw, and the constructor fails,
		 * the grabber object is in an invalid state. All member function calls will fail.
		 *
		 * @see Grabber::is_valid
		 */
		Grabber(Error& err = Error::Default())
		{
			if (!c_interface::ic4_grabber_create(&ptr_ ))
			{
				detail::returnUpdateFromLastError(err, false);
			}
			else
			{
				detail::clear(err);
			}
		}

		/**
		 * @brief	Creates a new grabber and opens the specified video capture device.
		 *
		 * @param[in] dev	A device information object representing the video capture device to be opened
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @note If @c err is not configured to throw, and the grabber object could not be created,
		 * the object will be in an invalid state. All member function calls will fail.
		 *
		 * @see Grabber::is_valid
		 */
		explicit Grabber(const DeviceInfo& dev, Error& err = Error::Default())
			: Grabber(err)
		{
			if (!err)
			{
				deviceOpen(dev, err);
			}
		}
		/**
		 * @brief	Creates a new grabber and opens the specified video capture device.
		 *
		 * @param[in] identifier	The model name, unique name or serial of a connected video capture device
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @remarks
		 * If there are multiple devices matching the specified identifier, the function will fail and the error value is set to @ref ErrorCode::Ambiguous. \n
		 * If there is no device with the specified identifier, the function will fail and the error value is set to @ref ErrorCode::DeviceNotFound. \n
		 *
		 * @note If @c err is not configured to throw, and the grabber object could not be created,
		 * the object will be in an invalid state. All member function calls will fail.
		 * 
		 * @see Grabber::is_valid
		 */
		explicit Grabber(const char* identifier, Error& err = Error::Default())
			: Grabber(err)
		{
			if (!err)
			{
				deviceOpen(identifier, err);
			}
		}
		/**
		 * @brief	Creates a new grabber and opens the specified video capture device.
		 *
		 * @param[in] identifier	The model name, unique name or serial of a connected video capture device
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @remarks
		 * If there are multiple devices matching the specified identifier, the function will fail and the error value is set to @ref ErrorCode::Ambiguous. \n
		 * If there is no device with the specified identifier, the function will fail and the error value is set to @ref ErrorCode::DeviceNotFound. \n
		 * 
		 * @note If @c err is not configured to throw, and the grabber object could not be created,
		 * the object will be in an invalid state. All member function calls will fail.
		 *
		 * @see Grabber::is_valid
		 */
		explicit Grabber(const std::string& identifier, Error& err = Error::Default())
			: Grabber(err)
		{
			if (!err)
			{
				deviceOpen(identifier, err);
			}
		}

	public:
		/**
		 * @brief	Checks whether this grabber is a valid object.
		 *
		 * If there is an error in the constructor, and function was not configured to throw on error,
		 * an invalid object is created. All member function calls will fail.
		 *
		 * @return	@c true, if this grabber was constructed successfully, otherwise @c false.\n
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
		 * @brief Opens the video capture device specified by the passed device information object.
		 * 
		 * @param[in] dev		A device information object representing the video capture device to be opened
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		*/
		bool                deviceOpen(const DeviceInfo& dev, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open( ptr_, dev.ptr_));
		}

		/**
		 * @brief Opens the video capture matching the specified identifier.
		 * 
		 * @param[in] identifier	The model name, unique name, serial, user id, IPV4 address or MAC address of a connected video capture device
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @remarks
		 * If the grabber already has a device open, the function will fail and the error value is set to @ref ErrorCode::InvalidOperation. \n
		 * If there are multiple devices matching the specified identifier, the function will fail and the error value is set to @ref ErrorCode::Ambiguous. \n
		 * If there is no device with the specified identifier, the function will fail and the error value is set to @ref ErrorCode::DeviceNotFound. \n
		 */
		bool                deviceOpen(const char* identifier, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open_by_identifier( ptr_, identifier));
		}
		/**
		 * @brief Opens the video capture matching the specified identifier.
		 *
		 * @param[in] identifier	The model name, unique name or serial of a connected video capture device
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @remarks
		 * If the grabber already has a device open, the function will fail and the error value is set to @ref ErrorCode::InvalidOperation. \n
		 * If there are multiple devices matching the specified model name, unique name or serial, the function will fail and the error value is set to @ref ErrorCode::Ambiguous. \n
		 * If there is no device with the specified model name, unique name or serial, the function will fail and the error value is set to @ref ErrorCode::DeviceNotFound. \n
		 */
		bool                deviceOpen(const std::string& identifier, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open_by_identifier( ptr_, identifier.c_str()));
		}

		/**
		 * @brief Restores the opened device and its settings from a memory buffer containing data that was previously written by @ref Grabber::deviceSaveState(Error&).
		 * 
		 * @param[in] device_state	A buffer containing data that was written by @ref Grabber::deviceSaveState(Error&).
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @note
		 * If the memory buffer contains settings for properties that could not be written, the function fails and the error value is set to @ref ErrorCode::Incomplete.
		 * 
		 * @see Grabber::deviceSaveState(std::vector<uint8_t>&, Error&)
		 */
		bool				deviceOpenFromState(const std::vector<uint8_t>& device_state, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open_from_state(ptr_, device_state.data(), device_state.size()));
		}

		/**
		 * @brief Restores the opened device and its settings from a file that was previously written by @ref Grabber::deviceSaveState(const char*, Error&).
		 *
		 * @param[in] file_path		Path to a file containing device state information
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @note
		 * If the memory buffer contains settings for properties that could not be written, the function fails and the error value is set to @ref ErrorCode::Incomplete.
		 * 
		 * @see Grabber::deviceSaveState(const char*, Error&)
		 */
		bool				deviceOpenFromState(const char* file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open_from_state_file(ptr_, file_path));
		}
		/**
		 * @brief Restores the opened device and its settings from a file that was previously written by @ref Grabber::deviceSaveState(const std::string&, Error&).
		 *
		 * @param[in] file_path		Path to a file containing device state information
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @note
		 * If the memory buffer contains settings for properties that could not be written, the function fails and the error value is set to @ref ErrorCode::Incomplete.
		 * 
		 * @see Grabber::deviceSaveState(const std::string&, Error&)
		 */
		bool				deviceOpenFromState(const std::string& file_path, Error& err = Error::Default())
		{
			return deviceOpenFromState(file_path.c_str(), err);
		}
#ifdef _WIN32
		bool				deviceOpenFromState(const wchar_t* file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_open_from_state_fileW(ptr_, file_path));
		}
		bool				deviceOpenFromState(const std::wstring& file_path, Error& err = Error::Default())
		{
			return deviceOpenFromState(file_path.c_str(), err);
		}
#endif

		/**
		 * @brief Establishes the data stream from the device.
		 * 
		 * A data stream is required for image acquisition from the video capture device, and must include a @ref Sink, a @ref Display, or both.
		 * 
		 * @param[in] sink		A sink to receive the images
		 * @param[in] display	A display to display images
		 * @param[in] action	Specifies whether to immediately start acquisition after the data stream was set up successfully
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @pre
		 * A device was previously opened using @ref Grabber::deviceOpen() or one of its sibling functions.
		 * 
		 * @note
		 * The grabber takes references to the passed sink and display, tying their lifetime to the grabber until the data stream is stopped.
		 * 
		 * @see Grabber::streamStop()
		 * @see Grabber::acquisitionStart()
		 */
		bool				streamSetup(const std::shared_ptr<Sink>& sink, const std::shared_ptr<Display>& display, StreamSetupOption action = StreamSetupOption::AcquisitionStart, Error& err = Error::Default())
		{
			auto sink_ptr = sink ? (c_interface::IC4_SINK*)sink->ptr_ : nullptr;
			auto display_ptr = display ? (c_interface::IC4_DISPLAY*)display->ptr_ : nullptr;
			bool do_start_acquisition = (action == StreamSetupOption::AcquisitionStart);

			if (c_interface::ic4_grabber_stream_setup( ptr_, sink_ptr, display_ptr, do_start_acquisition))
			{
				current_sink = sink;
				current_display = display;
				return detail::clearReturn(err, true);
			}
			else
			{
				return detail::returnUpdateFromLastError(err, false);
			}
		}

		/**
		 * @brief Establishes the data stream from the device.
		 *
		 * A data stream is required for image acquisition from the video capture device, and must include a @ref Sink, a @ref Display, or both.
		 *
		 * @param[in] sink		A sink to receive the images
		 * @param[in] action	Specifies whether to immediately start acquisition after the data stream was set up successfully
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @pre
		 * A device was previously opened using @ref Grabber::deviceOpen() or one of its sibling functions.
		 *
		 * @note
		 * The grabber takes references to the passed sink, tying its lifetime to the grabber until the data stream is stopped.
		 *
		 * @see Grabber::streamStop()
		 * @see Grabber::acquisitionStart()
		 */
		bool				streamSetup(const std::shared_ptr<Sink>& sink, StreamSetupOption action = StreamSetupOption::AcquisitionStart, Error& err = Error::Default())
		{
			return streamSetup(sink, nullptr, action, err);
		}

		/**
		 * @brief Establishes the data stream from the device.
		 *
		 * A data stream is required for image acquisition from the video capture device, and must include a @ref Sink, a @ref Display, or both.
		 *
		 * @param[in] display	A display to display images
		 * @param[in] action	Specifies whether to immediately start acquisition after the data stream was set up successfully
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @pre
		 * A device was previously opened using @ref Grabber::deviceOpen() or one of its sibling functions.
		 *
		 * @note
		 * The grabber takes references to the passed display, tying its lifetime to the grabber until the data stream is stopped.
		 *
		 * @see Grabber::streamStop()
		 * @see Grabber::acquisitionStart()
		 */
		bool				streamSetup(const std::shared_ptr<Display>& display, StreamSetupOption action = StreamSetupOption::AcquisitionStart, Error& err = Error::Default())
		{
			return streamSetup(nullptr, display, action, err);
		}

		/**
		 * @brief Returns a @c shared_ptr to the sink object that was passed to @ref Grabber::streamSetup()
		 * when setting up the currently established data stream.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	@c shared_ptr to the sink object, or @c nullptr if an error occurred.\n
		 *			Check the @c err output parameter for details.
		 */
		std::shared_ptr<Sink>	sink(Error& err = Error::Default()) const
		{
			auto sink_ptr = current_sink.lock();
			if (sink_ptr != nullptr)
			{
				return detail::clearReturn(err, sink_ptr);
			}

			c_interface::IC4_SINK* pSink = nullptr;
			if (!c_interface::ic4_grabber_get_sink( ptr_, &pSink))
			{
				return detail::updateFromLastErrorReturn(err, nullptr);
			}			

			switch (c_interface::ic4_sink_get_type(pSink))
			{
			case ic4::c_interface::IC4_SINK_TYPE_QUEUESINK:
				return detail::clearReturn(err, std::shared_ptr<Sink>(new QueueSink(pSink)));
			case ic4::c_interface::IC4_SINK_TYPE_SNAPSINK:
				return detail::clearReturn(err, std::shared_ptr<Sink>(new SnapSink(pSink)));
			default:
				return detail::updateReturn(err, ErrorCode::Internal, "Unexpected sink type", nullptr);
			}
		}

		/**
		 * @brief Returns a @c shared_ptr to the display object that was passed to @ref Grabber::streamSetup()
		 * when setting up the currently established data stream.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c shared_ptr to the display object, or @c nullptr if an error occurred.\n
		 *			Check the @c err output parameter for details.
		 */
		std::shared_ptr<Display>	display(Error& err = Error::Default()) const
		{
			auto display_ptr = current_display.lock();
			if (display_ptr != nullptr)
			{
				return detail::clearReturn(err, display_ptr);
			}

			c_interface::IC4_DISPLAY* pDisplay = nullptr;
			if (!c_interface::ic4_grabber_get_display( ptr_, &pDisplay))
			{
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, std::shared_ptr<Display>(new Display(pDisplay)));
		}

		/**
		 * @brief Checks whethere there is a data stream established from this grabber's video capture device.
		 * 
		 * @return	\c true, if a data stream was previously established by calling Grabber::streamSetup().\n
		 *			Otherwise, or if the data stream was stopped again, \c false.
		*/
		bool			isStreaming() const
		{
			return c_interface::ic4_grabber_is_streaming( ptr_ );
		}

		/**
		 * @brief Starts the acquisition of images from the video capture device.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @pre A data stream has was previously established using @ref Grabber::streamSetup().
		 * 
		 * @note
		 * This operation is equivalent to executing the @c AcquisitionStart command on the video capture device's property map.
		 * 
		 * @see Grabber::acquisitionStop()
		 * @see Grabber::streamSetup()
		 */
		bool            acquisitionStart(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_acquisition_start(ptr_));
		}
		/**
		 * @brief Stops the acquisition of images from the video capture device.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @pre
		 * The image acquisition was previously started.
		 * 
		 * @note
		 * This operation is equivalent to executing the \c AcquisitionStop command on the video capture device's property map.
		 * 
		 * @see Grabber::acquisitionStart()
		 * @see Grabber::isAcquisitionActive()
		 */
		bool			acquisitionStop(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_acquisition_stop( ptr_ ));
		}

		/**
		 * @brief Stops a data stream that was previously set up by a call to @ref Grabber::streamSetup().
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @note
		 * This function releases the sink and/or display references that were passed to @ref Grabber::streamSetup().\n
		 * If there are no external references to the sink or display, the sink or display is destroyed.
		 */
		bool			streamStop(Error& err = Error::Default())
		{
			bool status = c_interface::ic4_grabber_stream_stop( ptr_ );
			if (status)
			{
				current_sink.reset();
				current_display.reset();
			}
			return detail::returnUpdateFromLastError(err, status);
		}

		/**
		 * @brief Checks whether image acquisition is currently enabled for this grabber's video capture device.
		 * 
		 * @return	\c true, if image acquisition is currently active, otherwise \c false.
		 * 
		 * @remarks
		 * In contrast to @ref isStreaming, this additionally checks whether the device was instructed
		 * to begin image acquisition.
		 */
		bool				isAcquisitionActive() const
		{
			return c_interface::ic4_grabber_is_acquisition_active( ptr_ );
		}

		/**
		 * @brief Checks whether the grabber currently has an opened video capture device.
		 * 
		 * @return \c true, if the grabber has an opened video capture device, otherwise \c false.
		 */
		bool                isDeviceOpen() const
		{
			return c_interface::ic4_grabber_is_device_open( ptr_ );
		}

		/**
		 * @brief Returns information about the currently opened video capture device.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A device information object for the currently opened video capture device.\n
		 *			If an error occurs, an invalid object is returned. Check the @c err output parameter or @ref PropertyMap::is_valid().
		 */
		DeviceInfo	    deviceInfo(Error& err = Error::Default()) const
		{
			c_interface::IC4_DEVICE_INFO* dev = nullptr;
			if (c_interface::ic4_grabber_get_device( ptr_, &dev))
			{
				return detail::clearReturn(err, DeviceInfo(dev));
			}
			return detail::updateFromLastErrorReturn(err, DeviceInfo());
		}

		/**
		 * @brief Checks whether the grabber's currently opened video capture device is ready to use.
		 * 
		 * @return \c true, if the grabber has an opened video capture device that is ready to use, otherwise \c false.
		 * 
		 * @remarks
		 * There are multiple reasons for why this function may return \c false:
		 *	- No device has been opened
		 *	- The device was disconnected
		 *	- There is a loose hardware connection
		 *	- There was an internal error in the video capture device
		 *	- There was a driver error
		 */
		bool				isDeviceValid() const
		{
			return c_interface::ic4_grabber_is_device_valid( ptr_ );
		}

		/**
		 * @brief Closes the video capture device currently opened by this grabber instance
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @remarks
		 * If the device is closed, all its resources are released:
		 *	- If image acquisition is active, it is stopped.
		 *	- If a data stream was set up, it is stopped.
		 *  - References to data stream-related objects are released, possibly destroying the sink and/or display.
		 *  - Property objects retrieved from the device's @ref PropertyMap objects become invalid.
		 */
		bool                deviceClose(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_close( ptr_ ));
		}

		/**
		 * @brief Returns the property map for the currently opened video capture device.
		 * 
		 * The property map returned from this function is the origin for all device feature manipulation operations.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return	A @ref PropertyMap object.\n
		 *			If an error occurs, an invalid object is returned. Check the @c err output parameter or @ref PropertyMap::is_valid().
		 */
		PropertyMap			devicePropertyMap(Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY_MAP* map = nullptr;
			if (c_interface::ic4_grabber_device_get_property_map( ptr_, &map))
			{
				return detail::clearReturn(err, detail::propmap_attach(map));
			}
			return detail::updateFromLastErrorReturn(err, detail::propmap_invalid());
		}

		/**
		 * @brief Returns the property map for the driver of the currently opened video capture device.
		 *
		 * The property map returned from this function is the origin for driver-related feature operations.
		 *
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A @ref PropertyMap object.\n
		 *			If an error occurs, an invalid object is returned. Check the @c err output parameter or @ref PropertyMap::is_valid().
		 */
		PropertyMap			driverPropertyMap(Error& err = Error::Default()) const
		{
			c_interface::IC4_PROPERTY_MAP* map = nullptr;
			if (c_interface::ic4_grabber_driver_get_property_map(ptr_, &map))
			{
				return detail::clearReturn(err, detail::propmap_attach(map));
			}
			return detail::updateFromLastErrorReturn(err, detail::propmap_invalid());
		}

		/**
		 * @brief Contains statistics counters that can be used to analyze the stream behavior and identify possible bottlenecks.
		 * 
		 * This structure is filled by calling @ref Grabber::streamStatistics().
		 */
		struct StreamStatistics
		{
			uint64_t device_delivered = 0;				///< Number of frames delivered by the device
			uint64_t device_transmission_error = 0;		///< Number of frames dropped because of transmission errors, e.g. unrecoverable packet loss
			uint64_t device_underrun = 0;				///< Number of frames dropped by the device driver, because there was no free image buffer available

			uint64_t transform_delivered = 0;			///< Number of frames delivered by the transform element
			uint64_t transform_underrun = 0;			///< Number of frames dropped by the transform element, because there was no free image buffer available

			uint64_t sink_delivered = 0;				///< Number of frames processed by the sink
			uint64_t sink_underrun = 0;					///< Number of frames dropped by the sink, because there was no free image buffer available
			uint64_t sink_ignored = 0;					///< Number of frames ignored by the sink, because the sink was disabled or not instructed to process the data
		};

		/**
		 * @brief Query statistics counters from the currently running or previously stopped data stream.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A @ref StreamStatistics structure containing statistics counters.
		 * 
		 * @pre This operation is only valid after a data stream was established once.
		 */
		StreamStatistics streamStatistics(Error& err = Error::Default())
		{
			StreamStatistics result = {};

			c_interface::IC4_STREAM_STATS cstats = {};
			if (!c_interface::ic4_grabber_get_stream_stats( ptr_, &cstats))
				return detail::updateFromLastErrorReturn(err, result);

			result.device_delivered = cstats.device_delivered;
			result.device_transmission_error = cstats.device_transmission_error;
			result.device_underrun = cstats.device_underrun;
			result.transform_delivered = cstats.transform_delivered;
			result.transform_underrun = cstats.transform_underrun;
			result.sink_delivered = cstats.sink_delivered;
			result.sink_underrun = cstats.sink_underrun;
			result.sink_ignored = cstats.sink_ignored;

			return detail::clearReturn(err, result);
		}

		/**
		 * @brief Saves the currently opened video capture device and all its settings into a memory buffer.
		 * 
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return A vector of bytes containing the device state data.
		 * 
		 * @note
		 * To restore the device state at a later time, use @ref Grabber::deviceOpenFromState(const std::vector<uint8_t>&, Error&). \n
		 * In addition to serializing the device's properties (like @ref PropertyMap::serialize() would), this function also saves the
		 * currently opened video capture device so that it can be re-opened at a later time with all settings restored.
		 * 
		 * @see Grabber::deviceOpenFromState(const std::vector<uint8_t>&, Error&)
		 * @see Grabber::deviceSaveState(const std::string&, Error&)
		 */
		std::vector<uint8_t> deviceSaveState(Error& err = Error::Default())
		{
			std::vector<uint8_t> result;

			void* data_ptr = nullptr;
			size_t data_size = 0;
			if (!c_interface::ic4_grabber_device_save_state(ptr_, std::malloc, &data_ptr, &data_size))
				return detail::updateFromLastErrorReturn(err, result);

			result.assign(static_cast<uint8_t*>(data_ptr), static_cast<uint8_t*>(data_ptr) + data_size);
			std::free(data_ptr);

			return detail::clearReturn(err, result);
		}

		/**
		 * @brief Saves the currently opened video capture device and all its settings into a file.
		 * 
		 * @param file_path			Path to a file that the device stats is written to
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @note
		 * To restore the device state at a later time, use @ref Grabber::deviceOpenFromState(const std::string&, Error&). \n
		 * In addition to serializing the device's properties (like @ref PropertyMap::serialize() would), this function also saves the
		 * currently opened video capture device so that it can be re-opened at a later time with all settings restored.
		 * 
		 * @see Grabber::deviceOpenFromState(const std::string&, Error&)
		 * @see Grabber::deviceSaveState(std::vector<uint8_t>&, Error&)
		 */
		bool deviceSaveState(const std::string& file_path, Error& err = Error::Default())
		{
			return deviceSaveState(file_path.c_str(), err);
		}
		/**
		 * @brief Saves the currently opened video capture device and all its settings into a file.
		 *
		 * @param file_path			Path to a file that the device stats is written to
		 * @param[out] err			Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @note
		 * To restore the device state at a later time, use @ref Grabber::deviceOpenFromState(const char*, Error&). \n
		 * In addition to serializing the device's properties (like @ref PropertyMap::serialize() would), this function also saves the
		 * currently opened video capture device so that it can be re-opened at a later time with all settings restored.
		 * 
		 * @see Grabber::deviceOpenFromState(const char*, Error&)
		 * @see Grabber::deviceSaveState(std::vector<uint8_t>&, Error&)
		 */
		bool deviceSaveState(const char* file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_save_state_to_file(ptr_, file_path));
		}
#ifdef _WIN32
		bool deviceSaveState(const std::wstring& file_path, Error& err = Error::Default())
		{
			return deviceSaveState(file_path.c_str(), err);
		}
		bool deviceSaveState(const wchar_t* file_path, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_device_save_state_to_fileW(ptr_, file_path));
		}
#endif

	public:
		/**
		 * @brief Represents a registered callback.
		 *
		 * When a callback function is registered using @ref Grabber::eventAddDeviceLost, a token is returned.
		 *
		 * The token can then be used to remove the callback using @ref Grabber::eventRemoveDeviceLost at a later time.
		 */
		using NotificationToken = void*;
		
	private:
		struct DeviceLostNotificationFunction
		{
			std::function<void(Grabber&)> func;
			Grabber& self;
		};

		static void handle_devicelost_notification(c_interface::IC4_GRABBER* grabber_ptr, void* user_ptr)
		{
			(void)grabber_ptr;

			auto* fn = static_cast<DeviceLostNotificationFunction*>(user_ptr);
			assert(grabber_ptr == fn->self.ptr_);

			fn->func(fn->self);
		}

		static void delete_devicelost_notification(void* user_ptr)
		{
			auto* fn = static_cast<DeviceLostNotificationFunction*>(user_ptr);
			delete fn;
		}

	public:
		/**
		 * @brief Function prototype for device-lost event handlers.
		 *
		 * @param[in] grabber	The grabber on which the event handler was registered.
		 */
		using DeviceLostHandler = std::function<void(Grabber& grabber)>;

		/**
		 * @brief Registers a new device-lost event handler.
		 *
		 * @param[in] cb	Callback function to be called when the connection to the currently opened device ist lost.
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A token that can be used to unregister the callback using @ref Grabber::eventRemoveDeviceLost().\n
		 *			If an error occurrs, the function returns @c nullptr.
		 *
		 * @see Grabber::eventRemoveDeviceLost
		 */
		NotificationToken	eventAddDeviceLost(DeviceLostHandler cb, Error& err = Error::Default())
		{
			auto* fn = new DeviceLostNotificationFunction{ cb, *this };

			if (!c_interface::ic4_grabber_event_add_device_lost( ptr_, handle_devicelost_notification, fn, delete_devicelost_notification))
			{
				delete fn;
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, fn);
		}
		/**
		 * @brief Unregisters a device-lost event handler.
		 *
		 * @param[in] token		A token that was returned when registering an event handler using @ref Grabber::eventAddDeviceLost().
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @see Grabber::eventAddDeviceLost
		 */
		bool				eventRemoveDeviceLost(NotificationToken token, Error& err = Error::Default())
		{
			auto* fn = static_cast<DeviceLostNotificationFunction*>(token);

			return detail::returnUpdateFromLastError(err, c_interface::ic4_grabber_event_remove_device_lost( ptr_, handle_devicelost_notification, fn));
		}
	};

	namespace detail
	{
		inline c_interface::IC4_GRABBER* grabber_ptr(const Grabber& grabber) noexcept
		{
			return grabber.ptr_;
		}

		inline Grabber grabber_wrap(c_interface::IC4_GRABBER* handle) noexcept
		{
			return Grabber(handle);
		}
	}
}

#endif // IC4_GRABBER_H_INC_
