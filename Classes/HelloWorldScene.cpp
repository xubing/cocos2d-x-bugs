#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    _screenSize = Director::getInstance()->getVisibleSize();
    _previousTouchLocation = _touchLocation = _halfScreenSizeVect = Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5);
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + _screenSize.width/2,
                            origin.y + _screenSize.height - label->getContentSize().height));
    this->addChild(label, 1);

    Vec2 screenCenter = Vec2(_screenSize.width/2 + origin.x, _screenSize.height/2 + origin.y);
    
    _sprite = Sprite::create("HelloWorld.png");
    _sprite->setPosition(screenCenter);

    // Create the world Node that will get its own camera.
    _worldContainer = Node::create();

    // add the sprite as a child of the world container.
    _worldContainer->addChild(_sprite, 0);
    
    _zoomValue = 100;
    _zoomDirection = 1.0;
    _zoomStepSize = 3.0;
    
    _moveValue = _sprite->getPositionX();
    _moveDirection = 1.0;
    _moveStepSize = 5.0;

    Vec3 cameraStartPosition3D = Vec3(_sprite->getPosition3D());
    this->addChild(_worldContainer);
    _worldContainer->setCameraMask((unsigned short)CameraFlag::USER1, true);
    float openGLFieldOfView = 60;
    _openGLAspectRatio = _screenSize.width/_screenSize.height;
    float nearPlane = 1;
    float farPlane = 1500.0;
    _worldCamera = Camera::createPerspective(openGLFieldOfView, _openGLAspectRatio, nearPlane, farPlane);
    _worldCamera->setCameraFlag(CameraFlag::USER1);
    _worldCamera->setPosition3D(cameraStartPosition3D + Vec3(0,0,_zoomValue));
    _worldCamera->lookAt(cameraStartPosition3D , Vec3(0.0,1.0,0.0));
    _worldContainer->addChild(_worldCamera);
    _worldContainer->setGlobalZOrder(1000);
   
    _zoomFarPlaneHeightFactor = tan(CC_DEGREES_TO_RADIANS(openGLFieldOfView) * 0.5); // For a field of view of 60 degrees, this would be 0.57735

    this->setupTouchControls();
    
    this->schedule(schedule_selector(HelloWorld::update));
    
    return true;
}

void HelloWorld::onExit()
{
    Layer::onExit();
    _eventDispatcher->removeEventListener(_touchEventListener);
}

void HelloWorld::update(float dt)
{
    _zoomValue += _zoomDirection * _zoomStepSize;
    
    
    if (_zoomValue > 1000)
    {
        _zoomDirection = -1.0;
    }
    else if (_zoomValue < 100)
    {
        _zoomDirection = 1.0;
    }

    _moveValue += _moveDirection * _moveStepSize;

    if (_moveValue > 800)
    {
        _moveDirection = -1.0;
    }
    else if (_moveValue < -800)
    {
        _moveDirection = 1.0;
    }

    Vec3 currentPosition3D = _worldCamera->getPosition3D();
    Vec2 currentPosition2D = _worldCamera->getPosition();

    //if (_previousTouchLocation != _touchLocation)
    {
        _previousTouchLocation = _touchLocation;
        float gamePlaneHalfHeight = _zoomValue * _zoomFarPlaneHeightFactor;
        float gamePlaneHalfWidth = _openGLAspectRatio * gamePlaneHalfHeight;
        
        Vec2 touchLocationOffsetFromCenter = (_touchLocation - _halfScreenSizeVect);
        Vec2 touchLocationNormal = Vec2(touchLocationOffsetFromCenter.x / _halfScreenSizeVect.x,
                                        touchLocationOffsetFromCenter.y / _halfScreenSizeVect.y);
        
        Vec2 mappedTouchLocationOffset = Vec2(gamePlaneHalfWidth * touchLocationNormal.x, gamePlaneHalfHeight * touchLocationNormal.y);
        Vec2 mappedTouchLocation = currentPosition2D + mappedTouchLocationOffset;
        _sprite->setPosition(mappedTouchLocation);
        
        CCLOG("touchLocation:%.0f,%.0f  touchLocationNormal:%.2f,%.2f touchLocationOffsetFromCenter:%.0f,%.0f Touch:%.0f,%.0f => World:%.0f,%.0f  distantPlane:%.0f,%.0f",
              _touchLocation.x, _touchLocation.y,
              touchLocationNormal.x, touchLocationNormal.y,
              touchLocationOffsetFromCenter.x, touchLocationOffsetFromCenter.y,
              _touchLocation.x, _touchLocation.y,
              mappedTouchLocation.x, mappedTouchLocation.y,
              2 * gamePlaneHalfHeight, 2 * gamePlaneHalfWidth);
    }
    
    _worldCamera->setPosition3D(Vec3(_moveValue, currentPosition3D.y, 0) + Vec3(0,0,_zoomValue));
    _worldCamera->lookAt(Vec3(_moveValue, currentPosition3D.y, 0) , Vec3(0.0,1.0,0.0)); // Assumes all sprites are on the z=0 plane.
}

void HelloWorld::setupTouchControls()
{
    _touchEventListener = EventListenerTouchAllAtOnce::create();
    
    _touchEventListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event *event)
    {
        for (auto touch : touches)
        {
            _touchLocation = touch->getLocation();
            CCLOG("Start Touch:%.0f,%.0f", _touchLocation.x, _touchLocation.y);
        }
        
    };
    
    _touchEventListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event *event)
    {
        for (auto touch : touches)
        {
            _touchLocation = touch->getLocation();
            CCLOG("Move Touch:%.0f,%.0f", _touchLocation.x, _touchLocation.y);
        }
    };
    
    _touchEventListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event *event)
    {
        for (auto touch : touches)
        {
            cocos2d::Vec2 location = touch->getLocation();
            CCLOG("End Touch:%.0f,%.0f", location.x, location.y);
        }
    };
    
    _touchEventListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event *event)
    {
        for (auto touch : touches)
        {
            cocos2d::Vec2 location = touch->getLocation();
            CCLOG("Cancel Touch:%.0f,%.0f", location.x, location.y);
        }
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchEventListener, this);
}
