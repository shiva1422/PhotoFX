#version 310 es
precision highp int;
precision highp float;
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
layout(location=1) uniform float param1;
layout(location=2) uniform float param2;
layout (location =21) uniform sampler2D image;
in vec2 uvOut;
out vec4 Fragcolor;
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
void main()
{
    vec4 finalColor;
    vec2 finalTexCoods=vec2(uvOut.x,1.0-uvOut.y);
    finalColor= texture(image,finalTexCoods);
   // finalColor.r=param1;
    vec3 hsi=rgbToHsi(vec3(finalColor.xyz));
    hsi.r=hsi.r+param1;
   vec3 rgb=hsiToRgb(hsi);
 //    finalColor.rgb=rgb-finalColor.rgb;
 //   if(finalColor.b==0.0)
   // finalColor.b=1.0;
    Fragcolor=vec4(rgb,finalColor.a);


}

vec3 rgbToHsi(vec3 rgb)//use seperate r,g,b than a vector;no extram memory and conversion needed
{
    float r=rgb.r*255.0,g=rgb.g*255.0,b=rgb.b*255.0,hue,saturation,minRGB,intensity;
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
    float hue=hsi.r,saturation=hsi.g,intensity=hsi.b,r,g,b;
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
    r=r/255.0;
    g=g/255.0;
    b=b/255.0;
    return vec3(r,g,b);
}
