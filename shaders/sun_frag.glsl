#version 330

uniform sampler2D sun_tex;

in vec2 f_tex_coord;
in float angle_cos;
out vec4 color;

void main()
{
	vec4 c=  texture( sun_tex, f_tex_coord );
	color= vec4( exp( c.xyz * 5.0f ) - vec3( 1.0f, 1.0f, 1.0f ), c.a  );
#ifndef FORCE_EARLY_Z
	gl_FragDepth= 1.0f;
#endif
}