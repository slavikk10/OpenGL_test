#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VERTEX_OUT {
    vec3 normal;
} vert_out;

uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vert_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}