#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
layout(std430) buffer;
float kdistance(ivec2 vect1,ivec2 vect2);
vec4 clipToRange(vec4 inputVec,float lowest , float highest);
vec3 calculateLaplacian(ivec2 pos);
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
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uint lid=gl_LocalInvocationIndex;
    vec4 tempFinalPixel=vec4(0,0,0,0);
    float scale=(params[1]*2.0/360.0);//check scale range;
    imageDims=imageSize(imageIn);
    center=imageDims/2;
    if (kdistance(center,pos)<=params[0]*5.0)
    {
        tempFinalPixel=vec4(calculateLaplacian(pos),255.0);//alpha value should be same as input;
    }
    else
    {
        tempFinalPixel=vec4(imageLoad(imageIn,pos));
        imageStore(imageOut, pos, uvec4(tempFinalPixel));
        return;
    }
    switch(filterType)
    {
        case 0://non scaled laplacian;(laplace not in range 0 - 255 or as required;
        {
            tempFinalPixel*=scale;//scale to 0 - 255;//just the high freq image or laplacian or edges
         //   tempFinalPixel=clipToRange(tempFinalPixel, 0.0, 255.0);
          //  tempFinalPixel+=vec4(imageLoad(imageIn,pos));//sharpened result//scale 0 - 255
         //   tempFinalPixel=clipToRange(tempFinalPixel, 0.0, 255.0);

        }break;
        case 1://scaled laplacian
        {
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
                vec3 minValues=vec3(minMax[0],minMax[1],minMax[2]);
                vec3 maxValues=vec3(minMax[3],minMax[4],minMax[5]);
                tempFinalPixel.r=255.0*(tempFinalPixel.r-minValues.r)/(maxValues.r-minValues.r);//scalinglaplacian image in range 0 -255;
                tempFinalPixel.g=255.0*(tempFinalPixel.g-minValues.g)/(maxValues.g-minValues.g);
                tempFinalPixel.b=255.0*(tempFinalPixel.b-minValues.b)/(maxValues.b-minValues.b);

             //   tempFinalPixel=vec4(255.0,255.0,255.0,255.0)-tempFinalPixel;
                tempFinalPixel*=scale;
               // tempFinalPixel=clipToRange(tempFinalPixel, 0.0, 255.0);
               // tempFinalPixel+=vec4(imageLoad(imageIn,pos));//sharpened result//scale 0 - 255
                //tempFinalPixel=clipToRange(tempFinalPixel, 0.0, 255.0);

            }
        }break;
        case 3://UnsharpMasked;
        {

        }break;
            default:
            {
                return;
            }


    }
    imageStore(imageOut, pos, uvec4(tempFinalPixel));

    /* int iteration=atomicAdd(minMax[6],0);
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
            // tempFinalPixel=ivec4(imageLoad(imageIn,pos))+tempFinalPixel;//sharpened result;
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
     imageStore(imageOut,pos,imageLoad(imageIn,pos));*/


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
            if (tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)
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
vec4 clipToRange(vec4 inputVec,float lowest ,float highest)
{
    if(inputVec.r>255.0||inputVec.g>255.0||inputVec.b>255.0)
    {
        inputVec.rgb=vec3(255.0,255.0,255.0);//just so that no color component is visible or set to minimum of three or average instead of 255.0;
        return inputVec;
    }
    else if(inputVec.r<0.0||inputVec.g<0.0||inputVec.b<0.0)
    {
        inputVec.rgb=vec3(0.0,0.0,0.0);
        return inputVec;
    }
    if(inputVec.r<0.0)
    {
        inputVec.r=0.0;
    }
    if(inputVec.g<0.0)
    {
        inputVec.g=0.0;
    }
    if(inputVec.b<0.0)
    {
        inputVec.b=0.0;
    }
    if(inputVec.r>255.0)
    {
        inputVec.r=255.0;
    }
    if(inputVec.g>255.0)
    {
        inputVec.g=255.0;
    }
    if(inputVec.b>255.0)
    {
        inputVec.b=255.0;
    }
    return inputVec;
}
