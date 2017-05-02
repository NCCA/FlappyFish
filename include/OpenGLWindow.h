

#ifndef OPENGLWINDOW_H__
#define OPENGLWINDOW_H__

#include <QtGui/QWindow>


class QOpenGLContext;

class OpenGLWindow : public QWindow
{

    Q_OBJECT

    private:
        bool m_updatePending;
        QOpenGLContext *m_context;

    protected:
        bool event(QEvent *event);
        void exposeEvent(QExposeEvent *event);

    public:
        explicit OpenGLWindow(QWindow *_parent = nullptr);

        virtual void render()=0;
        virtual void initialize()=0;

        ~OpenGLWindow();

    public slots:
        void renderLater();
        void renderNow();

};

#endif
