#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
layout(std430) buffer;
float kdistance(ivec2 vect1,ivec2 vect2);
layout (rgba8ui,binding=0) uniform readonly highp uimage2D imageIn;//image//image unifroms are supported in fragment shaders so try equalize in fragemnt shader;
layout(rgba8ui,binding=1) uniform writeonly highp uimage2D imageOut;
layout (std430, binding=2) buffer MinLaplace//sharpen requires two iteration to make mimum value as zero (see graph)
{
    int minMax[7];//last is 0 before iteration (minumum not calculated) 1 after calculating minimum value;
};
layout(location=0) uniform int filterType;
layout(location=5) uniform float params[4];
void main()
{
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uint lid=gl_LocalInvocationIndex;
    ivec2 imageDims=imageSize(imageIn);
    int m=3,n=3;//size of mask
    uvec4 finalPixel=uvec4(0,0,0,0);//just r,g,b enough;
    ivec2 center=imageDims/2;
    for(pos.y=0;pos.y<imageDims.y;pos.y++)
    {
        finalPixel=uvec4(0,0,0,0);
        if(kdistance(center,pos)<=params[0]*5.0)
        {
            for(int i=-4;i<5;i++)
            {
                for(int j=-4;j<5;j++)
                {
                    ivec2 tempPos=pos + ivec2(i,j);
                   // if(tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)
                  //  {
                        finalPixel+=imageLoad(imageIn,tempPos);
                   // }
                }
            }
            //  finalPixel.r=uint(255*25);
            finalPixel/=uint(81);//division not accurate so check
            imageStore(imageOut,pos,uvec4(finalPixel.rgb,uint(255)));//////this alpha should be equal to input pixel;
        }
        else
        imageStore(imageOut,pos,imageLoad(imageIn,pos));
    }





}
float kdistance(ivec2 vect1,ivec2 vect2)
{
    vec2 dis=vec2(vect1-vect2);
    return sqrt(float(dis.x*dis.x+dis.y*dis.y));

}