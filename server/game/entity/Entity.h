#ifndef H_ENTITY
#define H_ENTITY
typedef struct {
    int* absX;
    int* absY;
    int* absZ;
    int* homeX;
    int* homeY;
    int* homeZ;
    int* walkable;
    char* name;
} Entity;

Entity* newEntity();
/* initEntity(e,x,y):
    @param e: entity to initialize
    @param x,y,z: coordinates to place the entity
    Entity will be set to unwalkable by default
*/
void initEntity(Entity* e, int x, int y, int z);
void deleteEntity(Entity* e);

/* setEntityCoordinates(e,x,y):
    @param e: entity to move
    @param x,y,z: new coordinates to change to
*/
void setEntityCoordinates(Entity* e, int x, int y, int z);
/* setEntityName:
    @param e: entity to rename
    @param nom: text to rename to
*/
void setEntityName(Entity* e, char* nom);
#endif
