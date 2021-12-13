#include "simple_logger.h"
#include "simple_json.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "bonfire.h"
#include "goldsack.h"
#include "SDL_timer.h"


Vector3D cameraPosition;
float playerStamina;
float player_maxStamina;
float player_maxHealth;
float playerHealth;
int player_exp;
int player_level;
int player_gold;
float isPlayer;
Vector3D playerPosition;
Uint8 canSave=0;


Uint8 isLockedOn = 0;
int exp_tolevel= 10;
Entity* target = NULL;
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
        playerStamina += 0.05;
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

Entity *player_new(Vector3D position, char* filename)
{
    Entity *ent = NULL;
    char* model1= NULL;
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    SJson *json, *pjson;
    json=sj_load(filename);
    if (!json)
    {
        slog("failed to load json file (%s) for the world data", filename);
        free(ent);
        return NULL;
    }
    pjson = sj_object_get_value(json, "player");
    if (!pjson)
    {
        slog("failed to find world object in %s world config", filename);
        free(ent);
        sj_free(json);
        return NULL;
    }
    model1 = sj_get_string_value(sj_object_get_value(pjson, "model"));
    if (model1)
    {
        ent->model = gf3d_model_load(model1);
    }
    else
    {
        slog("world data (%s) has no model", filename);

    }
    sj_get_float_value(sj_object_get_value(pjson,"maxHealth"),&player_maxHealth);
    sj_get_integer_value(sj_object_get_value(pjson, "gold"), &player_gold);
    sj_get_float_value(sj_object_get_value(pjson, "maxStamina"), &player_maxStamina);
    sj_get_integer_value(sj_object_get_value(pjson, "exp"), &player_exp);
    sj_get_integer_value(sj_object_get_value(pjson, "level"), &player_level);
    sj_get_float_value(sj_object_get_value(pjson, "isPlayer"), &isPlayer);
    ent->think = player_think;
    ent->update = player_update;
    ent->physics = player_physics;
    vector3d_copy(ent->position,position);
    vector3d_copy(playerPosition, ent->position);
    ent->_isplayer = isPlayer; 
    ent->rotation.x = -M_PI;
    ent->health = player_maxHealth;
    slog("Current Level: (%i) ", player_level);
    slog("Current Max Health: (%f)", player_maxHealth);
    slog("Current Health: (%f)", player_maxHealth);
    slog("MAX STAMINA: (%f)", player_maxStamina);
    slog("Current Health: (%f)", player_maxHealth);
    slog("Current Gold: (%i)", player_gold);
    vector3d_add(ent->min,ent->position, vector3d(-5, -5, -5));
    vector3d_add(ent->max, ent->position, vector3d(5, 5, 5));
    //gfc_matrix_slog(ent->modelMat);
    
    return ent; 
}
Entity* player_load() {
    SJson* json, pjson, mjson;
}
void on_hit(Entity *self,Entity* other) {
    other->health -= 20;
    slog("enemy health: (%i)", other->health);
    if (other->health <= 0) {
        other->onDeath(other, self);
        gain_exp(self, 5);
        slog("Current EXP: (%i)", player_exp);
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
        slog("Current EXP: (%i)", player_exp);
    }
    return;
}
void level_up(Entity *self) {
    player_level += 1;
    player_maxHealth += 10;
    player_maxStamina += 10;
    exp_tolevel += exp_tolevel;
    slog("LEVEL UP");
    slog("Current Level: (%i) ", player_level);
    slog("Current Max Health: (%f)", player_maxHealth);
    slog("Current Health: (%f)", playerHealth);
    slog("MAX STAMINA: (%f)", player_maxStamina);
    slog("exp to next level: (%i)", exp_tolevel);
}
void gain_exp(Entity* self, float newexp) {
    player_exp += newexp;
    if (player_exp >= exp_tolevel) {
        level_up(self);
    }
}
void player_attack(Entity* self) {
    Entity *other;
    Vector3D attackMax = vector3d(0, 0, 0);
    Vector3D attackMin = vector3d(0, 0, 0);
    float stamina_required = 25;
    self->recoveryFrames = 50;
    EntityManager *thing = get_Entity_Manager_List();
    slog("attacked");
    if (!self)return;
    if (!check_Stamina(stamina_required, playerStamina))
        return;
    int i;
    vector3d_copy(attackMin, self->min);
    vector3d_copy(attackMax, self->max);
    attackMax.y += ((20 * cos(self->rotation.z)));
    attackMax.x -= ((20 * sin(self->rotation.z)));
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





}
void player_heavy_attack(Entity* self) {
    Entity* other;
    Vector3D attackMax= vector3d(0,0,0);
    Vector3D attackMin= vector3d(0,0,0);
    float stamina_required = 50;
    self->recoveryFrames = 200;
    EntityManager* thing = get_Entity_Manager_List();
    slog("attacked heavily");
    if(!self)return;
    if (!check_Stamina(stamina_required, playerStamina))
        return;
    int i;
    vector3d_copy(attackMin, self->min);
    vector3d_copy(attackMax, self->max);
    attackMax.y += ((40 * cos(self->rotation.z)));
    attackMax.x -= ((20 * sin(self->rotation.z)));
    for (i = 0; i < thing->entity_count; i++) {

        other = &thing->entity_list[i];
        if (self == other)
            continue;
        if (check_intersect(attackMin, attackMax, other->min, other->max)) {
            slog("heavy hit");
            on_heavy_hit(self,other);
        }

    }
    playerStamina -= stamina_required;
    slog("Current Stamina: (%f)", playerStamina);
    
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
        return;
    }
    if (keys[SDL_SCANCODE_D])
    {   
        self->position.y += (0.5 * sin(self->rotation.z));
        self->position.x += (0.5 * cos(self->rotation.z));
        vector3d_add(self->max, self->position, forward);
        vector3d_add(self->min, self->position, forward);
    }
    if (keys[SDL_SCANCODE_A])
    {
        self->position.y -= (0.50 * sin(self->rotation.z));
        self->position.x -= (0.50 * cos(self->rotation.z));
        vector3d_add(self->max, self->position, -forward);
        vector3d_add(self->min, self->position, -forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        self->position.y -= (0.50 * cos(self->rotation.z));
        self->position.x += (0.50 * sin(self->rotation.z));
        vector3d_add(self->max, self->position, right);
        vector3d_add(self->min, self->position, right);
    }
    if (keys[SDL_SCANCODE_W])    
    {
        self->position.y += (0.50 * cos(self->rotation.z));
        self->position.x -= (0.50 * sin(self->rotation.z));
        //gfc_matrix_rotate(self->modelMat, self->modelMat, 180, vector3d(0, 0, 1));
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
        cameraPosition.x = self->position.x;
        cameraPosition.y = self->position.y - 50;
        cameraPosition.z = self->position.z + 10;
        self->position = vector3d(0, 0, 0);
        self->min = vector3d(0, 0, 0);
        self->max = vector3d(0, 0, 0);
        gf3d_camera_set_position(cameraPosition);
        gf3d_camera_set_rotation(self->rotation);
    }
    if (keys[SDL_SCANCODE_RSHIFT]) {
        player_attack(self);
    }
    if (keys[SDL_SCANCODE_F]) {
        player_heavy_attack(self);
    }
    if (keys[SDL_SCANCODE_E]) {
        if (isLockedOn == 0) {
            player_get_lock_on(self);
            slog("_isEnemy: %i", (int)self->_isEnemy);
        }
        else {
            isLockedOn = 0;
        }
    }
    if (keys[SDL_SCANCODE_G]) {
        if (canSave) {
        player_save(self, "config/player1.json");
        slog("trying to save");
        }
        else {
            slog("can't save");
        }
    }
    if (isLockedOn == 0) {
        int x, y, z;
        SDL_GetMouseState(&x, &y);
        self->rotation.z = -((3*x) / (1200 / M_PI));
    }
}

void player_update(Entity *self)
{
    Uint8 health;
    cameraPosition.x = self->position.x + (50 * sin(self->rotation.z));
    cameraPosition.y = self->position.y - (50 * cos(self->rotation.z));
    cameraPosition.z = self->position.z + 10;
    Vector3D forward;
    Vector3D right;
    Vector3D up;
    float angle=0;
    if (!self)return;
    regen_stamina(); 
    if (isLockedOn == 1) {
        player_lock_on(self, target);
    }

    gf3d_camera_set_rotation(self->rotation);
    gf3d_camera_set_position(cameraPosition);
    health = self->health;
    if (health <= 0) {
        slog("player has died");
        entity_free(self);
    }
    vector3d_copy(playerPosition, self->position);
    if (self->recoveryFrames > 0) {
        self->recoveryFrames -= 1;
        if (self->recoveryFrames < 0)
            self->recoveryFrames = 0;
    }
    
}
void player_physics(Entity* self, Entity* other, World* w)
{
    if (!self || !other)return;
    if (check_intersect(self->min, self->max, other->min, other->max) && other->_isbonfire)
    {
        canSave = 1;
        slog("canSave");

    }
        //canSave = 0;
    if (check_intersect(self->min, self->max, other->min, other->max) && other->_isgoldsack) {
        player_pickup_goldsack(self, other);
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
void player_lock_on(Entity* self, Entity* target) {
    float angle = 0;
    Vector2D target_vector;
    target_vector.x = self->position.x - target->position.x;
    target_vector.y = self->position.y - target->position.y;
    angle = vector2d_get_angle(target_vector);
    self->rotation.z = angle;
    if (target_vector.y >= 0) {
        self->rotation.z = angle + (M_PI);
    }
    return;
}
void player_get_lock_on(Entity* self) {
    EntityManager* thing = get_Entity_Manager_List();
    float shortestDistance=100;
    float shortestDistanceTemp;
    int i;
    Entity* other;
    if (isLockedOn == 1) {
        slog("locked on already you dumbass");
        isLockedOn = 0;
        return;
    }
    isLockedOn=1;
    for (i = 0; i < thing->entity_count; i++) {

        other = &thing->entity_list[i];
        if (!other) {
            slog("no target entity");
            return;
        }
        if (!other->_isEnemy==1) {
            continue;
        }
        if (other->_isplayer == 1) {
            slog("entity self number: %i", i);
            continue;
        }
        shortestDistanceTemp = vector3d_magnitude_between(self->position, other->position);
        if (shortestDistanceTemp <= shortestDistance) {
            shortestDistance = shortestDistanceTemp;
            target = other;
            slog("entity number: %i isEnemy: %i", i, other->_isEnemy);
        }
        else {
            continue;
        }

    }
    if (target == NULL) {
        slog("no target");
        return;
    }

}
Vector3D player_get_position() {
    return playerPosition;
}
float player_get_level() {
    return player_level;
}
float player_get_exp() {
    return player_exp;
}
void player_save(Entity *self, char* filename) {
    SJson *json, *player, *model, *maxHealth, *maxStamina, *exp, *level, *gold, *isplayer, *position,
        *positionx,*positiony,*positionz;
    json = sj_object_new();
    player = sj_object_new();
    model = sj_new_str("dino");
    maxHealth = sj_new_float(player_maxHealth);
    maxStamina = sj_new_float(player_maxStamina);
    exp = sj_new_int(player_exp);
    level = sj_new_int(player_level);
    gold = sj_new_int(player_gold);
    isplayer = sj_new_int(isPlayer);
    position = sj_array_new();
    positionx = sj_new_float(self->position.x);
    positiony = sj_new_float(self->position.y);
    positionz = sj_new_float(self->position.z);
    sj_array_append(position, positionx);
    sj_array_append(position, positiony);
    sj_array_append(position, positionz);

    sj_object_insert(json, "player", player);
    sj_object_insert(player, "model", model);
    sj_object_insert(player, "maxHealth", maxHealth);
    sj_object_insert(player, "maxStamina", maxStamina);
    sj_object_insert(player, "exp", exp);
    sj_object_insert(player, "level", level);
    sj_object_insert(player, "gold", gold);
    sj_object_insert(player, "isPlayer", isplayer);
    sj_object_insert(player, "position", position);

    sj_save(json, "config/player.json");

}
void player_pickup_goldsack(Entity* self,Entity *goldsack) {
    player_gold += goldsack_get_goldValue();
    goldsack->onDeath(goldsack, self);
    slog("Total gold: (%i)", player_gold);


}



/*eol@eof*/
