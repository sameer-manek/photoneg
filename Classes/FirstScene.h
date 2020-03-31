#ifndef CLASSES_FIRSTSCENE_H_
#define CLASSES_FIRSTSCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "BaseView.h"
#include "string"

USING_NS_CC;

class FirstScene : public cocos2d::Scene, public BaseView
{
private:
  int count;
  cocos2d::ui::Text *count_text;

public:
  static cocos2d::Scene* createScene();
  virtual void initPlayers(void);
  virtual void addPlayer(bool local = false);
  virtual void onEnter();
  virtual void onExit();
  void renderScene();

  // implement the "static create()" method manually
  CREATE_FUNC(FirstScene);
};

#endif // CLASSES_FIRSTSCENE_H_
