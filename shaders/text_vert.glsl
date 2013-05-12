#version 330
uniform sampler2D tex;

in vec2 coord;
in vec2 tex_coord;
in vec3 color;

out vec3 f_color;
out vec2 f_tex_coord;
void main()
{
	ivec2 tex_size= textureSize( tex, 0 );
	f_tex_coord= tex_coord;
	f_color= color;
	gl_Position= vec4( coord, -1.0, 1.0 );
}