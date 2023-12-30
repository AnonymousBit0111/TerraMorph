#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;


layout (location = 2) in vec4 matrixRow1;
layout (location = 3) in vec4 matrixRow2;
layout (location = 4) in vec4 matrixRow3;
layout (location = 5) in vec4 matrixRow4;

layout(location = 0) out vec4 fragColor;


layout( push_constant ) uniform constants
{
	mat4 data;
	mat4 render_matrix;
} PushConstants;



void main() {

    mat4 model = mat4(matrixRow1,matrixRow2,matrixRow3,matrixRow4);

    // gl_Position = vec4(inPosition.x,inPosition.y, inPosition.z, 1.0);
    gl_Position = PushConstants.render_matrix  *model*vec4(inPosition.x,inPosition.y, inPosition.z, 1.0);
    fragColor = inColor;
}