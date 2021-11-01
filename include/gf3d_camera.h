#ifndef __GF3D_CAMERA_H__
#define __GF3D_CAMERA_H__

#include "gfc_matrix.h"

typedef struct
{
    Matrix4 cameraMat; // final matrix to become the view matrix
    Vector3D scale;
    Vector3D position;
    Vector3D rotation; // pitch, roll, yaw
}Camera;
/*
@brief take the position,scale, and rotation to calculate the view matrix
@note: Do not use if your are tailoring the camera matrix by hand
*/
void gf3d_camera_update_view();
/**
 * @brief get the current camera view
 * @param view output, the matrix provided will be populated with the current camera information
 */
void gf3d_camera_get_view_mat4(Matrix4 *view);

/**
 * @brief set the current camera based on the matrix provided
 */
void gf3d_camera_set_view_mat4(Matrix4 *view);

/**
 * @brief set the camera properties based on position and direction that the camera should be looking
 * @param position the location for the camera
 * @param target the point the camera should be looking at
 * @param up the direction considered to be "up"
 */
void gf3d_camera_look_at(
    Vector3D position,
    Vector3D target,
    Vector3D up
);

/**
 * @brief explicitely set the camera positon, holding all other parameters the same
 * @param position the new position for the camera
 */
void gf3d_camera_set_position(Vector3D position);
/** 
*@briedf explicitly set the camera scale 
*@param scale the new scale
*/
void gf3d_camera_set_scale(Vector3D position);
/*
@brief explictly set the camera rotation
@param scale the new scale
*/

void gf3d_camera_set_rotation(Vector3D position);

/**
 * @brief move the camera relatively based on the vector provided
 * @param move the ammount to move the camera
 */
void gf3d_camera_move(Vector3D move);

#endif
