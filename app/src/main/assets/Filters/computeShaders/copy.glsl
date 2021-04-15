#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
layout(std430) buffer;
layout (rgba8ui,binding=0) uniform readonly highp uimage2D imageIn;
layout(rgba8ui,binding=1) uniform writeonly highp uimage2D imageOut;
void main()
{
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uvec4 inPix= imageLoad(imageIn,pos);
    imageStore(imageOut,pos,inPix);
}