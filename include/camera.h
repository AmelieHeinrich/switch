//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 20:25:44
//

#ifndef CAMERA_H_
#define CAMERA_H_

#include "core.h"
#include "input.h"

typedef struct camera_t {
    HMM_Vec3 position;
    HMM_Vec3 front;
    HMM_Vec3 right;
    HMM_Vec3 up;

    f32 width;
    f32 height;

    f32 yaw;
    f32 pitch;

    HMM_Mat4 view;
    HMM_Mat4 projection;
} camera_t;

void camera_init(camera_t* camera);
void camera_update(camera_t* camera);
void camera_input(camera_t* camera, pad_t *pad);
void camera_resize(camera_t* camera, i32 width, i32 height);

#endif
