#ifndef __BONFIRE_H__
#define __BONFIRE_H__


#include "simple_logger.h"
#include "simple_json.h"
#include "player.h"
#include "agumon.h"
#include "entity.h"




Entity* goldsack_new(Vector3D position, int value);
void goldsack_think(Entity* self);
int goldsack_get_goldValue();
void goldsack_Consume(Entity* target);
#endif
