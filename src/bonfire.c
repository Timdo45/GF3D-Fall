#include "simple_logger.h"
#include "simple_json.h"
#include "player.h"
#include "agumon.h"
#include "world.h"
#include "bonfire.h"
#include "AABB_collisions.h"



Entity* bonfire_new(Vector3D position, char* filename)
{
    Entity* ent = NULL;
    EntityManager* list = get_Entity_Manager_List();
    int i;
    SJson json, mjson;
    Entity* target;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no bonfire for you!");
        return NULL;
    }
    ent->model = gf3d_model_load("cube");
    ent->think = bonfire_think;
    ent->_isbonfire = 1;
    vector3d_copy(ent->position, position);
    vector3d_add(ent->min, ent->position, vector3d(-5, -5, -5));
    vector3d_add(ent->max, ent->position, vector3d(5, 5, 5));
    return ent;
}
void bonfire_think(Entity *self) {

}
void bonfire_load_all(char* worldFile, char* monsterFile) {
    SJson sjson1, sjson2, pjson, mjson;

}
