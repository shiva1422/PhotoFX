#version 310 es
// NUM_X * NUM_Y * NUM_Z threads per work group.
//layout(local_size_x = NUM_X, local_size_y = NUM_Y, local_size_z = NUM_Z) in;
int  hueConvert(int pixel,float hup);
ivec4 IntToRgba(int pixel);
ivec4 removeGreen(ivec4 pixel);
int  RgbaToInt(ivec4 rgba);
layout(std430) buffer;
layout (rgba8ui,binding=0) uniform readonly highp uimage2D image;
layout (rgba8ui,binding=1) uniform writeonly highp uimage2D imageout;
layout(local_size_x = 1,local_size_y=1,local_size_z=1) in;
const float PI=3.14159265358979311599796346854;

void main()
{
    int finalPixel;
    ivec2 pos=ivec2(gl_GlobalInvocationID.xy);
    uint lid=gl_LocalInvocationIndex;
    uvec4 outp= imageLoad(image,pos);//////.rgba cool fx
    // pixel = removeGreen(pixel);
    // finalPixel=RgbaToInt(pixel);
    outp.x+=uint(50);
    if(outp.x>uint(255))outp.x=uint(255);
    outp.y+=uint(50);
    if(outp.y>uint(255))outp.y=uint(255);
    imageStore(imageout,pos,uvec4(outp.x,outp.yzw));
}
int  hueConvert(int pixel,float hup)
{
    float r,g,b,a,intensity,minRGB,hue,saturation,RADIAN=PI/180.0;
    a=float((((pixel>>24) & 0xFF)));
    b=float((((pixel>>16) & 0xFF)));
    g=float((((pixel>>8)  & 0xFF)));
    r=float((pixel & 0xFF ));
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
    hue=sqrt((r-g)*(r-g)+(r-b)*(g-b));
    hue=((r-g)+(r-b))/(2.0*hue);
    hue=acos(hue);
    hue=hue*180.0/PI;//hue in degrees
    if(b>g)
    {
        hue=360.0-hue;
    }
    //intensity+=hup;
    // if(intensity>255.0)
    //  intensity=255.0;

    saturation*=hup/255.0;
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
    else if(hue<120.0)
    {

        r=intensity*(1.0+( saturation*cos(hue*RADIAN)/cos((60.0-hue)*RADIAN)));
        b=intensity*(1.0-saturation);
        g=3.0*intensity-(b+r);

    }
    else
    {

        r = r+hup;
        g=g+hup;
        b=b+hup;
        //  BOOM("thre r g b are %lf %lf lf",r,g,b);



    }
    if(r>255.0)
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
    b=0.0;

    return RgbaToInt(ivec4(int(r),int(g),int(b),int(a)));
}
ivec4 IntToRgba(int pixel)
{
    ivec4 rgba;
    rgba.a=(((pixel>>24) & 0xFF));
    rgba.b=(((pixel>>16) & 0xFF));
    rgba.g=(((pixel>>8)  & 0xFF));
    rgba.r=(pixel & 0xFF );
    return rgba;
}
int  RgbaToInt(ivec4 pixel)
{
    int outPixel;
    pixel=pixel & 0xFF;
    outPixel=(pixel.a << 24) + (pixel.b << 16) +(pixel.g <<8 ) + (pixel.r);
    return outPixel;
}
ivec4 removeGreen(ivec4 pixel)
{
    if(pixel.g>90&&pixel.b<90&&pixel.r<120)
    {
        pixel.r=0;
        pixel.b=0;
        pixel.g=0;
    }
    return pixel;
}
