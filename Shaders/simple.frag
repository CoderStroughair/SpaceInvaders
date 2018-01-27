#version 410

in vec2 texture_coords;

uniform sampler2D texture_primary;
out vec4 frag_colour;
void main () 
{
	frag_colour = texture(texture_primary, texture_coords);
}