
#include "simple_logger.h"
#include "agumon.h"
#include "player.h"
#include "AABB_collisions.h"


void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position)
{
    Entity *ent = NULL;
    EntityManager *list=get_Entity_Manager_List();
    int i;
    Entity* target;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    for (i = 0; i < list->entity_count; i++) {
        target = &list->entity_list[i];
        if (target->_isplayer==1) {
            ent->target = target;
        }
        else continue;
    }
    ent->model = gf3d_model_load("dino");
    ent->think = agumon_think;
    ent->health = 100;
    ent->onDeath = on_Death;
    ent->exp_Points = 5;
    vector3d_copy(ent->position,position);
    vector3d_add(ent->min, ent->position, vector3d(-15, -15, -15));
    vector3d_add(ent->max, ent->position, vector3d(15, 15, 15));
    return ent;
}
void move_toPlayer(Entity* self) {
    if (!self)return;
    self->rotation.x += -0.002;
    if (self->position.x > self->target->position.x) {
        self->position.x -= 0.005;
    }
    if (self->position.x < self->target->position.x) {
        self->position.x += 0.005;
    }
    if (self->position.y > self->target->position.y) {
        self->position.y -= 0.005;
    }
    if (self->position.y < self->target->position.y) {
        self->position.y += 0.005;
    }
    if (self->position.z > self->target->position.z) {
        self->position.z -= 0.005;
    }
    if (self->position.z < self->target->position.z) {
        self->position.z += 0.005;
    }
    vector3d_add(self->min, self->position, vector3d(-15, -15, -15));
    vector3d_add(self->max, self->position, vector3d(15, 15, 15));
}


void agumon_think(Entity* self)
{

    if (!self)return;
    self->rotation.x += -0.002;
    move_toPlayer(self);
    agumon_attack(self);

}
void on_Death(Entity* self, Entity *inflictor) {
        slog("agumon has died");
        entity_free(self);

}
void agumon_attack(Entity* self) {
    if (check_intersect(self->min, self->max, self->target->min, self->target->max)) {
        slog("player has been hit");
        self->target->health -= 10;  
   }
}

/*eol@eof*/
