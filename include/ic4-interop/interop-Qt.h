
#ifndef IC4_QT_H_INC_
#define IC4_QT_H_INC_

#pragma once

#include <memory>
#include <mutex>

#include <QCoreApplication>
#include <QMainWindow>
#include <QOpenGLContext>
#include <QWidget>
#include <QWindow>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPlatformSurfaceEvent>

#include <ic4/ic4.h>

/**
 * @brief Contains functions providing interoperability between IC Imaging Control 4 and third-party libraries.
 *
 * The provided functions include conversions between @ref ic4::ImageBuffer objects and third-party library types
 * as well as display adapters for GUI toolkits.
 *
 * The interop support functions are declared separate headers:
 * - OpenCV: @c ic4interop/interop-OpenCV.h
 * - MVTec HALCON: @c ic4interop/interop-HALCON.h
 * - Qt5 and Qt6: @c interop/interop-Qt.h
 */
namespace ic4interop
{
	/**
	 * @brief Contains classes for displaying camera images in Qt5 or Qt6 applications.
	 */
	namespace Qt
	{
#ifndef IC4_DOXYGEN_INVISIBLE
		namespace detail
		{
			class DisplayWindow : public QWindow
			{
			public:
				DisplayWindow(QWidget* owner)
					: m_context(this)
					, owner_(owner)
				{
					setSurfaceType(QWindow::OpenGLSurface);
				}
			private:
				template<typename TLck>
				bool lazyInitialize(TLck& display_mtx_lck)
				{
					if (auto ic4display = m_ic4Display.lock(); !is_initialized_ && ic4display != nullptr)
					{
						if (!m_context.isValid())
						{
							m_context.setFormat(requestedFormat());
							if (!m_context.create())
							{
								return false;
							}
						}

						if (!m_context.makeCurrent(this))
						{
							return false;
						}

						ic4display->initialize();

						m_context.doneCurrent();

						is_initialized_ = true;
					}
					else if (is_initialized_ && ic4display == nullptr)
					{
						is_initialized_ = false;
					}

					return is_initialized_;
				}
				void uninitialize()
				{
					std::lock_guard lck(display_mtx_);

					if (auto ic4display = m_ic4Display.lock(); ic4display != nullptr)
					{
						ic4display->notifyWindowClosed();
					}
				}

				void renderNow(bool force = false)
				{
					if (!isExposed() && !force)
						return;

					std::lock_guard lck(display_mtx_);

					if (lazyInitialize(lck))
					{
						auto ic4display = m_ic4Display.lock();

						m_context.makeCurrent(this);

						auto ratio = owner_->devicePixelRatio();
						int w = (int)(width() * ratio);
						int h = (int)(height() * ratio);

						ic4display->render(w, h);

						m_context.swapBuffers(this);

						m_context.doneCurrent();
					}

					requestUpdate();
				}

			protected:
				bool event(QEvent* ev) override
				{
					switch (ev->type())
					{
					case QEvent::PlatformSurface:
						if (((QPlatformSurfaceEvent*)ev)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated)
						{
							std::lock_guard lck(display_mtx_);
							lazyInitialize(lck);
						}
						else
						{
							uninitialize();
						}
						return QWindow::event(ev);
					case QEvent::UpdateRequest:
						renderNow();
						return true;
					default:
						return QWindow::event(ev);
					}
				}
				void exposeEvent(QExposeEvent* ev) override
				{
					Q_UNUSED(ev);

					renderNow();
				}

			public:
				std::shared_ptr<ic4::Display> asDisplay()
				{
					std::lock_guard lck(display_mtx_);

					auto display = m_ic4Display.lock();
					if (display == nullptr)
					{
						m_ic4Display = display = ic4::ExternalOpenGLDisplay::create();
					}

					return display;
				}

			private:
				QWidget* owner_ = nullptr;
				QOpenGLContext m_context;

				std::mutex display_mtx_;
				std::weak_ptr<ic4::ExternalOpenGLDisplay> m_ic4Display;

				bool is_initialized_ = false;
			};
		}
#endif // !defined IC4_DOXYGEN_INVISIBLE

		/**
		 * @brief A Qt display widget
		 *
		 * Use @ref asDisplay to get a ic4::Display representing the display. The @ref ic4::Display can then be passed to @ref ic4::Grabber::streamSetup.
		 */
		class DisplayWidget : public QWidget
		{
			detail::DisplayWindow* display_window_ = nullptr;
			QWidget* display_container_ = nullptr;
		public:
			/**
			 * @brief Create a new Qt display widget
			 *
			 * @param parent	A parent widget
			 * @param f			Window flags
			 */
			DisplayWidget(QWidget* parent = nullptr, ::Qt::WindowFlags f = ::Qt::WindowFlags())
				: QWidget(parent, f)
				, display_window_(new detail::DisplayWindow(this))
			{
				display_container_ = QWidget::createWindowContainer(display_window_, this);

				auto layout = new QVBoxLayout(this);
				layout->setContentsMargins(QMargins());
				layout->addWidget(display_container_, 1);
				setLayout(layout);

				display_window_->installEventFilter(this);
			}

		public:
			/**
			 * @brief Returns a @ref ic4::Display to connect this display widget to a data stream.
			 * @return A @c shared_ptr to a @ref ic4::Display for this display widget.
			 *
			 * @remarks
			 * Pass the return value of this function to @ref ic4::Grabber::streamSetup to display live video on this display widget.
			 */
			std::shared_ptr<ic4::Display> asDisplay()
			{
				return display_window_->asDisplay();
			}

		protected:
#ifndef IC4_DOXYGEN_INVISIBLE
			bool eventFilter(QObject* object, QEvent* ev)
			{
				if (object != display_window_)
					return false;

				switch( ev->type() )
				{
				case QEvent::MouseButtonPress:
				case QEvent::MouseButtonRelease:
					QCoreApplication::sendEvent(this, ev);
					return false; // Let QWindow see the up/down events so that it can generate the ContextMenu event if required
				case QEvent::ContextMenu:
				case QEvent::MouseMove:
				case QEvent::MouseButtonDblClick:
				case QEvent::Wheel:
					return QCoreApplication::sendEvent(this, ev);
				default:
					return false;
				}
			}
#endif
		};

		/**
		 * @brief A Qt display window
		 *
		 * Use @ref asDisplay to get a ic4::Display representing the display. The @ref ic4::Display can then be passed to @ref ic4::Grabber::streamSetup.
		 */
		class DisplayWindow : public QMainWindow
		{
			DisplayWidget* display_widget_ = nullptr;
		public:
			/**
			 * @brief Create a new Qt display window
			 *
			 * @param parent	A parent widget
			 * @param f			Window flags
			 */
			DisplayWindow(QWidget* parent = nullptr, ::Qt::WindowFlags f = ::Qt::WindowFlags())
				: QMainWindow(parent, f)
			{
				display_widget_ = new DisplayWidget(this);

				setCentralWidget(display_widget_);
			}

		public:
			/**
			 * @brief Returns a @ref ic4::Display to connect this display window to a data stream.
			 * @return A @c shared_ptr to a @ref ic4::Display for this display window.
			 *
			 * @remarks
			 * Pass the return value of this function to @ref ic4::Grabber::streamSetup to display live video on this display window.
			 */
			std::shared_ptr<ic4::Display> asDisplay()
			{
				return display_widget_->asDisplay();
			}
		};
	}
}

#endif // IC4_QT_H_INC_