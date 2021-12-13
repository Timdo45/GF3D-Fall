#include <SDL.h>            

#include "simple_logger.h"
#include "simple_json.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_sprite.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"
#include "bonfire.h"

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 0;
    const Uint8 * keys;
    
    Sprite* mouse = NULL;
    Sprite* mainMenu = NULL;
    int mousex, mousey;
    float mouseFrame = 0;
    Sound* bgmusic = NULL;
    Vector2D mouselocation;
    Uint32 mouseState;
    Uint8 gameStart=0;
    
    World *w;
    
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"-disable_validate") == 0)
        {
            validate = 0;
        }
    }
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        validate                //validation
    );
	slog_sync();
    
    gfc_audio_init(4,1,1,1,true,true);
    bgmusic = gfc_sound_load("sounds/Kevin MacLeod - Fluffing a Duck.mp3", 1, 0);
    //bgmusic = gfc_sound_load("sounds/sound.wav", 1, 0);
    entity_system_init(2024);

    mouse = gf3d_sprite_load("images/pointer.png", 32, 32, 16);
    mainMenu = gf3d_sprite_load("images/newgame.png", 771, 161, 1);

    w = world_load("config/testworld.json");
    player_new(vector3d(0,10,0),"config/player.json");
    bonfire_new(vector3d(20, 20, 0), "config/testworld.json");

    for (a = 0; a < 2;a++)
    {
        agumon_new(vector3d(a * 20 -50,150,0));
    }
    // main game loop
	slog_sync();
    gf3d_camera_set_scale(vector3d(0.0010,0.0010,0.0010));
    
    slog("gf3d main loop begin");
    gfc_sound_play(bgmusic, -1, 1, -1, -1);
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

        mouseState=SDL_GetMouseState(&mousex, &mousey);

        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        gf3d_vgraphics_render_start();
        if (!gameStart) {
            gf3d_sprite_draw(mainMenu, vector2d(0, 0), vector2d(5, 5), 1);
        }
        gf3d_sprite_draw(mouse, vector2d(mousex, mousey), vector2d(1, 1), (Uint32)mouseFrame);
        vector2d_copy(mouselocation, vector2d(mousex, mousey));
        if (mouselocation.x <= 771 && mouselocation.y <= 161 && (mouseState & SDL_BUTTON_LMASK) != 0) {
            slog("world load");
            gameStart = 1;
            gf3d_sprite_free(mainMenu);
        }
        if (gameStart) {

            entity_think_all();
            entity_update_all();
            entity_physics_all(w);
            world_draw(w);
            entity_draw_all();
            world_run_updates(w);
        }
        gf3d_vgraphics_render_end();

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
