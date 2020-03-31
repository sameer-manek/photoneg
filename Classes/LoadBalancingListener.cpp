#ifdef __UNREAL__
#	include "PhotonDemoParticle.h"
#endif
#include <iostream>
#include <stdlib.h>
#include "Common-cpp/inc/UTF8String.h"
#include "Common-cpp/inc/MemoryManagement/Allocate.h"
#include "LoadBalancingListener.h"
#include "PhotonConsole.h"

#ifdef _EG_ANDROID_PLATFORM
#	include <stdarg.h>
#	include <android/log.h>
#endif

typedef PhotonConsole Console;

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

static int randomColor(int from=0, int to=256)
{
	int r = from + rand() % (to - from);
	int g = from + rand() % (to - from);
	int b = from + rand() % (to - from);
	return (r << 16) + (g << 8) + b;
}

const JString PeerStatesStr[] = {
	L"Uninitialized",
	L"PeerCreated",
	L"ConnectingToNameserver",
	L"ConnectedToNameserver",
	L"DisconnectingFromNameserver",
	L"Connecting",
	L"Connected",
	L"WaitingForCustomAuthenticationNextStepCall",
	L"Authenticated",
	L"JoinedLobby",
	L"DisconnectingFromMasterserver",
	L"ConnectingToGameserver",
	L"ConnectedToGameserver",
	L"AuthenticatedOnGameServer",
	L"Joining",
	L"Joined",
	L"Leaving",
	L"Left",
	L"DisconnectingFromGameserver",
	L"ConnectingToMasterserver",
	L"ConnectedComingFromGameserver",
	L"AuthenticatedComingFromGameserver",
	L"Disconnecting",
	L"Disconnected"
};

// TODO: update PeerStatesStr when PeerStates changes
// Checker below checks count match only
class PeerStatesStrChecker
{
public:
	PeerStatesStrChecker(void)
	{
		DEBUG_ASSERT(sizeof(PeerStatesStr)/sizeof(JString) == PeerStates::Disconnected + 1);
	}
} checker;


LoadBalancingListener::LoadBalancingListener(BaseView* pView) : mpLbc(NULL)
							      , mpView(pView)
{
}

LoadBalancingListener::~LoadBalancingListener(void)
{
	delete mpView;
}

void LoadBalancingListener::setLBC(ExitGames::LoadBalancing::Client* pLbc)
{
	this->mpLbc = pLbc;
}

void LoadBalancingListener::connect(const JString& userName)
{
	mpLbc->connect(AuthenticationValues().setUserID(JString()+GETTIMEMS()), userName);
}



void LoadBalancingListener::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void LoadBalancingListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"connection failed with error ") + errorCode);
	updateState();
}

void LoadBalancingListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + L" from client");
	updateState();
}

void LoadBalancingListener::warningReturn(int warningCode)
{
	Console::get().writeLine(JString(L"received warning ") + warningCode + " from client");
}

void LoadBalancingListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + " from server");
	updateState();
}

void LoadBalancingListener::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player)
{
	Console::get().writeLine(JString("player ") + player.getName() + L" has joined the game");
	mpView->addPlayer();
}

void LoadBalancingListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	if(isInactive)
		Console::get().writeLine(JString(L"player ") + playerNr + L" has suspended the game");
	else
	{
		Console::get().writeLine(JString(L"player ") + playerNr + L" has abandoned the game");
		// remove player
	}
}

void LoadBalancingListener::customEventAction(int playerNr, nByte eventCode, const Object& eventContentObj)
{
  // do something
}

void LoadBalancingListener::connectReturn(int errorCode,
					  const JString& errorString,
					  const JString& region,
					  const JString& cluster)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"connected to cluster " + cluster + L" of region " + region);
		mpLbc->opJoinRandomRoom();
	}
	else
		Console::get().writeLine(JString(L"Warn: connect failed ") + errorCode + L" " + errorString);
}

void LoadBalancingListener::disconnectReturn(void)
{
	updateState();
	Console::get().writeLine(L"disconnected");
	mpView->initPlayers();
}

void LoadBalancingListener::createRoomReturn(int localPlayerNr,
					     const Hashtable& gameProperties,
					     const Hashtable& playerProperties,
					     int errorCode,
					     const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been created");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opCreateRoom() failed: " + errorString);
}

void LoadBalancingListener::joinOrCreateRoomReturn(int localPlayerNr,
						   const Hashtable& gameProperties,
						   const Hashtable& playerProperties,
						   int errorCode,
						   const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been entered");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opJoinOrCreateRoom() failed: " + errorString);
}

void LoadBalancingListener::joinRoomReturn(int localPlayerNr,
					   const Hashtable& gameProperties,
					   const Hashtable& playerProperties,
					   int errorCode,
					   const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRoom() failed: " + errorString);
}

void LoadBalancingListener::joinRandomRoomReturn(int localPlayerNr,
						 const Hashtable& gameProperties,
						 const Hashtable& playerProperties,
						 int errorCode,
						 const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::NO_MATCH_FOUND)
		createRoom();
	else if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRandomRoom() failed: " + errorString);
}

void LoadBalancingListener::leaveRoomReturn(int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
		Console::get().writeLine(L"game room has been successfully left");
	else
		Console::get().writeLine(L"opLeaveRoom() failed: " + errorString);
	mpView->initPlayers();
}

void LoadBalancingListener::gotQueuedReturn(void)
{
	updateState();
}

void LoadBalancingListener::joinLobbyReturn(void)
{
	Console::get().writeLine(L"joined lobby");
	updateState();
}

void LoadBalancingListener::leaveLobbyReturn(void)
{
	Console::get().writeLine(L"left lobby");
	updateState();
}

void LoadBalancingListener::onLobbyStatsUpdate(const JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats update");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());

	// lobby stats request test
	JVector<LobbyStatsRequest> ls;
	ls.addElement(LobbyStatsRequest());
	ls.addElement(LobbyStatsRequest(L"AAA"));
	ls.addElement(LobbyStatsRequest(L"BBB"));
	ls.addElement(LobbyStatsRequest(L"CCC", LobbyType::DEFAULT));
	ls.addElement(LobbyStatsRequest(L"AAA", LobbyType::SQL_LOBBY));
	mpLbc->opLobbyStats(ls);
}

void LoadBalancingListener::onLobbyStatsResponse(const ExitGames::Common::JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats response");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());
}

void LoadBalancingListener::onRoomListUpdate()
{
	
}

void LoadBalancingListener::onRoomPropertiesChange(const ExitGames::Common::Hashtable& changes)
{
	
}

void LoadBalancingListener::updateState()
{
	int state = mpLbc->getState();
	// mpView->updateState(state, PeerStatesStr[state], state == PeerStates::Joined ? mpLbc->getCurrentlyJoinedRoom().getName() : JString());
}

void LoadBalancingListener::afterRoomJoined(int localPlayerNr)
{
	Console::get().writeLine(JString(L"afterRoomJoined: localPlayerNr=") + localPlayerNr);
}

void LoadBalancingListener::createRoom()
{
	JString name = JString(L"native-") + (rand()%100);
	Hashtable props;
	mpLbc->opCreateRoom(name, RoomOptions().setCustomRoomProperties(props));
	Console::get().writeLine(L"Creating room " + name);
}

void LoadBalancingListener::service(void)
{
  Console::get().writeLine(L"servicing room");
}
