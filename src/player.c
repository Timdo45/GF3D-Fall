#include "simple_logger.h"
#include "simple_json.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "SDL_timer.h"


Vector3D offset;
float playerStamina = 100;
float player_maxStamina = 100;
float player_maxHealth = 100;
float playerHealth = 100;
float player_exp=0;
float level=1;
float exp_tolevel = 10;
void player_think(Entity *self);
void player_update(Entity *self);





void player_dodgeroll(Entity* self) {
    if (!self)return;
    float stamina_required = 25;
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL);
    self->recoveryFrames = 100;
    if (!check_Stamina(stamina_required,playerStamina))
        return;
    if (keys[SDL_SCANCODE_D]) {
        self->position.x += 25;
        slog("dodgerolled right");
        playerStamina -= stamina_required;
        return;
    }
    if (keys[SDL_SCANCODE_A]) {
        self->position.x -= 25;
        slog("dodgerolled left");
        playerStamina -= stamina_required;
        return;
    }
    return;
}
void regen_stamina() {
    if (playerStamina < player_maxStamina) {
        playerStamina += 0.5;
        if (playerStamina > player_maxStamina) {
            playerStamina = player_maxStamina;
        }
    }
}
Bool check_Stamina(float Stamina_Required, float CurrentStam) {
    if (Stamina_Required > CurrentStam) {
        slog("too little stamina");
        return false;
    }
    else return true;
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
    ent->_isplayer = 1; 
    ent->rotation.x = -M_PI;
    ent->health = playerHealth;
    slog("player health is: (%f)" , playerHealth);
    vector3d_add(ent->min,ent->position, vector3d(-5, -5, -5));
    vector3d_add(ent->max, ent->position, vector3d(5, 5, 5));
    //gfc_matrix_slog(ent->modelMat);

    
    return ent; 
}
void on_hit(Entity *self,Entity* other) {
    other->health -= 20;
    slog("enemy health: (%i)", other->health);
    if (other->health <= 0) {
        other->onDeath(other, self);
        gain_exp(self, 5);
        slog("Current EXP: (%f)", player_exp);
    }
    else
        slog("thing");
    return;
}
void on_heavy_hit(Entity *self,Entity* other) {
    other->health -= 50;
    if (other->health <= 0) {
        other->onDeath(other, self);
        gain_exp(self, 5);
        slog("Current EXP: (%f)", player_exp);
    }
    return;
}
void level_up(Entity *self) {
    level += 1;
    player_maxHealth += 10;
    player_maxStamina += 10;
    slog("LEVEL UP");
    slog("Current Level: (%f) ", level);
    slog("Current Max Health: (%f)", player_maxHealth);
    slog("Current Health: (%i)", self->health);
    slog("MAX STAMINA: (%f)", player_maxStamina);
}
void gain_exp(Entity* self, float newexp) {
    player_exp += newexp;
    if (player_exp >= exp_tolevel) {
        level_up(self);
    }
}
void player_attack(Entity* self) {
    Vector3D temp1;
    Vector3D temp2;
    Vector3D temp3;
    Entity *other;
    float stamina_required = 25;
    self->recoveryFrames = 30;
    EntityManager *thing = get_Entity_Manager_List();
    slog("attacked");
    if (!self)return;
    if (!check_Stamina(stamina_required, playerStamina))
        return;
    int i;
    vector3d_copy(temp1, self->position);
    vector3d_copy(temp2, self->min);
    vector3d_copy(temp3, self->max);
    self->position.y += 5;
    self->min.y = self->position.y + 5;
    self->max.y = self->position.y + 5;
    for (i = 0; i < thing->entity_count; i++) {
       
        other = &thing->entity_list[i];
        if (self == other)
            continue;
        if (check_intersect(self->min, self->max, other->min, other->max)) {
            slog("hit");
            slog("count: (%i)", i);
            on_hit(self,other);
        }

    }
    playerStamina -= stamina_required;
    vector3d_copy(self->position,temp1);
    vector3d_copy(self->min,temp2);
    vector3d_copy(self->max,temp3);




}
void player_heavy_attack(Entity* self) {
    Vector3D temp1;
    Vector3D temp2;
    Vector3D temp3;
    Entity* other;
    float stamina_required = 50;
    self->recoveryFrames = 200;
    EntityManager* thing = get_Entity_Manager_List();
    slog("attacked heavily");
    if(!self)return;
    if (!check_Stamina(stamina_required, playerStamina))
        return;
    int i;
    vector3d_copy(temp1, self->position);
    vector3d_copy(temp3, self->max);
    
    self->max.y = self->position.y + 20;
    for (i = 0; i < thing->entity_count; i++) {

        other = &thing->entity_list[i];
        if (self == other)
            continue;
        if (check_intersect(self->min, self->max, other->min, other->max)) {
            slog("heavy hit");
            on_heavy_hit(self,other);
        }

    }
    playerStamina -= stamina_required;
    slog("Current Stamina: (%f)", playerStamina);
    self->max = temp3;
    
}
Bool canThink(Entity* self) {
    if (self->recoveryFrames == 0) {
        return true;
    }
    else
        return false;
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
    if (self->recoveryFrames != 0) {
        slog("can't act");
        slog("recovery Frames: %i", (int)self->recoveryFrames);
        return;
    }
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
        slog("Current Stamina: (%f)", playerStamina);
        player_dodgeroll(self);
        
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
    if (keys[SDL_SCANCODE_F]) {
        player_heavy_attack(self);
    }

}

void player_update(Entity *self)
{
    Uint8 health;
    offset.x = self->position.x;
    offset.y = self->position.y - 50;
    offset.z = self->position.z + 10;
    if (!self)return;
    regen_stamina(); 
    gf3d_camera_set_position(offset);
    gf3d_camera_set_rotation(self->rotation);
    health = self->health;
    if (health <= 0) {
        slog("player has died");
        entity_free(self);
    }
    if (self->recoveryFrames > 0) {
        self->recoveryFrames -= 1;
        if (self->recoveryFrames < 0)
            self->recoveryFrames = 0;
    }
    
}
void player_physics(Entity* self, Entity* other, World* w)
{
    if (!self || !other)return;
    if (check_intersect(self->min, self->max, other->min, other->max))
    {
        slog("collided with entity");
        
        
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
