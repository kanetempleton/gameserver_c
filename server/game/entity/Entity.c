#include "Entity.h"
#include <stdlib.h>
#include <string.h>

Entity* newEntity() {
    return (Entity*)malloc(sizeof(Entity));
}
/* initEntity(e,x,y):
    @param e: entity to initialize
    @param x,y,z: coordinates to place the entity
    Entity will be set to unwalkable by default
*/
void initEntity(Entity* e, int x, int y, int z) {
    e->absX = malloc(sizeof(int));
    e->absY = malloc(sizeof(int));
    e->absZ = malloc(sizeof(int));
    e->homeX = malloc(sizeof(int));
    e->homeY = malloc(sizeof(int));
    e->homeZ = malloc(sizeof(int));
    e->walkable = malloc(sizeof(int));
    e->name = malloc(50);
    *(e->absX) = x;
    *(e->absY) = y;
    *(e->absZ) = z;
    *(e->homeX) = x;
    *(e->homeY) = y;
    *(e->homeZ) = z;
    *(e->walkable) = 1;
}

void deleteEntity(Entity* e) {
    free(e->absX);
    free(e->absY);
    free(e->absZ);
    free(e->homeX);
    free(e->homeY);
    free(e->homeZ);
    free(e->walkable);
}

/* setEntityCoordinates(e,x,y):
    @param e: entity to move
    @param x,y,z: new coordinates to change to
*/
void setEntityCoordinates(Entity* e, int x, int y, int z) {
    *(e->absX) = x;
    *(e->absY) = y;
    *(e->absZ) = z;
}

void setEntityName(Entity* e, char* nom) {
    strcpy(e->name,nom);
}
