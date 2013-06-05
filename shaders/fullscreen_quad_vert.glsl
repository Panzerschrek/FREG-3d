#version 330
uniform vec3 viewport_scale;

in vec2 coord;

out vec2 f_tex_coord;
out vec2 f_screen_coord;

void main()
{
	f_tex_coord= coord * 0.5 + vec2( 0.5, 0.5 );
	f_screen_coord= coord * viewport_scale.xy;
	gl_Position= vec4( coord, 1.0, 1.0 );
}