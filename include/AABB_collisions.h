#ifndef __AABB_COLLISIONS_H__
#define __AABB_COLLISIONS_H__

#include "simple_logger.h"
#include "gfc_types.h"
#include <stdio.h>
#include <string.h>

#include "gf3d_camera.h"

typedef struct {
	Vector3D min;
	Vector3D max;

}AABB;

Vector3D asfd;
void set_AABB(AABB *box);
Bool check_intersect(Vector3D minSelf, Vector3D maxSelf, Vector3D minOther, Vector3D maxOther);
#endif