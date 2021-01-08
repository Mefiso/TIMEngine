#version 430

out vec4 FragColor;

in vec3 texcoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, texcoords);
}