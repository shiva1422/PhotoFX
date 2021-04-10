#version 310 es
precision highp int;
precision highp float;
precision highp sampler2DArray;
in vec4 colorOut;
in vec2 textCoodsOut;
out vec4 Fragcolor; 
in float texZ;
uniform sampler2D image;
uniform sampler2D image2;
layout(location=0) uniform int drawType;
layout(location=4) uniform int param3;
layout(location = 7)uniform sampler2DArray images;

void main()
{


    vec4 finalColor;//=texture(bassTex,textCoodsOut);
    vec2 finalTexCoods=vec2(textCoodsOut.x,1.0-textCoodsOut.y);//cause tex cood start at botoom whare as image from top
    switch(drawType)
    {

        case 0:
        {
            finalColor= texture(image,finalTexCoods);
        }
        break;
        case 1:
        {
            finalColor=texture(images,vec3(finalTexCoods,2.0));
        }
        case 3:
        {
            finalColor=colorOut;
        }
        break;
        default:
        {
            finalColor=vec4(colorOut.rgb,1.0);
        }
    }
   //finalColor=vec4(1.0,1.0,1.0,1.0);
    Fragcolor=finalColor;
}
