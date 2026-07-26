// Sink.h: interface for the Sink class.
//
//////////////////////////////////////////////////////////////////////

#ifndef IC4_SINK_H_
#define IC4_SINK_H_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Sink.h"

#include "HandleRef.h"

namespace ic4
{
	/**
	 * Identifies the type of a sink
	 */
	enum class SinkType
	{
		QueueSink = c_interface::IC4_SINK_TYPE_QUEUESINK,	///< Queue sink
		SnapSink = c_interface::IC4_SINK_TYPE_SNAPSINK,		///< Snap sink
	};

	/**
	 * @brief Abstract base class for sinks.
	 * 
	 * Sink objects provide programmatic access to the image data acquired from video capture devices.
	 *
	 * There are multiple sink types available:
	 *  - A @ref QueueSink is recommended when a program needs to process all or most images received from the device.
	 *  - A @ref SnapSink can be used to capture images or short image sequences on demand.
	 *
	 * To create a sink object, use its creation function, e.g. @ref QueueSink::create() or @ref SnapSink::create().
	 *
	 * A sink is connected to a video capture device using the @ref Grabber::streamSetup() function.
	 */
	class Sink
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		friend class Grabber;
#endif

	protected:
#ifndef IC4_DOXYGEN_INVISIBLE
		detail::FixedHandleRef<c_interface::IC4_SINK, c_interface::ic4_sink_unref> ptr_;
#endif

	public:
		virtual ~Sink() = default;

	public:
		/**
		 * Defines the possible sink modes
		 */
		enum class SinkMode
		{
			/**
			 * The sink is operational.
			 */
			Run = c_interface::IC4_SINK_MODE_RUN,

			/**
			 * The sink is paused, immediately returning all image buffers it receives to the device to the device driver
			 */
			Pause = c_interface::IC4_SINK_MODE_PAUSE
		};

	public:
		/**
		 * @brief Sets the sink mode for a sink.
		 * @param[in] mode	The new sink mode
		 * @param[out] err	Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	@c true on success, otherwise @c false.\n
		 *			Check the @a err output parameter for error code and error message.
		 */
		bool   setSinkMode(SinkMode mode, Error& err = Error::Default()) noexcept
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_sink_set_mode( ptr_, static_cast<c_interface::IC4_SINK_MODE>(mode)));
		}

		/**
		 * @brief Gets the current sink mode of a sink
		 * 
		 * @return The current sink mode
		 */
		SinkMode  sinkMode() const noexcept
		{
			return static_cast<SinkMode>(c_interface::ic4_sink_get_mode( ptr_ ));
		}

		/**
		 * @brief Checks whether a sink is currently attached to a @ref Grabber as part of a data stream.
		 * 
		 * @return @c true, if the sink is currently attached to a @ref Grabber, otherwise @c false.
		 */
		bool isAttached() const noexcept
		{
			return c_interface::ic4_sink_is_attached( ptr_ );
		}

		/**
		 * @brief Returns the type of this sink
		 * 
		 * @return The type of this sink
		 */
		virtual SinkType	sinkType() const noexcept = 0;

    protected:
#ifndef IC4_DOXYGEN_INVISIBLE
        Sink( c_interface::IC4_SINK* p ) noexcept
            : ptr_( p )
        {
        }
#endif
	};
}

#endif // IC4_SINK_H_
