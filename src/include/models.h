#ifndef MODELS_H
#define MODELS_H

#include "gfx.h"
#include "small3dlib.h"

extern const uint8_t cityTexture[262144];

extern S3L_Model3D carModel;
extern S3L_Model3D cityModel;

void carModelInit(void);
void cityModelInit(void);


/* 
 * Models
 */

#define CAR_VERTEX_COUNT 12
extern const S3L_Unit carVertices[CAR_VERTEX_COUNT * 3];

#define CAR_TRIANGLE_COUNT 18
extern const S3L_Index carTriangleIndices[CAR_TRIANGLE_COUNT * 3];

#define CAR_UV_INDEX_COUNT 18
extern const S3L_Index carUVIndices[CAR_UV_INDEX_COUNT * 3];

#define CAR_UV_COUNT 24
extern const S3L_Unit carUVs[CAR_UV_COUNT * 2];

#define CITY_VERTEX_COUNT 155
extern const S3L_Unit cityVertices[CITY_VERTEX_COUNT * 3];

#define CITY_TRIANGLE_COUNT 197
extern const S3L_Index cityTriangleIndices[CITY_TRIANGLE_COUNT * 3];

#define CITY_UV_COUNT 377
extern const S3L_Unit cityUVs[CITY_UV_COUNT * 2];

#define CITY_UV_INDEX_COUNT 197
extern const S3L_Index cityUVIndices[CITY_UV_INDEX_COUNT * 3];

#endif