#include "PhotonConsole.h"
#include "cocos2d.h"
#include<string>
using namespace std;
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
PhotonConsole::PhotonConsole(void)
	: mImp(*reinterpret_cast<ConsoleImplementation*>(NULL))
{}

PhotonConsole::~PhotonConsole(void)
{}

void PhotonConsole::update(void)
{}

void PhotonConsole::write(const ExitGames::Common::JString & str)
{
  std::string log = str.UTF8Representation().cstr();
  CCLOG("PHOTON: %s", log.c_str());
}

void PhotonConsole::writeLine(const ExitGames::Common::JString& str)
{
  std::string log = str.UTF8Representation().cstr();
  CCLOG("PHOTON: %s", log.c_str());
}


void PhotonConsole::log(std::string& what)
{
	CCLOG("cclog# %s",what.c_str());
}



PhotonConsole& PhotonConsole::get(void)
{
	static PhotonConsole console;
	return console;
}
//#endif
