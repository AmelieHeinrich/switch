//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 20:28:09
//

#include <string.h>

#include "camera.h"

void camera_compute_vectors(camera_t *camera)
{
    HMM_Vec3 front;
    front.X = cos(HMM_ToRad(camera->yaw)) * cos(HMM_ToRad(camera->pitch));
    front.Y = sin(HMM_ToRad(camera->pitch));
    front.Z = sin(HMM_ToRad(camera->yaw)) * cos(HMM_ToRad(camera->pitch));
    camera->front = HMM_NormV3(front);

    camera->right = HMM_NormV3(HMM_Cross(camera->front, HMM_V3(0.0f, 1.0f, 0.0f)));
    camera->up = HMM_NormV3(HMM_Cross(camera->right, camera->front));
}

void camera_init(camera_t* camera)
{   
    memset(camera, 0, sizeof(camera_t));

    camera->position = HMM_V3(0.0f, 0.0f,-1.0f);
    camera->front = HMM_V3(0.0f, 0.0f, 1.0f);
    camera->right = HMM_V3(1.0f, 0.0f, 0.0f);
    camera->up = HMM_V3(0.0f, 1.0f, 0.0f);
    camera->yaw = 90.0f;

    camera_compute_vectors(camera);
}

void camera_update(camera_t* camera)
{
    camera->view = HMM_LookAt_LH(camera->position, HMM_AddV3(camera->position, camera->front), HMM_V3(0.0f, 1.0f, 0.0f));
    camera->projection = HMM_Perspective_LH_ZO(HMM_ToDeg(90.0f), camera->width / camera->height, 0.001f, 10000.0f);

    camera_compute_vectors(camera);
}

void camera_input(camera_t* camera, pad_t *pad)
{
    i32 lx, ly, rx, ry;
    pad_get_lstick(pad, &lx, &ly);
    pad_get_rstick(pad, &rx, &ry);

    if (ly > 3000 || ly < 3000)
        camera->position = HMM_AddV3(camera->position, HMM_MulV3F(camera->front, (ly / 32766.0) * 0.1));
    if (lx > 3000 || lx < 3000)
        camera->position = HMM_SubV3(camera->position, HMM_MulV3F(camera->right, (lx / 32766.0) * 0.1));

    camera->yaw -= (rx / 32766.0) * 2.0;
    camera->pitch += (ry / 32766.0) * 2.0;

    camera_compute_vectors(camera);
}

void camera_resize(camera_t* camera, i32 width, i32 height)
{
    camera->width = width;
    camera->height = height;
}
