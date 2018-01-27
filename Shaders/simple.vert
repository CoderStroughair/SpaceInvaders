#version 410
layout(location = 0) in vec2 vp;
layout(location = 1) in vec2 vt;

uniform mat4 model;

out vec2 texture_coords;

void main () 
{
	gl_Position = model * vec4 (vp, 0.0, 1.0);
	texture_coords = vt;
}
