#ifndef _PHOTON_CONSOLE_H
#define _PHOTON_CONSOLE_H
#include "cocos2d.h"

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Demos/shared/inc/OutputListener.h"

class ConsoleImplementation;

class PhotonConsole : public OutputListener
{
public:
  PhotonConsole(void);
  ~PhotonConsole(void);
  void update(void);
  void write(const ExitGames::Common::JString& str);
  void writeLine(const ExitGames::Common::JString& str = ExitGames::Common::JString());
	void log(std::string& string);
  static PhotonConsole& get(void);
private:
  ConsoleImplementation& mImp;
};
//#endif

#endif // !_PHOTON_CONSOLE_H
