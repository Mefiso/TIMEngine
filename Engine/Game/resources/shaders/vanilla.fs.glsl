#version 430

out vec4 color;

in vec2 uv0;

uniform sampler2D diffuse1;

void main()
{
 color = texture2D(diffuse1, uv0);
}
