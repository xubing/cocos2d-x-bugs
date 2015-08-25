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
    
    
    // The Issue
    //
    // There does not seem to be a way to make UI HUD elements such as the MenuItemImage only touchable
    // by coordinates touched in the default camera. Or for specific alternative cameras.
    //
    // This issue seems to be a result of the EventDispatcher::dispatchTouchEventToListeners cycling
    // through all cameras on line 834 of CCEventDispatcher.cpp of Cocos2d-x 3.8 beta 0.
    //
    // e.g. touch the same coordinates of the MenuItemImage in the view of camera 1 and the button
    // will be pressed. This creates a problem if world node has the same coordiantes as the hud node.
    // Maybe the solution is to not let the coordinates overlap?
    //
    // Or maybe for efficiency reasons and to fix the above issue, a filter could be applied somehow to
    // only forward events from cameras that the API user selects.
    //
    // Another way of describing the issue:
    // The issue is that that MenuItemImage by default (all Nodes do) has the CameraFlag::DEFAULT
    // mask. This may stop the button from being visible in camera 1. However, if the rect that is
    // defined for the button in the default camera is touched in camera 1, an event is dispatched
    // as if the button were pressed in the defualt camera's view.
    //
    // This issue can be demonstrated by this code by touching the HelloWorld.png sprite and observe
    // that the button is being presssed and released.
    //
    // The HelloWorld.png sprite is added to a world node that has a custom camera as described below.
    //

    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    closeItem->setScale(4.0);
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width * 0.5 ,
                                origin.y + visibleSize.height * 0.5));

    
    Node * hud = Node::create();
    this->addChild(hud);
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    hud->addChild(menu, 1);
    
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    this->addChild(label, 1);

    Vec2 screenCenter = Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y);
    
    auto sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(screenCenter);

    // Create the world Node that will get its own camera.
    Node * worldContainer = Node::create();

    // add the sprite as a child of the world container.
    worldContainer->addChild(sprite, 0);
    
    // Create camera 1 for the worldContainer, zoom it out to 400 points away from the HelloWorld.png
    // sprite, and also shift the camera over to the left a distance of 1.5 screen widths. But since
    // the camera is zoomed out, the HelloWorld.png sprite will be small and on the visible screen to
    // the elft of the button.
    Vec3 cameraStartPosition3D = Vec3(1.5 * screenCenter.x , screenCenter.y,0);
    this->addChild(worldContainer);
    worldContainer->setCameraMask((unsigned short)CameraFlag::USER1, true);
    float openGLFieldOfView = 60;
    float openGLAspectRatio = visibleSize.width/visibleSize.height;
    Camera * worldCamera = Camera::createPerspective(openGLFieldOfView, openGLAspectRatio, 1.0, 10000);
    worldCamera->setCameraFlag(CameraFlag::USER1);
    worldCamera->setPosition3D(cameraStartPosition3D + Vec3(0,0,400));
    worldCamera->lookAt(cameraStartPosition3D , Vec3(0.0,1.0,0.0));
    worldContainer->addChild(worldCamera);
    worldContainer->setGlobalZOrder(1000);
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("zoom1.wav");
}
