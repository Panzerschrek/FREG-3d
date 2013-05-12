#version 120
uniform sampler2D tex;
varying vec2 f_tex_coord;
varying vec2 f_tex_coord_shift;
varying float f_light;
void main()
{
	vec2 tc= mod( f_tex_coord, ( 1.0 / 16.0 ) ) + f_tex_coord_shift;
	vec4 c=  texture2D( tex, tc );
	if( c.a <= 0.5 )
		discard;
	gl_FragColor= c * f_light;
}
