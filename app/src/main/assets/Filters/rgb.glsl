#version 310 es
precision highp int;
precision highp float;
layout(location=1) uniform float param1;
layout(location=2) uniform float param2;
layout (location =21) uniform sampler2D image;
in vec2 uvOut;
out vec4 Fragcolor;
void main()
{
    vec4 finalColor;
    vec2 finalTexCoods=vec2(uvOut.x,1.0-uvOut.y);
    finalColor= texture(image,finalTexCoods);
    finalColor.r=param1;
    Fragcolor=vec4(finalColor);


}