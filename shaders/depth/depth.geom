#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoords;  // input from vertex shader
} gs_in[];

out vec2 TexCoords; // output for fragment shader

void main() {
    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].TexCoords;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].TexCoords;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].TexCoords;
    EmitVertex();
    EndPrimitive();
}