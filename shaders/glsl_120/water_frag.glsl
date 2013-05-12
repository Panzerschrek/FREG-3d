#version 120

uniform sampler3D normal_map;
uniform float time;
uniform vec3 cam_pos;

varying vec2 f_tex_coord;
varying float f_light;
varying vec3 f_world_position;


const vec3 water_color= vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 );
const float inv_frenel_k= 1.0/1.6, inv_frenel_k2= 1.0/2.56;

void main( void )
{
   	float l= f_light + 0.01;
	

	vec3 n= texture3D( normal_map, vec3( f_tex_coord, time * 0.25 ) ).xyz;
	n*=2.0;
	n-= vec3( 1.0, 1.0, 1.0 );
	//n= mix( f_normal, n, abs( f_normal.z ) );	

	vec3 tocam_vec= normalize( cam_pos - f_world_position );

	float a= dot( tocam_vec, n );
	a= abs( a );
	float b,c;
	b= a * inv_frenel_k;
	c=1.0 - a * a;
	c*= inv_frenel_k2;
	c=1.0 - c;
	c= sqrt( c );
	a= ( b - c ) / ( b + c );
	a= max( a * a, 0.2 );

	gl_FragColor= vec4( water_color * l, a );

}
