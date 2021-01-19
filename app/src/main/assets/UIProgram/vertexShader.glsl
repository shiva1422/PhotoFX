#version 310 es
precision mediump float;
precision mediump int;
vec4 setWhiteKeyColor(int TopOrDown,int Index);
layout(location = 0) in vec2 verts;
layout(location = 1)  in vec4 color;
layout(location = 2)  in vec2 texCoords;
layout(location = 3)  in float texZValue;
layout(location=0) uniform int drawType;
layout(location=1) uniform float param1;
layout(location=2) uniform float param2;

out vec2 textCoodsOut;
out vec4 colorOut;
out float texZ;
vec2 finalVerts;
float instanceId;
void main()
{
    instanceId=float(gl_InstanceID);
    switch(drawType)
    {
        case 0://imageView
        {
            finalVerts.x=verts.x;
            finalVerts.y=verts.y;
        }
        break;
        case 1://ImageViewStack
        {
            //param1-viewGap,param2-viewWidht

            finalVerts.x=verts.x+(instanceId+1.0)*param1+instanceId*param2;
            finalVerts.y=verts.y;

        }
        break;
    }


    gl_Position=vec4(finalVerts.x, finalVerts.y, 1.0, 1.0);
    textCoodsOut=texCoords;
    texZ=texZValue;
}

///function defs
vec4 setWhiteKeyColor(int TopOrDown,int Index)
{
    int x;
    vec4 whiteKeyColor;
    return whiteKeyColor;
}