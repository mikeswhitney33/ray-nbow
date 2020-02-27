#version 330

in vec2 in_vert;

out vec3 raydir;

uniform float fov;
uniform float aspect;

void main() {
    float scale = tan(radians(fov) * 0.5);
    gl_Position = vec4(in_vert, 1.0, 1.0);
    raydir = vec3(in_vert.x * scale * aspect, in_vert.y * scale, -1);
}