#version 330

uniform vec3 sun_vector= vec3( 0.05, 0.82, 0.517 );
uniform vec3 fog_color= vec3( 0.8, 0.8, 1.0 );
uniform sampler2DShadow shadow_map;
uniform sampler3D normal_map;
uniform float direct_sun_light= 20.0;
uniform vec3 cam_pos;
uniform float time;

in vec3 f_tex_coord;
in float f_light;
in vec3 f_normal;
in float fog_k;
in vec3 f_position;
in vec3 f_world_position;
out vec4 color;


const vec3 water_color= vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 );
const float inv_frenel_k= 1.0/1.6, inv_frenel_k2= 1.0/2.56;

void main( void )
{
   	float l= f_light + 0.01;
	

	vec3 n= texture( normal_map, vec3( f_tex_coord.xy, time * 0.25 ) ).xyz;
	n*=2.0;
	n-= vec3( 1.0, 1.0, 1.0 );
	n= mix( f_normal, n, abs( f_normal.z ) );	


	float shadow_fact= round( texture( shadow_map, f_position ) );
	float diffuse= max( dot( n, sun_vector )- 0.05, 0.0 );
	vec3 tocam_vec= normalize( f_world_position - cam_pos );
	vec3 tocam_vec_ref= reflect( tocam_vec, n );
	float specular= max( dot( tocam_vec_ref, sun_vector ), 0.0 );
	specular= pow( specular, 64.0 ) * 10.0;
	l+= direct_sun_light * shadow_fact * ( diffuse + specular );


	float a= dot( tocam_vec, n );
	a= abs( a );
	float b,c;
	b= a * inv_frenel_k;
	c=1.0 - a * a;
	c*= inv_frenel_k2;
	c=1.0 - c;
	c= sqrt( c );
	a= ( b - c ) / ( b + c );
	a= max( a * a, 0.1 );

	//l+= SunLight(n);
	//float t= Tansparency(n);
	color= vec4( water_color * l, a );

}
