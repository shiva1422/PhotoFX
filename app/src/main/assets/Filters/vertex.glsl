#version 310 es
precision highp int;
precision highp float;
precision highp iimage2D;
layout(location = 0) in vec2 verts;
layout(location = 2)  in vec2 uv;//all locations same as in //UIShader;
layout (location =21) uniform sampler2D image;
//layout(location=1) uniform float param1;
//layout(location=2) uniform float param2;
layout (r32i,binding=0) uniform readonly iimage2D binImage;//to store bins
layout (r32i,binding=1) uniform writeonly iimage2D inputTex;//shader storage blocks not supported so using imagevars//bind texturs buffer
layout(location=3) uniform int frameBuf;
out vec2 uvOut;//can have layout location matching in in fragment shader event with diff name in frag shader
void main()
{
    vec2 finalVerts;
    finalVerts=verts;
    uvOut=uv;
    if(frameBuf==1)
    {
        finalVerts.y=finalVerts.y*-1.0;
    }
    gl_Position=vec4(finalVerts.x, finalVerts.y, 1.0, 1.0);

}