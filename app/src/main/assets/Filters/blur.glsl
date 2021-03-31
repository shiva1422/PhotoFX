#version 310 es
precision highp int;
precision highp float;
vec3 rgbToHsi(vec3 rgb);
vec3 hsiToRgb(vec3 hsi);
float logTranform(float intensity,float c);//check can be done on other as well ;
float powerTransform(float intensity, float c);
float contrastStretch(float intensity,float r1,float s1,float r2,float s2);
layout(location=0) uniform int filterType;
layout(location=5) uniform float params[4];
//layout(location=3) uniform float param3;//location name different from uishader so make filter shaders independent of ui locatoin
layout (location =21) uniform sampler2D image;
in vec2 uvOut;//can have layout location same as out uvOut in vertexShader and the name  can be differen(still matches)
out vec4 Fragcolor;////this can have loayout location as buffer index that this writes to
const float PI=3.14159265358979311599796346854;
const float RADIAN=PI/180.0;
void main()
{
    vec4 finalColor;
    vec2 finalTexCoods=vec2(uvOut.x,1.0-uvOut.y);
    finalColor= texture(image,finalTexCoods);
    vec3 hsi=rgbToHsi(vec3(finalColor.xyz));
    switch(filterType)
    {
        case 0:
        {

        }
    }
    // hsi.g=hsi.g*param1*5.0/360.0;
    // float hue=hsi.r;
    // hsi.r=param1;
    vec3 rgb=hsiToRgb(hsi);
    Fragcolor=vec4(rgb,finalColor.a);


}
