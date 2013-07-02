#version 330

uniform vec3 sun_vector= vec3( 0.05, 0.82, 0.517 );
uniform vec3 fog_color= vec3( 0.8, 0.8, 1.0 );
uniform sampler2DShadow shadow_map;
uniform sampler3D normal_map;
uniform sampler2D depth_buffer;
uniform float direct_sun_light= 20.0;
uniform vec3 cam_pos;
uniform float time;

in vec2 f_tex_coord;
in float f_light;
in vec3 f_normal;
in float fog_k;
in vec3 f_position;
in vec3 f_world_position;
in float f_depth;
out vec4 color;


uniform vec3 depth_convert_k;
uniform float underwater_fog_factor;
uniform vec3 inv_screen_size;
uniform float inv_max_view_distance= 0.03125;
uniform vec3 viewport_scale;

float LinearFragDepth( vec2 tc )
{
    return depth_convert_k.x / ( texture( depth_buffer , tc ).x + depth_convert_k.y );
}


const vec3 water_color= vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 );
const vec3 water_deep_color= vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 ) * 0.5;
const float inv_frenel_k= 1.0/1.6, inv_frenel_k2= 1.0/2.56;

void main( void )
{
   	float l= f_light + 0.01;


	vec3 n= texture( normal_map, vec3( f_tex_coord, time * 0.25 ) ).xyz;
	//n+= texture( normal_map, vec3( f_tex_coord * 0.25, time * 0.25 ) ).xyz;
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

	vec3 screen_coord= vec3( viewport_scale.xy * ( 2.0 * gl_FragCoord.xy * inv_screen_size.xy - vec2( 1.0, 1.0 ) ), 1.0 );
	vec3 world_pos= screen_coord * ( LinearFragDepth( gl_FragCoord.xy * inv_screen_size.xy ) - f_depth );
	float depth_delta= length( world_pos );
	float a2= max( underwater_fog_factor * depth_delta * inv_max_view_distance, 0.0 );
	a2=min( a2, 1.0 );

	
	vec3 final_color= l * water_color * a + a2 * water_deep_color * f_light;
	color= vec4( final_color , a2 );
}
