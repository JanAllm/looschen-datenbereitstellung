
#ifndef IC4_C_DISPLAY_H_INC_
#define IC4_C_DISPLAY_H_INC_

#include "ic4core_export.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * \defgroup display Display
 *
 * The display functions provide an easy and fast way to display images.
 *
 * To create a display, call ic4_display_create().
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
	 * @brief Defines the possible display types
	 */
	enum IC4_DISPLAY_TYPE
	{
		/**
		 * @brief Selects the platform's default display type.
		 *
		 * For Windows, this is IC4_DISPLAY_WIN32_OPENGL.
		 */
		IC4_DISPLAY_DEFAULT,

		/**
		 * @brief Optimized OpenGL display for Windows platform
		 */
		IC4_DISPLAY_WIN32_OPENGL,
	};

	/**
	 * @struct IC4_DISPLAY
	 * @brief Represents a display that can be used to display images.
	 * 
	 * This type is opaque, programs only use pointers of type \c IC4_DISPLAY*.
	 * 
	 * To create a display, use @ref ic4_display_create() or @ref ic4_display_create_external_opengl().
	 * 
	 * Display objects are generally used in two distinct ways:
	 * -	The display is connected to a data stream when calling @ref ic4_grabber_stream_setup(),
	 *		automatically displaying all images from the opened device.
	 * -	@ref IC4_IMAGE_BUFFER objects are displayed manually by calling @ref ic4_display_display_buffer().
	 * 
	 * Display objects are reference-counted. The initial reference count is one.
	 * 
	 * To share a display object between multiple parts of a program, use ic4_display_ref().
	 * Call ic4_display_unref() when a reference is no longer required.
	 * If the reference count reaches zero, the display object is destroyed.
	 * 
	 * @note
	 * Some functions, such as @ref ic4_grabber_stream_setup(), share ownership of the display object passed as an argument.
	 * The display object is kept alive by the #IC4_GRABBER instance even if no external references exist.
	 * 
	 * @see ic4_display_create
	 * @see ic4_display_ref
	 * @see ic4_display_unref
	 */
	struct IC4_DISPLAY;

	struct IC4_IMAGE_TYPE;
	struct IC4_IMAGE_BUFFER;

#ifndef IC4_DOXYGEN_INVISIBLE
#ifdef _WIN32
	typedef void* IC4_WINDOW_HANDLE;
#define IC4_WINDOW_HANDLE_NULL NULL
#else
	typedef long unsigned int IC4_WINDOW_HANDLE;
#define IC4_WINDOW_HANDLE_NULL 0
#endif
#else
	/**
	 * @brief A window handle to be used as parent
	 * 
	 * The type of \c IC4_WINDOW_HANDLE is platform-specific.
	 */
	typedef platform_specific IC4_WINDOW_HANDLE;

	/**
	 * @brief An invalid window handle value.
	 * 
	 * When passed to a function, this value specifies that no parent window is set.
	 * 
	 * The value of @c IC4_WINDOW_HANDLE_NULL is platform-specific.
	 */
#define IC4_WINDOW_HANDLE_NULL platform_specific
#endif

	/**
	 * @brief A structure containing display statistics
	 * 
	 * This structure contains information about the number of frames that were
	 * displayed or dropped by a display.
	 */
	struct IC4_DISPLAY_STATS
	{
		/**
		 * @brief The number of frames that were displayed by a display
		 */
		uint64_t num_frames_displayed;
		/**
		 * @brief The number of frames that were passed to a display, but not displayed
		 * 
		 * A frame is considered dropped by a display, when the display receives a new frame
		 * before the previous frame was rendered.
		 */
		uint64_t num_frames_dropped;
	};

#if defined _WIN32 || defined IC4_DOXYGEN_INVISIBLE
	/**
	 * @brief Creates a new display.
	 * 
	 * @param[in] type The type of display to create
	 * @param[in] parent Handle to the parent window to embed the display into.
	 * @param[out] ppDisplay Pointer to receive the handle to the new display object.\n
	 *             When the display is no longer required, release the object reference using ic4_display_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @warning
	 * This function only works in Windows platforms. For other platforms, use @ref ic4_display_create_external_opengl().
	 * 
	 * @see ic4_display_unref
	 */
	IC4CORE_API bool ic4_display_create(enum IC4_DISPLAY_TYPE type, IC4_WINDOW_HANDLE parent, struct IC4_DISPLAY** ppDisplay);
#endif
	/**
	 * @brief Creates a new external OpenGL display
	 * 
	 * @param[out]			ppDisplay Pointer to receive the handle to the new display object.\n
	 *						When the display is no longer required, release the object reference using ic4_display_unref().
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * To use the external renderer, the application has to follow these steps:
	 * - Create an OpenGL window, typically using the UI toolkit of the application
	 * - Call @ref ic4_display_external_opengl_initialize with the OpenGL context activated for the active thread
	 * - Repeatedly call @ref ic4_display_external_opengl_render with the OpenGL context activated for the active thread
	 *
	 * @see ic4_display_unref
	 */
	IC4CORE_API bool ic4_display_create_external_opengl(struct IC4_DISPLAY** ppDisplay);

	/**
	 * @brief Increases the display's internal reference count by one.
	 *
	 * @param[in] pDisplay A pointer to a display
	 *
	 * @return The pointer passed via \a pDisplay
	 *
	 * @remarks If \a pDisplay is \c NULL, the function returns \c NULL. An error value is not set.
	 *
	 * @see ic4_display_unref
	 */
	IC4CORE_API struct IC4_DISPLAY* ic4_display_ref(struct IC4_DISPLAY* pDisplay);

	/**
	 * @brief Decreases the display's internal reference count by one.
	 *
	 * If the reference count reaches zero, the object is destroyed.
	 *
	 * @param[in] pDisplay A pointer to a display
	 *
	 * @remarks If \a pDisplay is \c NULL, the function does nothing. An error value is not set.
	 *
	 * @see ic4_display_ref
	 */
	IC4CORE_API void ic4_display_unref(struct IC4_DISPLAY* pDisplay);

	/**
	 * @brief Displays a specific image buffer.
	 * 
	 * @param[in] pDisplay A display
	 * @param[in] buffer The buffer to display
	 * 
	 * @remarks It is not always necessary to call this function.\n
	 *			When a display is registered with a #IC4_GRABBER using ic4_grabber_set_display(), images are displayed automatically.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 * 
	 * @remarks
	 * When @a buffer is @c NULL, the display is cleared and will no longer display the previous buffer.
	 */
	IC4CORE_API bool ic4_display_display_buffer(struct IC4_DISPLAY* pDisplay, const struct IC4_IMAGE_BUFFER* buffer);

	/**
	 * @brief Queries statistics about a display
	 * 
	 * @param[in] pDisplay	A display
	 * @param[out] stats	Pointer to a #IC4_DISPLAY_STATS structure receiving statistics about the display
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_display_get_stats(struct IC4_DISPLAY* pDisplay, struct IC4_DISPLAY_STATS* stats);

	/**
	 * @brief Contains the possible display alignment and stretch modes
	 */
	enum IC4_DISPLAY_RENDER_POSITION
	{
		IC4_DISPLAY_RENDER_POSITION_TOPLEFT,		///< Display images unscaled at the top left corner of the window
		IC4_DISPLAY_RENDER_POSITION_CENTER,			///< Display images unscaled an the center of the window
		IC4_DISPLAY_RENDER_POSITION_STRETCH_TOPLEFT,///< Display images stretched at the top left corner of the window
		IC4_DISPLAY_RENDER_POSITION_STRETCH_CENTER,	///< Display images stretched at the center of the window
		IC4_DISPLAY_RENDER_POSITION_CUSTOM			///< Display images at custom coordinates
	};

	/**
	 * @brief Configure the image scaling and alignment options for a display.
	 * 
	 * @param[in] pDisplay	A display
	 * @param[in] pos		The scaling and alignment mode to use
	 * @param[in] left		The left coordinate of the target rectangle inside the display window
	 * @param[in] top		The top coordinate of the target rectangle inside the display window
	 * @param[in] width		The width of the target rectangle inside the display window
	 * @param[in] height	The height of the target rectangle inside the display window
	 * 
	 * @remarks
	 * The \a left, \a top, \a width and \a height parameters are ignored unless \a pos is \c IC4_DISPLAY_RENDER_POSITION_CUSTOM.
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_display_set_render_position(struct IC4_DISPLAY* pDisplay, enum IC4_DISPLAY_RENDER_POSITION pos, int left, int top, int width, int height);

	/**
	 * Function pointer for the window-closed handler
	 *
	 * @param[in] pDisplay	Pointer to the display whose window was closed
	 * @param[in] user_ptr	User data that was specified when calling #ic4_display_event_add_window_closed()
	 */
	typedef void (*ic4_display_window_closed_handler)(struct IC4_DISPLAY* pDisplay, void* user_ptr);
	/**
	 * Function pointer for cleanup of the device-lost user data
	 *
	 * @param[in] user_ptr	User data that was specified when calling #ic4_grabber_event_add_device_lost()
	 */
	typedef void (*ic4_display_window_closed_deleter)(void* user_ptr);

	/**
	 * @brief Registers a callback to be called when the display is closed.
	 * 
	 * @param[in] pDisplay	A display
	 * @param[in] handler	The function to be called when the display is closed
	 * @param[in] user_ptr	User data to be passed in calls to \a handler.
	 * @param[in] deleter	A function to be called when the handler was unregistered and the user_ptr will no longer be required.\n
	 *						The deleter function can be used to release data associated with \a user_ptr.\n
	 *						The \a deleter function will be called when the display-closed handler is unregistered,
	 *						or the display object itself is destroyed.
	 * 
	 * @note
	 * To unregister a display-closed handler, call #ic4_display_event_remove_window_closed().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_display_event_add_window_closed(struct IC4_DISPLAY* pDisplay, ic4_display_window_closed_handler handler, void* user_ptr, ic4_display_window_closed_deleter deleter);
	/**
	 * Unregisters a display-closed handler that was previously registered using #ic4_display_event_add_window_closed().
	 *
	 * @param[in] pDisplay	The display on which the callback is currently registered
	 * @param[in] handler	Pointer to the function to be unregistered
	 * @param[in] user_ptr	User data that the function was previously registered with
	 *
	 * @note
	 * The pair of \a handler and \a user_ptr has to be an exact match to the parameters used in the call to #ic4_display_event_add_window_closed().
	 *
	 * @return \c true on success, otherwise \c false.\n
	 *			Use ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_display_event_remove_window_closed(struct IC4_DISPLAY* pDisplay, ic4_display_window_closed_handler handler, void* user_ptr);

	/**
	 * @brief Initialize the external OpenGL display
	 *
	 * @param[in] pDisplay	The external OpenGL display
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * This function must be called with the OpenGL context activated for the executing thread (e.g. @c makeCurrent).
	 */
	IC4CORE_API bool ic4_display_external_opengl_initialize(struct IC4_DISPLAY* pDisplay);
	/**
	 * @brief Updates the external OpenGL display with the latest image available.
	 *
	 * @param[in] pDisplay	The external OpenGL display
	 * @param[in] width		Width of the display window in physical pixels
	 * @param[in] height	Height of the display window in physical pixels
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 *
	 * @remarks
	 * This function must be called with the OpenGL context activated for the executing thread (e.g. @c makeCurrent).
	 */
	IC4CORE_API bool ic4_display_external_opengl_render(struct IC4_DISPLAY* pDisplay, int width, int height);
	/**
	 * @brief Notifies the external OpenGL display component that the window has been closed.
	 * 
	 * @param[in] pDisplay	The external OpenGL display
	 * 
	 * @return \c true on success, otherwise \c false.\n
	 *			Use @ref ic4_get_last_error() to query error information.
	 */
	IC4CORE_API bool ic4_display_external_opengl_notify_window_closed(struct IC4_DISPLAY* pDisplay);

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

#endif //IC4_C_DISPLAY_H_INC_