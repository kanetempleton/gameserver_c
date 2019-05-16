#ifndef H_CMD
#define H_CMD
#include "../server.h"
typedef struct {
    Server * linked;
} CMD;
CMD * newCMD();
void initCMD(CMD* me,Server* s);
void deleteCMD(CMD * me);
void * listenForCommands(void * arg);
#endif
