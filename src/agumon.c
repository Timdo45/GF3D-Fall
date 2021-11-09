
#include "simple_logger.h"
#include "agumon.h"
#include "player.h"


void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
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


void agumon_think(Entity* self)
{

    if (!self)return;
    self->rotation.x += -0.002;
}
void on_Death(Entity* self, Entity *inflictor) {
        slog("agumon has died");
        entity_free(self);

}

/*eol@eof*/
