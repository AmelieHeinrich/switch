//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 00:16:05
//

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <deko3d.h>

#include "core.h"

typedef struct sampler_t {
    DkSampler sampler;
    DkSamplerDescriptor descriptor;    
} sampler_t;

sampler_t sampler_init(DkFilter filter, DkWrapMode mode);

#endif
