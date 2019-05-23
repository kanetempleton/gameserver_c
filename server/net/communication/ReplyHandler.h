#ifndef H_REPLYHANDLER
#define H_REPLYHANDLER
#include "../server.h"
char* getReply(Server* s, int from, char* recv);
void registerPlayer(int fd, int id, Player* p);
#endif
