#include <SDL2/SDL.h>

#include <stdint.h>
#include <stdlib.h>


#include "gfx.h"
#include "logger.h"
#include "models.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *textureSDL;
SDL_Surface *screenSurface;
SDL_Event event;

void gfx_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		LOG_FATAL("Could not intialize SDL: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow(WINDOW_TITLE, 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		S3L_RESOLUTION_X, 
		S3L_RESOLUTION_Y, 
		SDL_WINDOW_SHOWN); 
	renderer = SDL_CreateRenderer(window, -1, 0);
	textureSDL = SDL_CreateTexture(renderer, 
		SDL_PIXELFORMAT_RGBX8888, 
		SDL_TEXTUREACCESS_STATIC, 
		S3L_RESOLUTION_X, 
		S3L_RESOLUTION_Y);
	screenSurface = SDL_GetWindowSurface(window);

	cityModelInit();
	carModelInit();

	models[CITY_MODEL] = cityModel;
	models[CAR_MODEL] = carModel;
}

void gfx_end(void)
{
	SDL_DestroyTexture(textureSDL);
	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window);
}

void gfx_update(void)
{
	SDL_UpdateTexture(textureSDL, NULL, pixels, S3L_RESOLUTION_X * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, textureSDL, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void clear_screen(void)
{
	memset(pixels, 200, S3L_RESOLUTION_X * S3L_RESOLUTION_Y * sizeof(uint32_t));
}

void clearScreenBlue()
{
	uint32_t index = 0;

	for (uint16_t y = 0; y < S3L_RESOLUTION_Y; ++y)
	{
		S3L_Unit t = S3L_min(S3L_F, ((y * S3L_F) / S3L_RESOLUTION_Y) * 4);

		uint32_t r = S3L_interpolateByUnit(200,242,t);
		uint32_t g = S3L_interpolateByUnit(102,255,t);
		uint32_t b = S3L_interpolateByUnit(255,230,t);

		uint32_t color = (r << 24) | (g << 16 ) | (b << 8);

		for (uint16_t x = 0; x < S3L_RESOLUTION_X; ++x)
		{
			pixels[index] = color;
			index++;
		}
	}
}

static inline void set_pixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t *p = ((uint8_t *) pixels) + (y * S3L_RESOLUTION_X + x) * 4 + 1;

	*p = blue;
	++p;
	*p = green;
	++p;
	*p = red;
}

void sample_texture(const uint8_t *tex, int32_t u, int32_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
	u = S3L_wrap(u,TEXTURE_W);
	v = S3L_wrap(v,TEXTURE_H);

	const uint8_t *t = tex + (v * TEXTURE_W + u) * 4;

	*r = *t;
	t++;
	*g = *t;
	t++;
	*b = *t;
}

uint32_t previousTriangle = -1;
S3L_Vec4 uv0, uv1, uv2;

void draw_pixel(S3L_PixelInfo *p)
{
	if (p->triangleID != previousTriangle)
	{
		const S3L_Index *uvIndices;
		const S3L_Unit *uvs;

		if (p->modelIndex == 0)
		{
			uvIndices = cityUVIndices;
			uvs = cityUVs;
		}
		else
		{
			uvIndices = carUVIndices;
			uvs = carUVs;
		}

		S3L_getIndexedTriangleValues(p->triangleIndex, uvIndices, uvs, 2, &uv0, &uv1, &uv2);

		previousTriangle = p->triangleID;
	}

	uint8_t r, g, b;

	S3L_Unit uv[2];

	uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
	uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

	sample_texture(cityTexture,uv[0] >> 1,uv[1] >> 1,&r,&g,&b);
	
	set_pixel(p->x,p->y,r,g,b); 
}

void draw(S3L_Scene scene)
{
	S3L_newFrame();
	clearScreenBlue();
	S3L_drawScene(scene);
}