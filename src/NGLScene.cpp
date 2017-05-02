

#include <QGuiApplication>
#include <QKeyEvent>

#include "NGLScene.h"

#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <unistd.h>
#include <sys/types.h>


NGLScene::NGLScene(QWindow *_parent) :  OpenGLWindow(_parent),
                                        m_world(new b2World(b2Vec2(0.f, -10.f))),
                                        m_obstacles(std::array<b2Body*,6>{{}}),
                                        distribution(std::uniform_int_distribution<int>(0,6))
{
    setTitle("Flappy Fish");
}


NGLScene::~NGLScene()
{
    m_world->DestroyBody(m_player);
    m_player = nullptr;
}


void NGLScene::initialize()
{
    ngl::NGLInit::instance();

    glClearColor(0.f, 0.5f, 0.8f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);


    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    shader->createShaderProgram("TextureShader");
    shader->attachShader("TextureVertex",ngl::ShaderType::VERTEX);
    shader->attachShader("TextureFragment",ngl::ShaderType::FRAGMENT);
    shader->loadShaderSource("TextureVertex","shaders/TextureVertex.glsl");
    shader->loadShaderSource("TextureFragment","shaders/TextureFragment.glsl");
    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    shader->attachShaderToProgram("TextureShader","TextureVertex");
    shader->attachShaderToProgram("TextureShader","TextureFragment");
    shader->linkProgramObject("TextureShader");

    (*shader)["TextureShader"]->use();
    (*shader)["nglColourShader"]->use();


    glViewport(0,0,width(),height());
    m_projection=ngl::ortho(-40,40,-20,20, 0.1f, 10.0f);
    m_view=ngl::lookAt(ngl::Vec3(0,0,1),ngl::Vec3(0,0,0),ngl::Vec3(0,1,0));


    leftSideScreen = -(width()*0.04);
    rightSideScreen = width()*0.04;

    b2BodyDef bodyDefinition;
    b2PolygonShape bodyShape;
    b2FixtureDef bodyFixture;


    // PLAYER BODY DEFINITION
    bodyDefinition.type = b2_dynamicBody;

    // PLAYER SHAPE
    bodyShape.SetAsBox(3.f, 2.f);

    // PLAYER FIXTURE
    bodyFixture.shape = &bodyShape;
    bodyFixture.density = 10.f;

    // PLAYER BIND TO THE WORLD
    bodyDefinition.position.Set((leftSideScreen-rightSideScreen)*0.15,0);
    m_player = m_world->CreateBody(&bodyDefinition);
    m_player->SetAngularVelocity(0.f);
    m_player->SetAngularDamping(0.4f);
    m_player->SetGravityScale(2.5f);
    m_player->SetActive(true);
    m_player->CreateFixture(&bodyFixture);

    m_playerObj = new ngl::Obj("model_texture/fish.obj","model_texture/fish.jpg");
    m_playerObj->createVAO();

    // OBSTACLE BODY DEFINITION
    bodyDefinition.type = b2_staticBody;

    // OBSTACLE SHAPE
    bodyShape.SetAsBox(1.f, 10.f);

    // OBSTACLE FIXTURE
    bodyFixture.shape = &bodyShape;

    // OBSTACLES BIND TO THE WORLD
    for(std::size_t i=0; i<m_obstacles.size(); i+=2)
    {
        randomHeightPos = distribution(generator);

        bodyDefinition.position.Set(rightSideScreen+(i*spaceBetweenObstacles),(height()*0.025)+randomHeightPos);
        m_obstacles[i] = m_world->CreateBody(&bodyDefinition);
        m_obstacles[i]->CreateFixture(&bodyFixture);

        bodyDefinition.position.Set(rightSideScreen+(i*spaceBetweenObstacles),-(height()*0.025)+randomHeightPos);
        m_obstacles[i+1] = m_world->CreateBody(&bodyDefinition);
        m_obstacles[i+1]->CreateFixture(&bodyFixture);
    }


    startTimer(0);
}


void NGLScene::loadMatricesToShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    ngl::Mat4 MVP=m_transform.getMatrix()*m_view*m_projection;
    shader->setShaderParamFromMat4("MVP",MVP);
}


void NGLScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,width(),height());

    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    (*shader)["TextureShader"]->use();
    m_transform.reset();
    {
        m_transform.setPosition(m_player->GetPosition().x,m_player->GetPosition().y,0);
        m_transform.setRotation(0,90,m_player->GetAngle());
        m_transform.setScale(3.f,3.f,3.f);
        loadMatricesToShader();
        m_playerObj->draw();
    }

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    (*shader)["nglColourShader"]->use();
    shader->setShaderParam4f("Colour",0.f,1.f,0.f,1.0f);
    for(const auto &i : m_obstacles)
    {
        m_transform.reset();
        {
            m_transform.setPosition(i->GetPosition().x,i->GetPosition().y,0);
            m_transform.setScale(2.f,24.f,0.1f);
            loadMatricesToShader();
            prim->draw("cube");
        }
    }

    m_player->SetAngularVelocity(m_player->GetAngularVelocity()-m_player->GetAngularDamping());
}


void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    switch (_event->key())
    {
        case Qt::Key_Escape :
            QGuiApplication::exit(EXIT_SUCCESS);
            break;
        case Qt::Key_Space :
            m_player->SetAngularVelocity(0.f);
            m_player->SetLinearVelocity(b2Vec2(0.f,18.f));
            m_player->SetTransform(m_player->GetPosition(),15.f);
            break;
        default : break;
    }

    renderLater();
}


void NGLScene::collision()
{
    for(const auto &i : m_obstacles)
    {
        auto collision_dist = i->GetPosition().x - m_player->GetPosition().x;
        if (collision_dist <= 5)
            if (b2TestOverlap(m_player->GetFixtureList()->GetAABB(0),i->GetFixtureList()->GetAABB(0)))
                reset();
    }
}


void NGLScene::reset()
{
    m_player->SetTransform(b2Vec2((leftSideScreen-rightSideScreen)*0.15,0),0);
    m_player->SetAngularVelocity(0.f);
    m_player->SetLinearVelocity(b2Vec2(0,8.f));

    auto size = m_obstacles.size();
    for(std::array<b2Body*,6>::size_type i=0; i<size; i+=2)
    {
        randomHeightPos = distribution(generator);
        m_obstacles[i]->SetTransform(b2Vec2(rightSideScreen+(i*spaceBetweenObstacles),(height()*0.025)+randomHeightPos),0);
        m_obstacles[i+1]->SetTransform(b2Vec2(rightSideScreen+(i*spaceBetweenObstacles),-(height()*0.025)+randomHeightPos),0);
    }
}


void NGLScene::timerEvent( QTimerEvent *_event)
{
    m_world->Step((1.f/60.f),6,2);

    collision();

    for(const auto &i : m_obstacles)
    {
        if (i->GetPosition().x < leftSideScreen)
        {
            randomHeightPos = distribution(generator);
            i->SetTransform(b2Vec2(rightSideScreen+(spaceBetweenObstacles-4),i->GetPosition().y+randomHeightPos),0);
        }
        else
        {
            i->SetTransform(b2Vec2(i->GetPosition().x-0.1f,i->GetPosition().y),0);
        }
    }

    if ((m_player->GetPosition().y >= height()*0.03) || (m_player->GetPosition().y <= -(height()*0.03)))
        reset();

    renderLater();
}


void NGLScene::rtfm()
{
    pid_t pid;
    pid=fork();
    if (pid==0)
        execl("/opt/google/chrome/google-chrome",".","http://box2d.org/manual.pdf", NULL);
}




