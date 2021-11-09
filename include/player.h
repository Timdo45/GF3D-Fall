#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */
Entity *player_new(Vector3D position);

void player_physics(Entity * self, Entity* other, World *w);

void player_think(Entity* self);
void player_update(Entity* self);
void player_dodgeroll(Entity* self);
void regen_stamina();
Bool check_Stamina(float Stamina_Required, float CurrentStam);
void onDeath(Entity* self);
void on_hit(Entity* other);
void on_heavy_hit(Entity* other);
void level_up(Entity* self);
void gain_exp(Entity* self, float newexp);
void player_attack(Entity* self);
void player_heavy_attack(Entity* self);
void player_physics(Entity* self, Entity* other, World* w);



#endif
