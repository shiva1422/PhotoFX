#version 310 es
in vec2 position;
in vec2 texCoords;////uvs
uniform vec2 displayParams;//width and height
layout(location=5)uniform int imageNum;
uniform int numRows;
uniform int numCols;
out vec2 uvs;
void main() {
  //  int index=gl_InstanceID;
    int index=imageNum;
    vec2 finalPosition;
    finalPosition.x=position.x+(float((index)%numCols))*2.0/float(numCols);//0.1 for gap//2/cols whole screenwidth is 2 image width =2/nocols
    finalPosition.y=position.y-(float((index)/numCols))*2.0/float(numCols);
gl_Position = vec4(finalPosition,0.0,1.0);
    uvs=texCoords;
}