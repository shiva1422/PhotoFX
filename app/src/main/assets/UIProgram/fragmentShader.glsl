#version 310 es
precision mediump int;
precision mediump float;
precision mediump sampler2DArray;
in vec4 colorOut;
in vec2 textCoodsOut;
out vec4 Fragcolor; 
in float texZ;
uniform sampler2D image;
uniform sampler2D image2;
layout(location=0) uniform int drawType;
layout(location = 7)uniform sampler2DArray images;
void main()
{


    vec4 finalColor;//=texture(bassTex,textCoodsOut);
    switch(drawType)
    {

        case 0:
        {
            finalColor= texture(image,textCoodsOut);
        }
        break;
        case 1:
        {
            finalColor=texture(images,vec3(textCoodsOut,2.0));
        }
        break;
    }

    Fragcolor=finalColor;
}
