#ifndef GFX_H
#define GFX_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include "config.h"
#include "small3dlib.h"
#include "models.h"

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

// SDL
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textureSDL;
extern SDL_Surface *screenSurface;
extern SDL_Event event;

// Models
#define NUM_MODELS 2

enum ModelType {
	CITY_MODEL,
	CAR_MODEL
};

S3L_Model3D models[NUM_MODELS];

// Graphics Management
void gfx_init(void);
void gfx_end(void);
void gfx_update(void);

// Utility
void clear_screen(void);
void clear_screen_blue(void);
void sample_texture(const uint8_t *tex, int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b);
void draw_pixel(S3L_PixelInfo *p);
void draw(S3L_Scene scene);

#endif