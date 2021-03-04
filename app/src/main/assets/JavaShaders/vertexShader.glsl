#version 310 es
in vec2 position;
in vec2 texCoords;////uvs
uniform vec2 displayParams;//width and height
out vec2 uvs;
void main() {
    int index=gl_InstanceID;
    vec2 finalPosition;
    finalPosition.x=position.x+(float(index%3))*0.68;
    finalPosition.y=position.y-(float(index/3))*0.45;
gl_Position = vec4(finalPosition,0.0,1.0);
    uvs=texCoords;
}