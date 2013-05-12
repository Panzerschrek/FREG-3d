#version 120
uniform sampler2D tex;

varying vec3 f_color;
varying vec2 f_tex_coord;


void main()
{
	float c= texture2D( tex, f_tex_coord ).x;
	gl_FragColor= vec4( c * f_color, c * 0.5 + 0.5 );
}
