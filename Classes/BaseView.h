#ifndef CLASSES_BASEVIEW_H_
#define CLASSES_BASEVIEW_H_

#include "Common-cpp/inc/Common.h"

class BaseView
{
public:
  virtual ~BaseView(void) {}
  // virtual void updateState(int state,
  //			      const ExitGames::Common::JString& stateStr,
  //			      const ExitGames::Common::JString& joinedRoomName) = 0;
  virtual void initPlayers(void) = 0;
  virtual void addPlayer(bool local = false) = 0; // TODO: add player info (eg. player_name)
	// virtual void removePlayer(int player_nr) = 0;
};

#endif // CLASSES_BASEVIEW_H_

