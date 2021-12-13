#include "goldsack.h"
int goldValue;
Entity* goldsack_new(Vector3D position, int value)
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
    ent->think = goldsack_think;
    ent->_isgoldsack = 1;
    goldValue = value;
    ent->onDeath = goldsack_Consume;
    vector3d_copy(ent->position, position);
    vector3d_add(ent->min, ent->position, vector3d(-5, -5, -5));
    vector3d_add(ent->max, ent->position, vector3d(5, 5, 5));
    return ent;
}
void goldsack_think(Entity* self) {
    
}
void goldsack_Consume(Entity *self, Entity* consumer) {
    slog("got (%i) gold", goldValue);
    entity_free(self);
}
int goldsack_get_goldValue() {
    return goldValue;
}