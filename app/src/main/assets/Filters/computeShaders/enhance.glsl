#version 310 es
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
float logTranform(float intensity,float c);//check can be done on other as well ;
float powerTransform(float intensity, float c);
float contrastStretch(float intensity,float r1,float s1,float r2,float s2);
layout (rgba8ui,binding=0) uniform readonly highp uimage2D image;
layout (rgba8ui,binding=1) uniform writeonly  highp uimage2D imageout;
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
layout(location=0) uniform int filterType;
layout(location=5) uniform float params[4];
void main()
{
    int finalPixel;
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uint lid=gl_LocalInvocationIndex;
    uvec4 outp= imageLoad(image,pos);//////.rgba cool fx
    // pixel = removeGreen(pixel);
    // finalPixel=RgbaToInt(pixel);
    vec3 rgb=vec3(outp.xyz);
    vec3 hsi=rgbToHsi(rgb);
    switch(filterType)
    {
        case 0://light//should match in editor.cpp setActiveFilter;
        {
            hsi.b=hsi.b+params[0]*255.0/360.0-128.0;
        }break;
        case 1://saturation
        {
            hsi.g*=params[0]*10.0/360.0;
        }break;
        case 2://hue
        {
            hsi.r+=params[0];
        }break;
        case 3://log transform(contrast)
        {
            //if(hsi.b>1.0)//positive
            //  hsi.r=0.0;
            // hsi.g=0.0;
            hsi.b=logTranform(hsi.b,params[0]);//log///doint log and then power at same time changin inputs
            hsi.b=powerTransform(hsi.b,params[0]);
        }break;
        case 4://contrast streching
        {
            ///errro in this caus fragment shader inp differn thand compute input;
            // hsi.b=hsi.b+params[0]*255.0/360.0-128.0;
            hsi.b=contrastStretch(hsi.b,params[0],params[1],params[2],params[3]);

        }break;
        default:
        {hsi.b=hsi.b;}

    }
    rgb=hsiToRgb(hsi);
    uvec4 outpu=uvec4(rgb,outp.a);
    imageStore(imageout,pos,uvec4(outpu));
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

float logTranform(float intensity,float c)/////function not neded can be done above directly
{
    //out =c * log(1 + input) + constant; c=1 to L/log(1+L),L is max intense  value(255),constant optional dependening on situation
    //increase dynamic range of low intesity pixels while higher pixel move towarg gray; ?
    //reduce contrast of brighre pixels;

    c=1.0+c*60.0/360.0;    //therefore c=0-106 check;
    float finalIntensity=c * log(1.0+intensity);//precision of values check for undefined negative values
    ///ROundoff correctly to 255.0 as it should be in integer
    if(finalIntensity>255.0)
    finalIntensity=255.0;//intensity
    return finalIntensity ;
    //check is should be incremental (next input be present output?);
}
float powerTransform(float intensity, float c)
{
    //ouput= x* pow(input,gamma);
    //c>1 power c<1 log;
    //c or gamma 0.3 t oaround 5.0 acceptable or may be different but the beyond that will have unacceptable contrast
    if(c<=180.0)
    {
        c=c/180.0;//0/0 to 1.0
    }
    else
    {
        c=c*4.0/180.0;//1.0 to 2.0(replace to for more values
    }
    intensity/=255.0;
    float finalIntensity=pow(intensity,c);////multiplication not used check that.pow(x,y)-The result is undefined if x<0 or if x=0 and y≤0.
    if(finalIntensity>1.0)
    finalIntensity=1.0;
    return finalIntensity*255.0;
}
float contrastStretch(float intensity,float r1,float s1,float r2,float s2)
{
    //params=r1,s1,r2,s2;
    ////r1,s1 always less than equal to r2, s2
    // if(r1=r2)and s1=0 and s2=L-1 then its a thresolding function with giving binary image.
    //check (r1,s1=(rmin,0) and r2,s2=(rmax,L-1) or instead of rmax rmin uses average intensity


    float finalIntensity;
    if(intensity<=r1)
    {
        finalIntensity=s1*intensity/r1;
    }
    else if(intensity>r1&&intensity<=r2)
    {
        finalIntensity=s1 +(intensity-r1)*(s2-s1)/(r2-r1);
    }
    else
    {
        finalIntensity=s2+(intensity-r2)*(1.0-s2)/(1.0-s1);
    }
    return finalIntensity;
}