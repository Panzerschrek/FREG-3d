#version 120
uniform sampler2D tex;
uniform float inv_atlas_size= 1.0 / ( 2.0 * 16.0 * 128.0 );
varying vec2 f_tex_coord;
varying vec2 f_tex_coord_shift;
varying float f_light;
void main()
{
	vec2 tc0= f_tex_coord + mod( gl_FragCoord.xy, 2.0 ) * inv_atlas_size;// fake-filtration. Doesn`t work
	vec2 tc= mod( tc0, ( 1.0 / 16.0 ) ) + f_tex_coord_shift;
	vec4 c=  texture2D( tex, tc );
	if( c.a <= 0.5 )
		discard;
	gl_FragColor= c* f_light;
}
