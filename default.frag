#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_basecolor1;

void main()
{
    vec4 base = texture(texture_basecolor1, TexCoords);
    vec4 fallback = texture(texture_diffuse1, TexCoords);
    
    vec4 texColor = base;
    if (length(texColor.rgb) < 0.001) texColor = fallback;
    if (length(texColor.rgb) < 0.001) texColor = vec4(0.0, 1.0, 0.0, 1.0); // fallback verde
    
    if (texColor.a == 1.0 && texture(texture_diffuse1, TexCoords).a < 0.95)
        texColor.a = texture(texture_diffuse1, TexCoords).a;

    if (texColor.a < 0.1)
        discard;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}
