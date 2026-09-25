//main.c
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "prime_renderer_64.c"

#define W 256
#define H 144
#define TILE 16
#define TARGET_FPS 60
#define DEAD_ZONE 0.2
#define STICK_NORMALIZER 0.0117647
#define DIAGONAL_SPEED 0.7071

static sprite_t *sprite = NULL;

float px = 100;
float py = 100;
float pa = 0;

bool running = true;

joypad_inputs_t inputs;
joypad_inputs_t prev_inputs;

int main(void) {
    // libdragon subsystems are typically initialized on demand by calling
    // *_init functions, like here we initialize the display and rdpq systems.
    const resolution_t my_resolution = { W, H, false };

    display_init(my_resolution, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    rdpq_init();

    dfs_init(DFS_DEFAULT_LOCATION);
	joypad_init();

    // Register a built-in font as id 1
    rdpq_text_register_font(1, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR));

    //load sprite
    sprite = sprite_load("rom:/cube_engine.sprite");

    while (running) {
        // Acquire a free framebuffer for rendering
        surface_t *disp = display_get();
        // Attach the framebuffer for use by rdpq
        rdpq_attach(disp, NULL);

        // Clear the framebuffer with black
        rdpq_clear((color_t){0, 0, 0, 0});

        //-------------------------------
        //It starts with this!
        //-------------------------------
        beginQuads();

            //draw ground
            setTex(sprite); int step = TILE; float tile_size = TILE * 0.5;
            for(int y = 0; y <= 144; y += step){
                for(int x = 0; x <= 256; x += step){
                   renderQuad(x, y, tile_size, 0, 0, 1, 0, 1);
                }
            }

            //draw player
            setTex(sprite); renderQuad(px, py, TILE, pa, 0.5, 0.5, 0.5, 1);

        endQuads();
        //-------------------------------
        //THE END
        //-------------------------------

        // Print some text
        rdpq_text_print(NULL, 1, 50, 50, "Hello World!");

        // Draw sprite using the RDP
        rdpq_sprite_blit(sprite, 200, 50, NULL);

        // Detach the framebuffer and show it on screen when it's ready
        // (when previous rendering operations have completed)
        rdpq_detach_show();

        joypad_poll();
		inputs = joypad_get_inputs(JOYPAD_PORT_1);

        float ls_x = inputs.stick_x * STICK_NORMALIZER;
        float ls_y = inputs.stick_y * STICK_NORMALIZER;
        int diagonal = 0;

        if (fabsf(ls_x) < DEAD_ZONE) ls_x = 0.0;
        else diagonal += 1;

        if (fabsf(ls_y) < DEAD_ZONE) ls_y = 0.0;
        else diagonal += 1; 

        if (diagonal >= 2){
            ls_x = ls_x * DIAGONAL_SPEED;
            ls_y = ls_y * DIAGONAL_SPEED;
        }

        if (ls_x != 0.0f || ls_y != 0.0f) pa = atan2f(ls_x, ls_y);

        px += ls_x;
        py -= ls_y;

        if (inputs.btn.start) {
            running = false;
        }

        prev_inputs = inputs;
    }
}
