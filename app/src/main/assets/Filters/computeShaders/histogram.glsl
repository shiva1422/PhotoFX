#version 310 es
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
layout(std430) buffer;
layout (rgba8ui,binding=0) uniform readonly highp uimage2D image;
layout (std430, binding=1) buffer binsDat
{
    int bins[360];
};
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
layout(location=0) uniform int filterType;
void main()
{
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    //uint lid=gl_LocalInvocationIndex;
    uvec4 inPix= imageLoad(image,pos);
    vec3 rgb=vec3(inPix.xyz);
    vec3 hsi=rgbToHsi(rgb);////Thes covertion no need for rgb histograms
    switch (filterType)//first 6 for histogramequ; try equalize whole rgb ;
    {

        case 0://for R cases correpond to auto.glsl
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
        case 6:////Cmy has same histograms like red except they are inverted;
        {
            uint cyan=uint(255)-inPix.r;//instead of 255-inpix her doe in auto.glsl check
            atomicAdd(bins[uint(cyan)],1);
        }break;
        case 7:
        {
            uint magenta=uint(255)-inPix.g;
            atomicAdd(bins[uint(magenta)],1);
        }break;
        case 8:
        {
            uint magenta=uint(255)-inPix.b;
            atomicAdd(bins[uint(magenta)],1);
        }break;
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
