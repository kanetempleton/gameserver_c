#ifndef H_SENDMESSAGE
#define H_SENDMESSAGE
#include "../server.h"
void registerPlayerWithID(Player* p,Server * s, int fd);
void sendPlayerCoordinatesToClient(Game* g,Player* p);
void sendPlayerDataToClient(Game* g,Player * p);
void sendInvalidLoginNotification(Player* p); //invalid logins
void sendNewPlayerData(Player* p); //new logins
void sendLogoutSignalToPlayer(Player* p);
void broadcastPlayerPresence(Player* p, int oldx, int oldy);
void sendPlayerPresenceTo(Player* pOf, Player* pTo, int oldx, int oldy);
void sendMapToPlayer(Game* g,Player* p, int mapNum, char* data);
void sendPlayerExitTo(Player* pTo, int ofID, int fromX, int fromY);
void sendInfoMessage(Player* p, char* msg);
void sendPublicChatOfPlayerTo(Player* pOf, Player* pTo, char* msg);
#endif
