

#include "OpenGLWindow.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <iostream>




OpenGLWindow::OpenGLWindow(QWindow *_parent)
    : QWindow(_parent)
    , m_updatePending(false)
    , m_context(0)

{

  setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{

}


void OpenGLWindow::renderLater()
{
  if (!m_updatePending)
  {
    m_updatePending = true;

    QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool OpenGLWindow::event(QEvent *event)
{
  switch (event->type())
  {
  case QEvent::UpdateRequest:
    renderNow();
    return true;

  default:
    return QWindow::event(event);
  }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{

  Q_UNUSED(event);

  if (isExposed())
  {
    renderNow();
  }
}


void OpenGLWindow::renderNow()
{

  if (!isExposed())
  {
    return;
  }

  m_updatePending = false;

  bool needsInitialize = false;

  if (!m_context)
  {
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();

    needsInitialize = true;
    m_context->makeCurrent(this);

    initialize();

  }

  m_context->makeCurrent(this);

  render();

  m_context->swapBuffers(this);
}


