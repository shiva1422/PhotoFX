#version 310 es
layout(local_size_x = 16,local_size_y=8,local_size_z=1) in;//Thread Size;//min supported threads is 128;
float kdistance(ivec2 vect1,ivec2 vect2);
layout (rgba8ui,binding=0) uniform readonly highp uimage2D imageIn;
layout(rgba8ui,binding=1) uniform writeonly highp uimage2D imageOut;
layout(location=0) uniform float params[4];
const ivec2 workGroupSize=ivec2(16,8);//should be same as work group size above;
void main()
{
   // ivec2 workGroudId=ivec2(gl_GlobalInvocationID.xy);
   // ivec2 threadId=ivec2(gl_LocalInvocationID.xy);//in current WorkGroup////can also get 1d version using gl_LocalInvocation index;
    ivec2 pixelLoc=ivec2(gl_GlobalInvocationID.xy);//unique thread id in this dispatch;//as each thread access only one pixel it same for the pixelLocation;
    ivec2 imageDims=imageSize(imageIn);//should replace by uniform
    ivec2 centre=imageDims/2;
    uvec4 finalPixel=uvec4(0,0,0,0);
    //ivec2 pixelLoc=ivec2(pixelPos.x,pixelPos.y);//
      vec2 centerDif=vec2(pixelLoc-centre);
      float centerDis=sqrt(centerDif.x*centerDif.x+centerDif.y*centerDif.y);
        if (centerDis<=params[0]*15.0)
        {
            for (int i=-10;i<11;i++)
            {
                for (int j=-10;j<11;j++)
                {
                    ivec2 tempPos=pixelLoc + ivec2(i, j);
                   // if (tempPos.x>=0 &&tempPos.y>=0 &&tempPos.x<imageDims.x&&tempPos.y<imageDims.y)//this ccheck need but makes slow but work without this
                    //{//might crash for boundary pixels (neighbour pixle far thanmasksize);
                        finalPixel+=imageLoad(imageIn, tempPos);
                    //}
                }
            }
            //  finalPixel.r=uint(255*25);
            finalPixel/=uint(22*22);//division not accurate so check
            imageStore(imageOut, pixelLoc, uvec4(finalPixel.rgb, uint(255)));//////this alpha should be equal to input pixel;
        }
        else
        imageStore(imageOut, pixelLoc, imageLoad(imageIn, pixelLoc));

}
float kdistance(ivec2 vect1,ivec2 vect2)
{
    vec2 dis=vec2(vect1-vect2);
    return sqrt(float(dis.x*dis.x+dis.y*dis.y));

}