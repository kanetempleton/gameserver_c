#ifndef H_MAPDATA
#define H_MAPDATA
void getMapStringForChunk(int mapid, char* copyTo);
int getTileAt(int x, int y);
int tileWalkable(int tile);
#endif
