#version 310 es
in vec4 vPosition;
in vec2 texCoords;////uvs
out vec2 uvs;
void main() {
gl_Position = vPosition;
    uvs=texCoords;
}