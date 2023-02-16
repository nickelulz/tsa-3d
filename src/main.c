#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "gfx.h"
#include "logger.h"
#include "tgc.h"

#define MAX_VELOCITY 1000
#define ACCELERATION 700
#define TURN_SPEED 300
#define FRICTION 600

const uint8_t collisionMap[8 * 10] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 1, 0, 1,
	2, 2, 1, 0, 0, 0, 0, 3,
	1, 2, 1, 0, 1, 1, 3, 1,
	2, 0, 0, 0, 1, 1, 3, 3,
	1, 0, 1, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

static inline uint8_t collision(S3L_Vec4 worldPosition)
{
	worldPosition.x /= S3L_F;
	worldPosition.z /= -S3L_F;		

	uint16_t index = worldPosition.z * 8 + worldPosition.x;

	return collisionMap[index];
}

static inline void handle_collision(S3L_Vec4 *pos, S3L_Vec4 previousPos)
{
	S3L_Vec4 newPos = *pos;
	newPos.x = previousPos.x;
		
	if (collision(newPos))
	{
		newPos = *pos;
		newPos.z = previousPos.z;

		if (collision(newPos))
			newPos = previousPos;
	}

	*pos = newPos;
}

// Frame Variables
S3L_Scene scene;
S3L_Vec4 carDirection;
uint32_t frame = 0;
int16_t velocity = 0, fps = 0;
clock_t nextPrintT;
bool running = true;

// Garbage Collector
static tgc_t gc;

void loop() {
	tgc_run(&gc);

	clock_t frameStartT = clock();

	// overturn the car for the rendering
	models[CAR_MODEL].transform.rotation.y += models[CAR_MODEL].transform.rotation.z;

	draw(scene);

	// turn the car back for the physics
	models[CAR_MODEL].transform.rotation.y -= models[CAR_MODEL].transform.rotation.z;

	fps++;

	clock_t nowT = clock();

	double timeDiff = ((double) (nowT - nextPrintT)) / CLOCKS_PER_SEC;
	double frameDiff = ((double) (nowT - frameStartT)) / CLOCKS_PER_SEC;
	int16_t frameDiffMs = frameDiff * 1000;

	if (timeDiff >= 1.0)
	{
		nextPrintT = nowT;
		LOG_DEBUG("FPS: %d\n", fps);
		fps = 0;
	}

	while (SDL_PollEvent(&event))
		if (event.type == SDL_QUIT)
			running = 0;

	const uint8_t *state = SDL_GetKeyboardState(NULL);

	int16_t step = (velocity * frameDiffMs) / 1000;
	int16_t stepFriction = (FRICTION * frameDiffMs) / 1000;
	int16_t stepRotation = TURN_SPEED * frameDiffMs * S3L_max(0,velocity - 200) / (MAX_VELOCITY * 1000);

	int16_t stepVelocity = S3L_nonZero((ACCELERATION * frameDiffMs) / 1000);

	if (stepRotation == 0 && S3L_abs(velocity) >= 200)
		stepRotation = 1;

	if (velocity < 0)
		stepRotation *= -1;

	if (state[SDL_SCANCODE_ESCAPE])
		running = false;
	else if (state[SDL_SCANCODE_LEFT])
	{
		models[CAR_MODEL].transform.rotation.y += stepRotation;
		models[CAR_MODEL].transform.rotation.z =
			S3L_min(S3L_abs(velocity) / 64, models[CAR_MODEL].transform.rotation.z + 1);
	}
	else if (state[SDL_SCANCODE_RIGHT])
	{
		models[CAR_MODEL].transform.rotation.y -= stepRotation;
		models[CAR_MODEL].transform.rotation.z =
			S3L_max(-S3L_abs(velocity) / 64, models[CAR_MODEL].transform.rotation.z - 1);
	}
	else
		models[CAR_MODEL].transform.rotation.z = (models[CAR_MODEL].transform.rotation.z * 3) / 4;

	S3L_rotationToDirections(models[CAR_MODEL].transform.rotation,S3L_F,&carDirection,0,0);

	S3L_Vec4 previousCarPos = models[CAR_MODEL].transform.translation;

	int16_t friction = 0;

	if (state[SDL_SCANCODE_UP])
		velocity = S3L_min(MAX_VELOCITY, velocity + (velocity < 0 ? (2 * stepVelocity) : stepVelocity));
	else if (state[SDL_SCANCODE_DOWN])
		velocity = S3L_max(-MAX_VELOCITY, velocity - (velocity > 0 ? (2 * stepVelocity) : stepVelocity));
	else
		friction = 1;

	models[CAR_MODEL].transform.translation.x += (carDirection.x * step) / S3L_F;
	models[CAR_MODEL].transform.translation.z += (carDirection.z * step) / S3L_F;

	uint8_t coll = collision(models[CAR_MODEL].transform.translation);

	if (coll != 0)
	{
		if (coll == 1)
		{
			handle_collision(&(models[CAR_MODEL].transform.translation), previousCarPos);
			friction = 2;
		}
		else if (coll == 2)
		{
			// teleport the car
			models[CAR_MODEL].transform.translation.x += 5 * S3L_F;
			models[CAR_MODEL].transform.translation.z += 2 * S3L_F;
		}
		else
		{
			// teleport the car
			models[CAR_MODEL].transform.translation.x -= 5 * S3L_F;
			models[CAR_MODEL].transform.translation.z -= 2 * S3L_F;
		}
	}

	if (velocity > 0)
		velocity = S3L_max(0,velocity - stepFriction * friction);
	else
		velocity = S3L_min(0,velocity + stepFriction * friction);

	S3L_Unit cameraDistance = 
		S3L_interpolate(S3L_F / 2,(3 * S3L_F) / 4, S3L_abs(velocity), MAX_VELOCITY);

	scene.camera.transform.translation.x =
		scene.models[CAR_MODEL].transform.translation.x - (carDirection.x * cameraDistance) / S3L_F;

	scene.camera.transform.translation.z =
		scene.models[CAR_MODEL].transform.translation.z - (carDirection.z * cameraDistance) / S3L_F;

	scene.camera.transform.rotation.y = models[CAR_MODEL].transform.rotation.y;

	gfx_update();

	frame++;
}

int main(int argc, char* argv[])
{
	logger_initConsoleLogger(stderr);
	logger_setLevel(LogLevel_DEBUG);

	gfx_init();

	tgc_start(&gc, &argc);

	S3L_sceneInit(models, NUM_MODELS, &scene);
	S3L_transform3DSet(1909, 16, -3317, 0, -510, 0, 512, 512, 512, &(models[CAR_MODEL].transform));

	nextPrintT = clock();

	S3L_vec4Init(&carDirection);
	
	scene.camera.transform.translation.y = S3L_F / 2;
	scene.camera.transform.rotation.x = -S3L_F / 16;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, 1);
#else
	while (running)
		loop();
#endif

	gfx_end();
	tgc_stop(&gc);

	return 0;
}
