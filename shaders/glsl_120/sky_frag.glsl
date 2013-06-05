#version 120

uniform samplerCube cu; 
uniform vec3 sky_color= vec3( 4.78, 6.92, 12.32) * 0.1; 
uniform vec3 sun_vector;

varying vec3 view_vector;
varying float sun_vec_len;

const vec4 fog_color= vec4( 0.2, 0.2, 0.25, 1.0 );
void main( void )
{

   	vec3 tc= view_vector;
	tc= normalize( tc );
	vec3 c= textureCube( cu, tc ).xyz  * ( 1.0 - sun_vec_len );


	float sky_fact= ( 2.0f - tc.y ) * 0.5;
	vec3 sv;
	sv.x= sun_vector.x;
	sv.y= sun_vector.z;
	sv.z= -sun_vector.y;
	sky_fact+= 0.25 * pow( max( 0.0, dot( sv, tc ) ), 16.0 );
	sky_fact*= sun_vec_len;
	c+= sky_color * sky_fact;
	gl_FragColor= vec4( c , 1.0 );
}