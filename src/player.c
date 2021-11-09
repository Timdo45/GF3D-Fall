#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "SDL_timer.h"


Vector3D offset;
float playerStamina = 100;
float player_maxHealth = 100;
float playerHealth = 100;
void player_think(Entity *self);
void player_update(Entity *self);





void player_dodgeroll(Entity* self) {
    if (!self)return;
    const Uint8* keys;
    slog("dodgerolled");
    keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_D]) {
        self->position.x += 5;
        slog("dodgerolled right");
        return;
    }
    if (keys[SDL_SCANCODE_A]) {
        self->position.x -= 5;
        slog("dodgerolled left");
        return;
    }
    return;
}
AABB player_make_AABB(Entity* self) {
    vector3d_add(self->box->max,self->position,vector3d(10,10,10));
    vector3d_add(self->box->min, self->position, vector3d(-10, -10, -10));

    
}
void onDeath(Entity* self) {
    if (!self)return;
}

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("dino");
    ent->think = player_think;
    ent->update = player_update;
    ent->physics = player_physics;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -M_PI;
    slog("player health is: (%f)" , playerHealth);
    vector3d_add(ent->min,ent->position, vector3d(-5, -5, -5));
    vector3d_add(ent->max, ent->position, vector3d(5, 5, 5));
    gfc_matrix_slog(ent->modelMat);

    
    return ent; 
}
void on_hit(Entity* other) {
    other->health - 10;
    slog("hit");
    return;
}
void player_attack(Entity* self) {
    Vector3D temp1;
    Vector3D temp2;
    Vector3D temp3;
    Entity *other;
    EntityManager *thing = get_Entity_Manager_List();
    slog("attacked");
    //if (!self)return;
    int i;
    vector3d_copy(temp1, self->position);
    vector3d_copy(temp2, self->min);
    vector3d_copy(temp3, self->max);
    self->position.y += 1;
    self->min.y = self->position.y + 1;
    self->max.y = self->position.y + 1;
    for (i = 0; i < thing->entity_count; i++) {
       
        other = &thing->entity_list[i];
        if (self == other)
            continue;
        if (check_intersect(self->min, self->max, other->min, other->max)) {
            slog("hit");
            slog("count: (%i)", i);
            on_hit(other);
        }

    }
    vector3d_copy(self->position,temp1);
    vector3d_copy(self->min,temp2);
    vector3d_copy(self->max,temp3);




}

void player_think(Entity *self)
{
    
    Vector3D forward;
    Vector3D right;
    Vector3D up;
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    vector3d_angle_vectors(self->rotation, &forward, &right, &up);
    vector3d_set_magnitude(&forward,0.5);
    vector3d_set_magnitude(&right,0.5);
    vector3d_set_magnitude(&up,0.5);

    if (keys[SDL_SCANCODE_D])
    {   
        vector3d_add(self->position, self->position, forward);
        vector3d_add(self->max, self->position, forward);
        vector3d_add(self->min, self->position, forward);
    }
    if (keys[SDL_SCANCODE_A])
    {
        vector3d_add(self->position,self->position,-forward);
        vector3d_add(self->max, self->position, -forward);
        vector3d_add(self->min, self->position, -forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_add(self->position,self->position,right);
        vector3d_add(self->max, self->position, right);
        vector3d_add(self->min, self->position, right);
    }
    if (keys[SDL_SCANCODE_W])    
    {
        vector3d_add(self->position,self->position,-right);
        vector3d_add(self->max, self->position, -right);
        vector3d_add(self->min, self->position, -right);
    }
    if (keys[SDL_SCANCODE_SPACE])
    {
        self->position.z += 0.10;
        self->min.z += 0.10;
        self->max.z += 0.10;
        
    }
    if (keys[SDL_SCANCODE_Z]) { 
        self->position.z -= 0.10;
        self->min.z -= 0.10;
        self->max.z -= 0.10;
    }
    
    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0010;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0010;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z -= 0.0010;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z += 0.0010;

    if (keys[SDL_SCANCODE_LSHIFT]) {
        player_dodgeroll(self);
        slog("AABB position max is: (%f) x (%f) y (%f) z", self->max.x, self->max.y, self->max.z);
    }
    if (keys[SDL_SCANCODE_Q])
    {
        self->position = vector3d(0, 0, 0);
        self->min = vector3d(0, 0, 0);
        self->max = vector3d(0, 0, 0);
    }
    if (keys[SDL_SCANCODE_RSHIFT]) {
        player_attack(self);
    }

}

void player_update(Entity *self)
{
    
    offset.x = self->position.x;
    offset.y = self->position.y - 50;
    offset.z = self->position.z + 10;
    if (!self)return;
    
    gf3d_camera_set_position(offset);
    gf3d_camera_set_rotation(self->rotation);
}
void player_physics(Entity *self, Entity *other, World *w)
{
    if (!self || !other)return;
    if (check_intersect(self->min, self->max, other->min, other->max))
    {
        //SDL_Rect;
        
        return;
    }
    if(!check_intersect(self->min,self->max,w->min,w->max)){
        
        if (self->max.x > w->max.x)
            self->position.x = 99;
        if (self->max.y > w->max.y)
            self->position.y = 99;
        if (self->max.z > w->max.z)
            self->position.z = 199;
        if (self->min.x < w->min.x)
            self->position.x = -99;
        if (self->min.y < w->min.y)
            self->position.y = -99;
        if (self->min.z < w->min.z)
            self->position.z = 1;
    }
}


/*eol@eof*/
