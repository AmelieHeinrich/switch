//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 00:17:26
//

#include "sampler.h"

sampler_t sampler_init(DkFilter filter, DkWrapMode mode)
{
    sampler_t sampler;

    dkSamplerDefaults(&sampler.sampler);
    sampler.sampler.minFilter = filter;
    sampler.sampler.magFilter = filter;
    sampler.sampler.wrapMode[0] = mode;
    sampler.sampler.wrapMode[1] = mode;
    sampler.sampler.wrapMode[2] = mode;

    dkSamplerDescriptorInitialize(&sampler.descriptor, &sampler.sampler);
    return sampler;
}
