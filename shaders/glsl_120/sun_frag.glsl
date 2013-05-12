#version 120

uniform sampler2D sun_tex;

varying vec2 f_tex_coord;
void main()
{
	gl_FragColor=  texture2D( sun_tex, f_tex_coord );
}