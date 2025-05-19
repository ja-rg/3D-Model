#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord; // Necesitas pasarlo desde el vertex shader

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D baseColorTexture;

out vec4 FragColor;

void main()
{

    FragColor = vec4(color, 1.0);
}
