#version 310 es
precision highp int;
precision highp float;
precision highp sampler2DArray;
in vec4 colorOut;
in vec2 textCoodsOut;
flat in int index;
out vec4 Fragcolor; 
in float texZ;
in float centreX,centreY;
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
            if(finalColor.a<0.8)//circular images;
            {
                discard;
            }
        }
        break;
        case 1:
        {
            finalColor=texture(images,vec3(finalTexCoods,float(index)));

            if(finalColor.a<0.8)//circular images;
            {
                if(index==param3)
                {
                    finalColor=vec4(0.5,0.5,0.5,1.0);
                }
                else
                discard;
            }
        }
        break;
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
