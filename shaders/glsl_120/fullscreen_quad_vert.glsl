#version 120

attribute vec2 coord;

varying vec2 f_tex_coord;

void main()
{
	f_tex_coord= coord * 0.5 + vec2( 0.5, 0.5 );
	gl_Position= vec4( coord, 1.0, 1.0 );
}