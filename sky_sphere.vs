#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 p = normalize(aPos);
    // proyección equirectangular:
    float rawU = atan(-p.z, p.x) / (2.0 * 3.1415926) + 0.5;
    float u = fract(rawU);
    // recortamos para que quede siempre en (0,1), nunca exactamente en el borde:
    u = clamp(u, 0.001, 0.999);

    float v = acos(p.y) / 3.1415926;
    // opcionalmente clampa v también:
    v = clamp(v, 0.001, 0.999);

    TexCoord = vec2(u, v);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
