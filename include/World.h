#ifndef __WORLD_H___
#define __WORLD_H___

#pragma once
typedef struct
{
	Model* worldModel;
	List* spawnList;
	List* entityList;

}World;
World* world_load(char* filename);
void world_draw(World* world);
void world_delete(World* world);
void world_run_updates(World* world);
void world_add_entity(World* world);
#endif