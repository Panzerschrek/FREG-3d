#version 330

out vec4 color;

in vec2 f_tex_coord;
in vec3 f_color;
in float f_light;
uniform sampler2D tex;

void main()
{
	vec4 c= texture( tex, f_tex_coord );
	//if( c.a < 0.01 )
	//	discard;
	c.xyz *= f_color * f_light;
	color= c;
	//color= vec4( 10.0, 10.0, 10.0, 0.5 );
}