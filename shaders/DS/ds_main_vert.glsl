#version 330

out vec2 f_tex_coord;
out vec2 f_screen_coord;

in vec2 pos;

uniform vec3 viewport_scale;
void main()
{	
	f_tex_coord= pos * 0.5 + vec2( 0.5, 0.5 );
	f_screen_coord= pos * viewport_scale.xy;
	gl_Position= vec4( pos, 1.0, 1.0 );
}