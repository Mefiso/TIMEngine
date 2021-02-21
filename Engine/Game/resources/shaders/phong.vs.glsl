#version 440
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 fragUV;
out vec3 fragNormal;
out vec3 fragPosWorld;

void main()
{
	gl_Position = proj * view * model * vec4(pos,1.0);
	fragNormal = transpose(inverse(mat3(model))) * normal;
	fragUV = uv;
	fragPosWorld = vec3(model * vec4(pos, 1.0));
}