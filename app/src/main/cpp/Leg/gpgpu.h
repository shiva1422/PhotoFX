//
// Created by Shiva Shankar patel psy on 3/20/21.
//

#ifndef PHOTOFX_GPGPU_H
#define PHOTOFX_GPGPU_H

#include "Commons.h"
class Compute{

public:
    static void getLocalWorkGroupSize(int *x,int *y,int *z);
    static void getGlobalWorkGroupSize(int *x,int *y,int *z);
    static void getMaxInvocationSize(int *invocationSize);
    static void showGpuCapacity();
    kforceinline static void dispatch(int workGroupSizeX,int workGroupSizeY,int workGroupSizeZ);

};


#endif //PHOTOFX_GPGPU_H
