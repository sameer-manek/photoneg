#include "FirstScene.h"

USING_NS_CC;

Scene* FirstScene::createScene()
{
    return FirstScene::create();
}

void FirstScene::onEnter ()
{
    Scene::onEnter();
    initPlayers();
    this->renderScene();
}

void FirstScene::initPlayers (void)
{
  // fetch all the connected people and update count
  this->count = 0;
}

void FirstScene::addPlayer(bool local)
{
  
  this->count++;
  count_text->setString(std::to_string(this->count));
  if (!local) CCLOG("Broadcasting event");
}

void FirstScene::renderScene ()
{
    CCLOG("CROW: rendering text");
    auto dp_size = Director::getInstance()->getVisibleSize();
    
    count_text = cocos2d::ui::Text::create(std::to_string(this->count), "fonts/Marker Felt.ttf", 22);
    count_text->setAnchorPoint(Vec2(0.5, 0.5));
    count_text->setTextColor(Color4B::WHITE);
    auto text_pos = Vec2(dp_size.width / 2, dp_size.height / 2);
    count_text->setPosition(Vec2(text_pos.x, text_pos.y + 50));

    auto inc_btn = cocos2d::ui::Button::create();
    inc_btn->setAnchorPoint(Vec2(0.5, 0.5));
    inc_btn->setTitleText("connect");
    inc_btn->setPosition(Vec2(text_pos.x, text_pos.y));
    inc_btn->loadTextureNormal("btn.png");
    
    inc_btn->addTouchEventListener(
	[this, inc_btn] (Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
	  if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
	  {
	    // increment local count and send signal
	    this->addPlayer(true);
	    inc_btn->setTouchEnabled(false);
	    inc_btn->setBright(false);
	    inc_btn->setTitleText("connected");
	  }
        }
    );

    this->addChild(count_text);
    this->addChild(inc_btn);
}

void FirstScene::onExit ()
{
    // DO SOMETHING
}
