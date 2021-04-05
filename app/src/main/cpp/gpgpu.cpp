//
// Created by Shiva Shankar patel psy on 3/20/21.
//
#include "Graphics.h"
#include "gpgpu.h"
void Compute::getGlobalWorkGroupSize(int *x, int *y, int *z)
{
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0,x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,1,y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,2,z);
    Loge("GLobalWorkGroupsize  for dispatch", "theComputeGroupCount is %d and %d and %d",*x, *y,*z);
}
void Compute::getLocalWorkGroupSize(int *x, int *y, int *z)
{
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,0,x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,1,y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,2,z);
    Loge("maxLocalWorkGroup size in shaders", "theComputeGroupsize is %d and %d and %d",*x, *y,*z);
}
void Compute::getMaxInvocationSize(int *invocationSize)
{
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,invocationSize);
    Loge("MaxThreads supported per workgroup(product of thre localworkgroup sizes","the max invocationsiz is %d",*invocationSize);

}
void Compute::showGpuCapacity()
{
    int workGroupCount[3],workGroupSize[3],maxInvocationSize;//count for dispatch,size for shader(threads);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0,&workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,1,&workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,2,&workGroupCount[2]);
    Loge("GLobalWorkGroupsize  for dispatch", "theComputeGroupCount is %d and %d and %d", workGroupCount[0], workGroupCount[1], workGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,0,&workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,1,&workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,2,&workGroupSize[2]);
    Loge("maxLocalWorkGroup size in shaders", "theComputeGroupsize is %d and %d and %d", workGroupSize[0], workGroupSize[1], workGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,&maxInvocationSize);
    Loge("MaxThreads supported per workgroup(product of thre localworkgroup sizes","the max invocationsiz is %d",maxInvocationSize);
}
void Compute::dispatch(int workGroupSizeX,int workGroupSizeY,int workGroupSizeZ)
{
    glDispatchCompute(workGroupSizeX,workGroupSizeY,workGroupSizeZ);
}