#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void onExit();
    CREATE_FUNC(HelloWorld);
    
private:
    void setupTouchControls();
    void update(float dt);
    
    cocos2d::EventListenerTouchAllAtOnce    * _touchEventListener;
    Node                                    * _worldContainer;
    cocos2d::Camera                         * _worldCamera;
    float                                   _zoomValue;
    float                                   _zoomStepSize;
    float                                   _zoomDirection;
    
    float                                   _moveValue;
    float                                   _moveStepSize;
    float                                   _moveDirection;
    float                                   _zoomFarPlaneHeightFactor;
    float                                   _openGLAspectRatio;
    cocos2d::Vec2                           _touchLocation;
    cocos2d::Vec2                           _previousTouchLocation;
    cocos2d::Sprite                         * _sprite;
    cocos2d::Size                           _screenSize;
    cocos2d::Vec2                           _halfScreenSizeVect;
};

#endif // __HELLOWORLD_SCENE_H__
