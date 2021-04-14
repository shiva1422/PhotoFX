#version 310 es
precision highp float;
precision highp int;
vec4 setWhiteKeyColor(int TopOrDown,int Index);
layout(location = 0) in vec2 verts;///instead of location cache when first time gluse program
layout(location = 1)  in vec4 color;
layout(location = 2)  in vec2 texCoords;////uvs
layout(location = 3)  in float texZValue;
layout(location=0) uniform int drawType;
layout(location=1) uniform float param1;
layout(location=2) uniform float param2;
layout(location=3) uniform int frameBuf;///////check if needed else remove
layout(location=4) uniform int param3;
layout(location=5) uniform int param4;
uniform vec4 uniformColor;
uniform sampler2D image;
layout(std140,binding=2) uniform binsData
{
    int bins[256];
};
out vec2 textCoodsOut;
out vec4 colorOut;
out float texZ;
flat out int index;
vec2 finalVerts;
float instanceId;
void main()
{
    gl_PointSize=10.0;////////remove
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
            //param1-viewGap,param2-viewWidht,param3,active View;

            finalVerts.x=verts.x+(instanceId+1.0)*param1+instanceId*param2;
            finalVerts.y=verts.y;


        }
        break;
        case 2://histogram;////now histogram is acturally drawn in default check in drawHistogram
        {
            finalVerts.x=verts.x+(instanceId*2.0/256.0);
            finalVerts.y=100.0*float(bins[gl_InstanceID]);
           // finalVerts.y=float(gl_InstanceID)/256.0;

        }
        break;
        case 3://shapes;
        {
            finalVerts=verts;
            colorOut=uniformColor;
        }break;
        default:
        {
            finalVerts.x=verts.x;
            finalVerts.y=verts.y;
            colorOut=uniformColor;
        }
    }



        textCoodsOut=texCoords;

     //if(frameBuf==1)
   // {
       // textCoodsOut=texCoords;
       //textCoodsOut.x=texCoords.x;
      // textCoodsOut.y=1.0-texCoords.y;
      // finalVerts.y=finalVerts.y*-1.0;
   // }

    texZ=texZValue;
    index=gl_InstanceID;
    gl_Position=vec4(finalVerts.x, finalVerts.y, 1.0, 1.0);


}

///function defs
vec4 setWhiteKeyColor(int TopOrDown,int Index)
{
    int x;
    vec4 whiteKeyColor;
    return whiteKeyColor;
}