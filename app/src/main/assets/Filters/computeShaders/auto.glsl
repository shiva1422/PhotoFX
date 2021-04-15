#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
layout(std430) buffer;
layout (rgba8ui,binding=0) uniform readonly highp uimage2D imageIn;//image//image unifroms are supported in fragment shaders so try equalize in fragemnt shader;
layout(rgba8ui,binding=1) uniform writeonly highp uimage2D imageOut;
layout (std430,binding=2) buffer binsData
{
 int eqVals[256];//values after equalization not bins;
};
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
layout(location=0) uniform int autoType;
layout(location=1) uniform int paramInt;
layout(location=2) uniform int inOrOut;//to determina if bins for input or output image;
layout(location=5) uniform float params[4];///no need for eq has it has no sliders
void main()
{
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uvec4 inPix= imageLoad(imageIn,pos);
    vec3 rgb=vec3(inPix.xyz);
    vec3 hsi=rgbToHsi(rgb);////Thes covertion no need for rgb histograms
    uvec4 outPix;
    switch(autoType)//first 6 for histogramequ;//////reduce duplicated code;
    {

            case 0:///cases should correspond to in histogram.glsl
            {
                outPix.r=uint(eqVals[inPix.r]);
                outPix.gba=inPix.gba;
                imageStore(imageOut,pos,outPix);

            }break;
            case 1:
            {
                outPix.g=uint(eqVals[inPix.g]);
                outPix.rba=inPix.rba;
                imageStore(imageOut,pos,outPix);
            }break;
            case 2 :
            {
                outPix.b=uint(eqVals[inPix.b]);
                outPix.rga=inPix.rga;
                imageStore(imageOut,pos,outPix);
            }break;
            case 3:
            {
                hsi.b=float(eqVals[int(hsi.b)]);//check if conversions are correct;
                rgb=hsiToRgb(hsi);
                imageStore(imageOut,pos,uvec4(rgb,inPix.a));///move this from all cases to out of switch
            }break;
            case 4:
            {
                hsi.r=float(eqVals[uint(hsi.r)]);/////hue values are rounded to ints so check to add /sub difference; for intermediat hues;
                outPix.rgb=uvec3(hsiToRgb(hsi));
                outPix.a=inPix.a;
                imageStore(imageOut,pos,outPix);
            }break;
            case 5:
            {
                float tempSat=hsi.g*255.0;
                hsi.g=float(eqVals[uint(tempSat)]);
                hsi.g=hsi.g/255.0;
                outPix.rgb=uvec3(hsiToRgb(hsi));
                outPix.a=inPix.a;
                imageStore(imageOut,pos,outPix);

            }break;
            case 6://C
            {
                outPix.r=uint(eqVals[inPix.r]);//check 255-uint(eq) for al cases
                outPix.gba=inPix.gba;
                imageStore(imageOut,pos,outPix);
            }break;
            case 7://M
            {
                outPix.g=uint(eqVals[inPix.g]);//check 255-uint(eq) for al cases
                outPix.rba=inPix.rba;
                imageStore(imageOut,pos,outPix);

            }break;
            case 8://Y
            {
                outPix.b=uint(eqVals[inPix.b]);//check 255-uint(eq) for al cases
                outPix.rga=inPix.rga;
                imageStore(imageOut,pos,outPix);

            }break;
            case 9:
            {
                imageStore(imageOut,pos,inPix.brga);

            }break;
            case 10:
            {
                imageStore(imageOut,pos,inPix.bgra);
            }break;

            default:
            {
                imageStore(imageOut,pos,inPix);

            }
        }

}
vec3 rgbToHsi(vec3 rgb)//use seperate r,g,b than a vector;no extram memory and conversion needed
{
    //intesity 0-255,saturation 0-1.0,hue=0-360
    float r=rgb.r,g=rgb.g,b=rgb.b,hue,saturation,minRGB,intensity;
    intensity=(r+g+b)/3.0;
    if(r<=g&&r<=b)////////////min of RGB
    {
        minRGB=r;
    }
    else if(g<=b)
    {
        minRGB=g;
    }
    else minRGB=b;
    saturation=1.0-(minRGB)/(intensity);
    if(r==b&&r==g)
    {
        hue=0.0;
        saturation=0.0;
    }
    else
    {
        float rootover=(r-g)*(r-g)+(r-b)*(g-b);
        hue=sqrt(rootover);
        hue=((r-g)+(r-b))/(2.0*hue);
        hue=acos(hue);
        hue=hue*180.0/PI;//hue in degrees
        //  if(rootover<0.0)
        // hue=0.0;

    }
    if(isnan(hue)||isinf(hue))
    {
        // hue=0.0;
    }
    if(b>g)
    {
        hue=360.0-hue;
    }
    return vec3(hue,saturation,intensity);

}
vec3 hsiToRgb(vec3 hsi)
{
    float hue=hsi.r,saturation=hsi.g,intensity=hsi.b,r,g,b;///differe in fragment shader *255.0
    if(hue>360.0)
    hue-=360.0;
    if(hue>=120.0&&hue<240.0)//////////new RGB values after hue conversion
    {

        hue-=120.0;
        r=intensity*(1.0-saturation);///////new rgb
        g=intensity*(1.0+( saturation*cos(hue*RADIAN)/cos((60.0-hue)*RADIAN)));
        b=3.0*intensity-(g+r);

    }
    else if (hue>=240.0&&hue<360.0)
    {
        hue-=240.0;
        g=intensity*(1.0-saturation);
        b=intensity*(1.0+( saturation*cos(hue*RADIAN)/cos((60.0-hue)*RADIAN)));
        r=3.0*intensity-(g+b);

    }
    else if(hue<=120.0)
    {

        r=intensity*(1.0+( saturation*cos(hue*RADIAN)/cos((60.0-hue)*RADIAN)));
        b=intensity*(1.0-saturation);
        g=3.0*intensity-(b+r);

    }
    else
    {
        //this else only for checking error remove after everythhing is set
        //means the hue is nan or infinity check why this happen try prevent or make color as gray equal to intensity;
        r=intensity;
        g=r;
        b=r;
    }
    /*  if(r>255.0)
      r=255.0;
      else if (r<0.0)
      r=0.0;
      if(g>255.0)
      g=255.0;
      else if(g<0.0)
      g=0.0;
      if(b>255.0)
      b=255.0;
      else if(b<0.0)
      b=0.0;*/
    // r=r/255.0;//////////required in fragmentShader;
    //  g=g/255.0;
    // b=b/255.0;
    return vec3(r,g,b);
}
