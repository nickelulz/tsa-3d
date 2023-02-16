#ifndef GFX_H
#define GFX_H

#define S3L_FLAT 0
#define S3L_NEAR_CROSS_STRATEGY 3
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 2

#define S3L_PIXEL_FUNCTION draw_pixel

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480

#define TEXTURE_W 256
#define TEXTURE_H 256

#define WINDOW_TITLE "hello, world"

#include <SDL2/SDL.h>
#include <stdint.h>
#include "small3dlib.h"
#include "models.h"

extern uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

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

extern S3L_Model3D models[NUM_MODELS];

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