#ifndef __AGUMON_H__
#define __AGUMON_H__

#include "entity.h"

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity *agumon_new(Vector3D position);
void on_Death(Entity* self, Entity* inflictor);
void agumon_attack(Entity* self);
#endif
