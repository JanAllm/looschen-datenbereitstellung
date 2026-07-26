
#ifndef IC4_DISPLAY_H_INC_
#define IC4_DISPLAY_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Display.h"

#include "Error.h"
#include "HandleRef.h"
#include "ImageBuffer.h"

#include <memory>

namespace ic4
{
	/**
	 * @brief Type definition for parent window handles
	 */
	using WindowHandle = c_interface::IC4_WINDOW_HANDLE;

	/**
	 * @brief Defines the possible display types
	*/
	enum class DisplayType
	{
		/**
		 * @brief Selects the platform's default display type.
		 *
		 * For Windows, this is Win32OpenGL.
		 */
		Default = c_interface::IC4_DISPLAY_TYPE::IC4_DISPLAY_DEFAULT,
		/**
		 * @brief Optimized OpenGL display for Windows platform
		 */
		Win32OpenGL = c_interface::IC4_DISPLAY_TYPE::IC4_DISPLAY_WIN32_OPENGL,
	};

	/**
	 * @brief Contains the possible display alignment and stretch modes
	 */
	enum class DisplayRenderPosition
	{
		TopLeft = c_interface::IC4_DISPLAY_RENDER_POSITION::IC4_DISPLAY_RENDER_POSITION_TOPLEFT,				///< %Display images unscaled at the top left corner of the window
		Center = c_interface::IC4_DISPLAY_RENDER_POSITION::IC4_DISPLAY_RENDER_POSITION_CENTER,					///< %Display images unscaled an the center of the window
		StretchTopLeft = c_interface::IC4_DISPLAY_RENDER_POSITION::IC4_DISPLAY_RENDER_POSITION_STRETCH_TOPLEFT,	///< %Display images stretched at the top left corner of the window
		StretchCenter = c_interface::IC4_DISPLAY_RENDER_POSITION::IC4_DISPLAY_RENDER_POSITION_STRETCH_CENTER,	///< %Display images stretched at the center of the window
		Custom = c_interface::IC4_DISPLAY_RENDER_POSITION::IC4_DISPLAY_RENDER_POSITION_CUSTOM,					///< %Display images at custom coordinates
	};

	/**
	 * @brief A structure containing display statistics
	 *
	 * This structure contains information about the number of frames that were
	 * displayed or dropped by a display.
	 */
	struct DisplayStatistics
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

	/**
	 * @brief Represents a display that can be used to display images.
	 * 
	 * To create a display, call @ref Display::create() or @ref ExternalOpenGLDisplay::create().
	 * 
	 * Display objects are generally used in two distinct ways:
	 * -	The display is connected to a data stream when calling @ref Grabber::streamSetup(), automatically displaying
	 *		all images from the opened device.
	 * -	@ref ImageBuffer objects are displayed manually by calling @ref Display::displayBuffer().
	 * 
	 * Display objects are neither copyable nor movable, and are only handled via @c std::shared_ptr<Display>.
	 * 
	 * @note
	 * Some functions, such as @ref Grabber::streamSetup(), takes shared ownership of the display object.
	 * The display is kept alive by the @ref Grabber instance even if no external reference to the display object exists.
	 * 
	 * @see Display::create
	 */
	class Display
	{
#ifndef IC4_DOXYGEN_INVISIBLE
	protected:

		detail::FixedHandleRef<c_interface::IC4_DISPLAY, c_interface::ic4_display_unref> ptr_;

		friend class Grabber;
#endif
	public:
#if defined _WIN32 || defined IC4_DOXYGEN_INVISIBLE
		/**
		 * @brief Creates a new display
		 * 
		 * @param[in] type		The type of display to create
		 * @param[in] hParent	Handle to the parent window to embed the display into.
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The new display, or @c nullptr if an error occurs.
		 * 
		 * @warning
		 * This function only works in Windows platforms. For other platforms, use @ref ExternalOpenGLDisplay::create().
		 */
		static std::shared_ptr<Display> create(DisplayType type, WindowHandle hParent, Error& err = Error::Default())
		{
			auto display_type = static_cast<c_interface::IC4_DISPLAY_TYPE>(type);
			c_interface::IC4_DISPLAY* display = nullptr;
			if (!c_interface::ic4_display_create(display_type, hParent, &display))
			{
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, std::shared_ptr<Display>(new Display(display)));
		}
#endif
	public:
		/**
		 * @brief Displays a specific image buffer.
		 * 
		 * @param[in] buffer	The buffer to display
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @remarks
		 * When @a buffer is @c nullptr, the display is cleared and will no longer display the previous buffer.
		 * 
		 */
		bool displayBuffer(const std::shared_ptr<ic4::ImageBuffer>& buffer, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_display_buffer(ptr_, detail::buffer_ptr(buffer)));
		}

		/**
		 * @brief Configure the image scaling and alignment options for a display.
		 * 
		 * @param[in] pos		The scaling and alignment mode to use
		 * @param[in] left		The left coordinate of the target rectangle inside the display window
		 * @param[in] top		The top coordinate of the target rectangle inside the display window
		 * @param[in] width		The width of the target rectangle inside the display window
		 * @param[in] height	The height of the target rectangle inside the display window
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 * 
		 * @remarks
		 * The \a left, \a top, \a width and \a height parameters are ignored unless \a mode is @ref DisplayRenderPosition::Custom.
		 */
		bool setRenderPosition(DisplayRenderPosition pos, int left = -1, int top = -1, int width = -1, int height = -1, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_set_render_position(ptr_, static_cast<c_interface::IC4_DISPLAY_RENDER_POSITION>(pos), left, top, width, height));
		}		

		/**
		 * @brief Queries display statistics.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return A @ref DisplayStatistics structure containing the number of displayed and dropped frames.
		 */
		DisplayStatistics statistics(Error& err = Error::Default())
		{
			DisplayStatistics result = {};

			c_interface::IC4_DISPLAY_STATS stats = {};
			if (!c_interface::ic4_display_get_stats(ptr_, &stats))
			{
				return detail::updateFromLastErrorReturn(err, result);
			}

			result.num_frames_displayed = stats.num_frames_displayed;
			result.num_frames_dropped = stats.num_frames_dropped;

			return detail::clearReturn(err, result);
		}
	private:
		struct WindowClosedFunction
		{
			std::function<void(Display&)> func;
		};

		static void handle_window_closed(c_interface::IC4_DISPLAY* display_ptr, void* user_ptr)
		{
			auto* fn = static_cast<WindowClosedFunction*>(user_ptr);

			Display disp(display_ptr, true);
			fn->func(disp);
		}

		static void delete_window_closed(void* user_ptr)
		{
			auto* fn = static_cast<WindowClosedFunction*>(user_ptr);
			delete fn;
		}

	public:
		/**
		 * @brief Function prototype for window-closed event handlers.
		 *
		 * @param[in] display	The Display on which the event handler was registered.
		 */
		using WindowClosedHandler = std::function<void(Display& display)>;
		/**
		 * @brief Represents a registered callback.
		 *
		 * When a callback function is registered using @ref Display::eventAddWindowClosed, a token is returned.
		 *
		 * The token can then be used to remove the callback using @ref Display::eventRemoveWindowClosed at a later time.
		 */
		using NotificationToken = void*;

		/**
		 * @brief Registers a new window-closed event handler.
		 *
		 * @param[in] cb		Callback function to be called when the display window is closed.
		 * @param[out] err      Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return	A token that can be used to unregister the callback using @ref Display::eventRemoveWindowClosed().\n
		 *			If an error occurrs, the function returns @c nullptr.
		 *
		 * @see Display::eventRemoveWindowClosed
		 */
		NotificationToken eventAddWindowClosed(WindowClosedHandler cb, Error& err = Error::Default())
		{
			auto* fn = new WindowClosedFunction{ cb };

			if (!c_interface::ic4_display_event_add_window_closed(ptr_, handle_window_closed, fn, delete_window_closed))
			{
				delete fn;
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, fn);
		}
		/**
		 * @brief Unregisters a window-closed event handler.
		 *
		 * @param[in] token		A token that was returned when registering an event handler using @ref Display::eventAddWindowClosed().
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return @c true on success, otherwise @c false.
		 *
		 * @see Display::eventAddWindowClosed
		 */
		bool eventRemoveWindowClosed(NotificationToken token, Error& err = Error::Default())
		{
			auto* fn = static_cast<WindowClosedFunction*>(token);

			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_event_remove_window_closed(ptr_, handle_window_closed, fn));
		}
#ifndef IC4_DOXYGEN_INVISIBLE
	protected:
		Display( c_interface::IC4_DISPLAY* display, bool add_ref = false ) noexcept
			: ptr_(add_ref ? c_interface::ic4_display_ref(display) : display)
		{
		}
#endif
	};

	/**
	 * @brief A specialized type of display able to render into an externally created OpenGL window
	 */
	class ExternalOpenGLDisplay : public Display
	{
	public:
		/**
		 * @brief Creates a new external OpenGL display
		 *
		 * @param err				Reference to an error handler. See @ref technical_article_error_handling for details.
		 *
		 * @return The new display, or @c nullptr if an error occurs.
		 */
		static std::shared_ptr<ExternalOpenGLDisplay> create(Error& err = Error::Default())
		{
			c_interface::IC4_DISPLAY* display = nullptr;
			if (!c_interface::ic4_display_create_external_opengl(&display))
			{
				return detail::updateFromLastErrorReturn(err, nullptr);
			}

			return detail::clearReturn(err, std::shared_ptr<ExternalOpenGLDisplay>(new ExternalOpenGLDisplay(display)));
		}
	public:
		/**
		 * @brief Initialize the external OpenGL display
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return @c true on success, otherwise @c false.
		 *
		 * @remarks
		 * This function must be called with the OpenGL context activated for the executing thread (e.g. @c makeCurrent).
		 */
		bool initialize(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_external_opengl_initialize(ptr_));
		}

		/**
		 * @brief Updates the external OpenGL display with the newest image available.
		 *
		 * @param[in] width		Width of the display window in physical pixels
		 * @param[in] height	Height of the display window in physical pixels
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return @c true on success, otherwise @c false.
		 *
		 * @remarks
		 * This function must be called with the OpenGL context activated for the executing thread (e.g. @c makeCurrent).
		 */
		bool render(int width, int height, Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_external_opengl_render(ptr_, width, height));
		}

		/**
		 * @brief Notifies the display component that the window was closed.
		 * 
		 * @param[out] err		Reference to an error handler. See @ref technical_article_error_handling for details.
		 * 
		 * @return @c true on success, otherwise @c false.
		 */
		bool notifyWindowClosed(Error& err = Error::Default())
		{
			return detail::returnUpdateFromLastError(err, c_interface::ic4_display_external_opengl_notify_window_closed(ptr_));
		}

	private:
		ExternalOpenGLDisplay(c_interface::IC4_DISPLAY* display, bool add_ref = false) noexcept
			: Display(display, add_ref)
		{
		}
	};
}

#endif //IC4_DISPLAY_H_INC_