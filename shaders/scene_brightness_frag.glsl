#version 330
uniform sampler2D scene_buffer;

in vec2 f_tex_coord;

out float color;

void main()
{
	vec3 b= texture( scene_buffer, f_tex_coord ).xyz;
	color= b.x + b.y + b.z;
}