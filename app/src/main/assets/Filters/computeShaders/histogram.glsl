#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
layout(std430) buffer;
layout (rgba8ui,binding=0) uniform readonly highp uimage2D image;//image//image unifroms are supported in fragment shaders so try equalize in fragemnt shader;
layout (std430, binding=1) buffer binsDat//binsBuffer//check binding point can be same as image vars
{
    int bins[360];
};
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
layout(location=0) uniform int filterType;
layout(location=1) uniform int paramInt;
layout(location=2) uniform int inOrOut;//to determina if bins for input or output image;
layout(location=5) uniform float params[4];///no need for eq has it has no sliders
void main()
{
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    //uint lid=gl_LocalInvocationIndex;
    uvec4 inPix= imageLoad(image,pos);
    vec3 rgb=vec3(inPix.xyz);
    vec3 hsi=rgbToHsi(rgb);////Thes covertion no need for rgb histograms
    switch (filterType)//first 6 for histogramequ;
    {

        case 0://for R
        {
            // bins[outp.r]+=1;
            atomicAdd(bins[inPix.r], 1);//return value before adding
        }break;
        case 1://for G
        {
            atomicAdd(bins[inPix.g], 1);
        }break;
        case 2://for B
        {
            atomicAdd(bins[inPix.b], 1);
        }break;
        case 3://histogram for I;//
        {
            atomicAdd(bins[uint(hsi.b)], 1);/////for floats type adtomic load stores not directly but input image as float and ouput as int just  like that check
        }
        break;
        case 4://H
        {
            atomicAdd(bins[uint(hsi.r)], 1);///actually igonring decimals check to composate by add or sub difference in ouput
        }break;
        case 5:
        {
            float tempSat=hsi.g*255.0;
            atomicAdd(bins[uint(tempSat)], 1);
        }break;
    }
}