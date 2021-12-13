#ifndef __BONFIRE_H__
#define __BONFIRE_H__

#include "entity.h"
Entity* bonfire_new(Vector3D position, char* filename);
void bonfire_think(Entity* self);
void bonfire_load_all(char* worldFile, char* monsterFile);
#endif