#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform sampler2D texSampler;

out vec4 FragColor;

void main()
{
    vec3 color = texture(texSampler, TexCoord).rgb;

    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * color;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
