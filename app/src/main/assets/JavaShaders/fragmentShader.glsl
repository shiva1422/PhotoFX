#version 310 es
precision mediump float;
uniform vec4 vColor;
out vec4 FragColor;
in vec2 uvs;
uniform sampler2D image;
void main() {
   vec2 finalTexCoods=vec2(uvs.x,1.0-uvs.y);
    vec4 finalColor= texture(image,finalTexCoods);
  FragColor =finalColor;
}