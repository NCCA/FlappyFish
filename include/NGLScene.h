

#ifndef NGLSCENE_H__
#define NGLSCENE_H__

#include "OpenGLWindow.h"
#include <ngl/Transformation.h>
#include <Box2D/Box2D.h>
#include <QSet>
#include <ngl/Obj.h>


/// Box2D v2.3.0 User Manual >> http://box2d.org/manual.pdf

class NGLScene : public OpenGLWindow
{
    private:
        double leftSideScreen = 0.0;
        double rightSideScreen = 0.0;
        static constexpr int spaceBetweenObstacles = 16;

        b2World *m_world;
        b2Body *m_player;
        ngl::Obj *m_playerObj;
        std::array<b2Body*,6> m_obstacles;

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution;
        int randomHeightPos;

        ngl::Transformation m_transform;

        QSet<Qt::Key> m_keysPressed;
        ngl::Mat4 m_view;
        ngl::Mat4 m_projection;

    private:
        void loadMatricesToShader   ();
        void keyPressEvent          (QKeyEvent *_event);
        void timerEvent             (QTimerEvent *_event);

        void collision();
        void reset();

        void rtfm();

    public:
        NGLScene(QWindow *_parent=nullptr);

        void initialize();
        void render();


        ~NGLScene();

};



#endif
