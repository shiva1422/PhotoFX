#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
layout(std430) buffer;
float kdistance(ivec2 vect1,ivec2 vect2);
vec3 calculateLaplacian(ivec2 pos);
vec3 scaleFilter(vec3 inputFilter);
vec3 getBlurredPixel(ivec2 pos);
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
ivec2 imageDims,center;
void main()
{
    float scale=(params[1]*5.0/360.0);//check scale range;
    imageDims=imageSize(imageIn);
    center=imageDims/2;
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    vec4 tempFinalPixel;
    for(pos.y=0;pos.y<imageDims.y;pos.y++)//based on glDispatchCOmputeSize in cpu
    {
       // uint lid=gl_LocalInvocationIndex;
       tempFinalPixel=vec4(0,0,0,0);

        switch(filterType)
        {
            case 0://non scaled laplacian;(laplace not in range 0 - 255 or as required;
            {
                if (kdistance(center,pos)<=params[0]*5.0)
                {
                    tempFinalPixel=vec4(calculateLaplacian(pos),255.0);//alpha value should be same as input;
                }
                else
                {
                    tempFinalPixel=vec4(imageLoad(imageIn,pos));
                    break;
                }
                tempFinalPixel*=scale;//scale to 0 - 255;//just the high freq image or laplacian or edges
                //  tempFinalPixel+=vec4(imageLoad(imageIn,pos));//sharpened result//scale 0 - 255
            }break;
            case 1://scaled laplacian(0-255)
            {
                if (kdistance(center,pos)<=params[0]*5.0)
                {
                    tempFinalPixel=vec4(calculateLaplacian(pos),255.0);//alpha value should be same as input;
                    int iteration=minMax[6];
                    if(iteration==0)
                    {
                        atomicMin(minMax[0],int(tempFinalPixel.r));
                        atomicMin(minMax[1],int(tempFinalPixel.g));
                        atomicMin(minMax[2],int(tempFinalPixel.b));
                        atomicMax(minMax[3],int(tempFinalPixel.r));
                        atomicMax(minMax[4],int(tempFinalPixel.g));
                        atomicMax(minMax[5],int(tempFinalPixel.b));
                    }
                    else
                    {

                        tempFinalPixel.rgb=scaleFilter(tempFinalPixel.rgb);
                        tempFinalPixel*=scale;
                        tempFinalPixel+=vec4(imageLoad(imageIn,pos));//sharpened result//scale 0 - 255//chec from - to + just instead of +
                    }
                }
                else
                {
                    tempFinalPixel=vec4(imageLoad(imageIn,pos));
                }

            }break;
            case 2://UnsharpMasked;
            {
                /*
                1.blur the image;
                2.subtract the blurred image from original to get high frequency component or edges
                3.add the result from 2(mask) to the original image
                check amount of blurring needed (amout of edges highlighted)
                the final output wont get bright or noisy cause the mask as lower values compared to above two cases;
                scale from 0 to 1 , scale=1 unsharp mask,scale>1 highboost filtering
                */
                vec4 blurPixel=vec4(getBlurredPixel(pos),0.0);
                tempFinalPixel=vec4(imageLoad(imageIn,pos))-blurPixel;//Unsharped mask
                tempFinalPixel*=scale;
                tempFinalPixel+=vec4(imageLoad(imageIn,pos));//final image;

            }break;
            case 3:
            {//same as above case but mask is minimum is set to zero
                int iteration=minMax[6];
                vec4 blurPixel=vec4(getBlurredPixel(pos),0.0);
                tempFinalPixel=vec4(imageLoad(imageIn,pos))-blurPixel;//Unsharped mask
                if(iteration==0)
                {
                    atomicMin(minMax[0],int(tempFinalPixel.r));
                    atomicMin(minMax[1],int(tempFinalPixel.g));
                    atomicMin(minMax[2],int(tempFinalPixel.b));
                    atomicMax(minMax[3],int(tempFinalPixel.r));
                    atomicMax(minMax[4],int(tempFinalPixel.g));
                    atomicMax(minMax[5],int(tempFinalPixel.b));
                }
                else
                {
                    //tempFinalPixel.rgb=scaleFilter(tempFinalPixel.rgb);///scale to 0- 255 ? as already most in range instead just make minumum to zero;
                    tempFinalPixel.rgb=tempFinalPixel.rgb-vec3(minMax[0],minMax[1],minMax[2]);
                    tempFinalPixel*=scale;
                    tempFinalPixel+=vec4(imageLoad(imageIn,pos));//final image;
                }

            }break;

            default:
            {
                imageStore(imageOut,pos,imageLoad(imageIn,pos));
            }


        }
        imageStore(imageOut, pos, uvec4(tempFinalPixel));
    }

}
float kdistance(ivec2 vect1,ivec2 vect2)
{
    vec2 dis=vec2(vect1-vect2);
    return sqrt(float(dis.x*dis.x+dis.y*dis.y));

}
vec3 calculateLaplacian(ivec2 pos)
{
    float centerWeight=8.0,otherWeight=-1.0;
    vec3 tempFinalPixel=vec3(0,0,0);
    uvec3 tempPixel=uvec3(0,0,0);
    for (int i=-1;i<2;i++)
    {
        for (int j=-1;j<2;j++)
        {
            ivec2 tempPos=pos + ivec2(i, j);
            if (tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)//to much work check to do befor for loop
            {
                tempPixel=imageLoad(imageIn, tempPos).rgb;//*uvec4((1,1,1,1));
                if (i==0&&j==0)
                {
                    tempFinalPixel+=centerWeight*vec3(tempPixel);
                }
                else
                {
                    tempFinalPixel+=otherWeight*vec3(tempPixel);
                }///////check different weights;//if diagonal neighbour not include use one else if for diagonals;
            }
        }
    }

return tempFinalPixel;
}
vec3 scaleFilter(vec3 inputFilter)
{
    vec3 minValues=vec3(minMax[0],minMax[1],minMax[2]);
    vec3 maxValues=vec3(minMax[3],minMax[4],minMax[5]);
    inputFilter.r=255.0*(inputFilter.r-minValues.r)/(maxValues.r-minValues.r);//scalinglaplacian image in range 0 -255;
    inputFilter.g=255.0*(inputFilter.g-minValues.g)/(maxValues.g-minValues.g);
    inputFilter.b=255.0*(inputFilter.b-minValues.b)/(maxValues.b-minValues.b);
    return inputFilter;
}

vec3 getBlurredPixel(ivec2 pos)
{
    vec3 blurredPixel=vec3(0.0,0.0,0.0);
    if(kdistance(center,pos)<=params[0]*5.0)
    {
        for(int i=-2;i<3;i++)
        {
            for(int j=-2;j<3;j++)
            {
                ivec2 tempPos=pos + ivec2(i,j);
                if(tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)
                {
                    blurredPixel+=vec3(imageLoad(imageIn,tempPos).rgb);
                }
            }
        }
        //  finalPixel.r=uint(255*25);
        blurredPixel/=25.0;//division not accurate so check
    }
    return blurredPixel;
}

/*
    requires input range of selection for eg.circle selection inner and outer radius;
    requires mask size for blurring in unsharp masking
    scale input required but depending on case convert to apprpriate range;
*/