#version 330

uniform samplerCube cu; 
uniform vec3 sky_color= vec3( 4.78, 6.92, 12.32) * 2.0; 
uniform vec3 sun_vector;

in vec3 view_vector;
in float sun_vec_len;
out vec4 color;

const vec4 fog_color= vec4( 0.2, 0.2, 0.25, 1.0 );
void main( void )
{

   	vec3 tc= view_vector;
	tc= normalize( tc );
	vec3 c= texture( cu, tc ).xyz * 0.5;


	float sky_fact= ( 2.0f - tc.y ) * 0.5;
	vec3 sv;
	sv.x= sun_vector.x;
	sv.y= sun_vector.z;
	sv.z= -sun_vector.y;
	sky_fact+= 0.25 * pow( max( 0.0f, dot( sv, tc ) ), 16.0f );
	c+= sky_color * sky_fact * sun_vec_len;
	color= vec4( c , 1.0 );
#ifndef FORCE_EARLY_Z
	gl_FragDepth= 1.0f;
#endif
}