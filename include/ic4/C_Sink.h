
#ifndef IC4_C_SINK_H_INC_
#define IC4_C_SINK_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stdint.h>

/** @defgroup sink Sinks
 *
 * @brief Sink objects provide programmatic access to the image data acquired from video capture devices.
 *
 * There are multiple sink types available:
 *  - A @ref queuesink is recommended when a program needs to process all or most images received from the device.
 *  - A snap sink can be used to capture images or short image sequences on demand.
 *
 * To create a sink object, use its creation function, e.g. #ic4_queuesink_create() or #ic4_snapsink_create().
 *
 * A sink is connected to a video capture device using the #ic4_grabber_stream_setup() function.
 * 
 * All sink objects are referred to by pointers to the #IC4_SINK type.
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
	 * Identifies the type of a sink
	 */
	enum IC4_SINK_TYPE
	{
		IC4_SINK_TYPE_QUEUESINK = 4,				///< Queue sink
		IC4_SINK_TYPE_SNAPSINK = 5,					///< Snap sink

		IC4_SINK_TYPE_INVALID = -1,					///< Not a sink type
	};

	/**
	 * Defines the possible sink modes
	 */
	enum IC4_SINK_MODE
	{
		/**
		 * The sink is operational.
		 */
		IC4_SINK_MODE_RUN,
		/**
		 * The sink is paused, immediately returning all image buffers it receives to the device to the device driver
		 */
		IC4_SINK_MODE_PAUSE,

		/**
		 * Invalid sink mode, used to indicate an error
		 */
		IC4_SINK_MODE_INVALID = -1
	};	

	/**
	 * @struct IC4_SINK
	 * 
	 * @brief Represents a sink, allowing programmatic access to image data.
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_SINK*.
	 * 
	 * The \c IC4_SINK* handle type is used for all sink types, there is no type casting required.
	 * The actual type of a sink object can be examined by a call to #ic4_sink_get_type().
	 * 
	 * Sink objects are reference-counted, and created with an initial reference count of one.
	 * To share a sink object between multiple parts of a program, create a new reference by calling #ic4_sink_ref().
	 * When a reference is no longer required, call #ic4_sink_unref().
	 *
	 * If the sink object's internal reference count reaches zero, the sink object is destroyed.
	 */
	struct IC4_SINK;

	/**
	 * @brief Increases the sink's internal reference count by one.
	 *
	 * @param[in] pSink A pointer to a sink
	 *
	 * @return The pointer passed via \a pSink
	 *
	 * @remarks If \a pSink is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_sink_unref
	 */
	IC4CORE_API struct IC4_SINK* ic4_sink_ref(struct IC4_SINK* pSink);
	/**
	 * @brief Decreases the sink's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pSink A pointer to a sink
	 *
	 * @remarks
	 * If \a pSink is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_sink_ref
	 */
	IC4CORE_API void ic4_sink_unref(struct IC4_SINK* pSink);

	/**
	 * @brief Sets the sink mode for a sink.
	 * 
	 * @param[in] pSink	A sink
	 * @param[in] mode	The new sink mode
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_sink_set_mode(struct IC4_SINK* pSink, enum IC4_SINK_MODE mode);

	/**
	 * @brief Gets the current sink mode of a sink
	 * 
	 * @param[in] pSink	A sink
	 * 
	 * @return	A #IC4_SINK_MODE value describing the current sink mode.\n
	 *			If an error occurred, the return value is \c IC4_SINK_MODE_INVALID.
	 *			Use ic4_get_last_error() to query error information.
	 *			
	 */
	IC4CORE_API enum IC4_SINK_MODE ic4_sink_get_mode(struct IC4_SINK* pSink);

	/**
	 * Checks whether a sink is currently attached to a @ref grabber.
	 * 
	 * @param[in] pSink
	 * 
	 * @return \c true, if the sink is attached, otherwise \c false.
	 */
	IC4CORE_API bool ic4_sink_is_attached(struct IC4_SINK* pSink);

	/**
	 * Returns the actual type of the sink
	 * 
	 * @param[in] pSink A sink
	 * 
	 * @return The type of the sink, or \c IC4_SINK_TYPE_INVALID in case of an error.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API enum IC4_SINK_TYPE ic4_sink_get_type(struct IC4_SINK* pSink);

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

#endif //IC4_C_SINK_H_INC_