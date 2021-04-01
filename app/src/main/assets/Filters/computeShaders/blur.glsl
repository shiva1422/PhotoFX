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
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
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
    switch(filterType)//Implement for other shapes as well;and for r,g,b,h,s,i, see color image processing;
    {
        case 0:
        {
            if(kdistance(center,pos)<=params[0]*5.0)
            {
                for(int i=-4;i<5;i++)
                {
                    for(int j=-4;j<5;j++)
                    {
                        ivec2 tempPos=pos + ivec2(i,j);
                        if(tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)
                        {
                            finalPixel+=imageLoad(imageIn,tempPos);
                        }
                    }
                }
                //  finalPixel.r=uint(255*25);
                finalPixel/=uint(81);//division not accurate so check
                imageStore(imageOut,pos,uvec4(finalPixel.rgb,uint(255)));//////this alpha should be equal to input pixel;
            }
            else
            imageStore(imageOut,pos,imageLoad(imageIn,pos));

        }break;
        case 1:
        {
            int centerWeight=8,otherWeight=-1;
            int scale=int(params[1]*10.0/360.0);//scale 0 - 10;
            ;//////check different weights;//if diagonal neighbour not include use one else if inside loop for i!=j
            ivec4 tempFinalPixel=ivec4(0,0,0,0);
            int iteration=atomicAdd(minMax[6],0);
            if(kdistance(center,pos)<=params[0]*5.0||iteration==0)
            {
                for(int i=-1;i<2;i++)
                {
                    for(int j=-1;j<2;j++)
                    {
                        ivec2 tempPos=pos + ivec2(i,j);
                        if(tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)
                        {
                            finalPixel=imageLoad(imageIn,tempPos);//*uvec4((1,1,1,1));
                            if(i==0&&j==0)
                            {
                                tempFinalPixel+=centerWeight*ivec4(finalPixel);
                            }
                            else
                            {
                                tempFinalPixel+=otherWeight*ivec4(finalPixel);
                            }
                        }
                    }
                }
                if(iteration==0)//first iteration;
                {
                    //tempFinalPixel=ivec4(imageLoad(imageIn,pos))+tempFinalPixel;//sharpened result;
                    atomicMin(minMax[0],tempFinalPixel.r);
                    atomicMin(minMax[1],tempFinalPixel.g);
                    atomicMin(minMax[2],tempFinalPixel.b);
                    atomicMax(minMax[3],tempFinalPixel.r);
                    atomicMax(minMax[4],tempFinalPixel.g);
                    atomicMax(minMax[5],tempFinalPixel.b);
                }
                else
                {
                    vec3 minValues=vec3(atomicAdd(minMax[0],0),atomicAdd(minMax[1],0),atomicAdd(minMax[2],0));//atomic neede?
                    vec3 maxValues=vec3(minMax[3],minMax[4],minMax[5]);

                    minValues.r=128.0*(float(tempFinalPixel.r)-minValues.r)/(maxValues.r-minValues.r);//converting laplacian image in range 0 -255;
                    minValues.g=128.0*(float(tempFinalPixel.g)-minValues.g)/(maxValues.g-minValues.g);
                    minValues.b=128.0*(float(tempFinalPixel.b)-minValues.b)/(maxValues.b-minValues.b);
                    //minValues=vec3(255.0,255.0,255.0)-minValues;///no need;




                    tempFinalPixel=scale*ivec4(minValues,tempFinalPixel.a);
                    //tempFinalPixel=scale*tempFinalPixel;
                    tempFinalPixel=ivec4(imageLoad(imageIn,pos))+tempFinalPixel;//sharpened result;
                    if(tempFinalPixel.r>255)
                    tempFinalPixel.r=255;
                    if(tempFinalPixel.g>255)
                    tempFinalPixel.g=255;
                    if(tempFinalPixel.b>255)
                    tempFinalPixel.b=255;
                    imageStore(imageOut,pos,uvec4(tempFinalPixel));
                }


            }
            else
            imageStore(imageOut,pos,imageLoad(imageIn,pos));

        }break;
    }


}
float kdistance(ivec2 vect1,ivec2 vect2)
{
    vec2 dis=vec2(vect1-vect2);
    return sqrt(float(dis.x*dis.x+dis.y*dis.y));

}