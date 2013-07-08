#version 120

uniform sampler2D depth_buffer;
uniform sampler3D normal_map;
uniform float time;
uniform vec3 cam_pos;

varying vec2 f_tex_coord;
varying float f_light;
varying vec3 f_world_position;
varying vec3 f_normal;
varying float f_depth;


const vec3 water_color= vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 );
const vec3 water_deep_color= vec3( 43.0/255.0,82.0/255.0, 198.0/255.0 ) * 0.5;
const float inv_frenel_k= 1.0/1.6, inv_frenel_k2= 1.0/2.56;

uniform vec3 depth_convert_k;
uniform float underwater_fog_factor;
uniform vec3 inv_screen_size;
uniform float inv_max_view_distance= 0.03125;
uniform vec3 viewport_scale;

float LinearFragDepth( vec2 tc )
{
    return depth_convert_k.x / ( texture2D( depth_buffer , tc ).x + depth_convert_k.y );
}


void main( void )
{
   	float l= f_light + 0.01;
	

	vec3 n= texture3D( normal_map, vec3( f_tex_coord, time * 0.25 ) ).xyz;
	n*=2.0;
	n-= vec3( 1.0, 1.0, 1.0 );
	n= mix( f_normal, n, abs( f_normal.z ) );	

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
	a= max( a * a + 0.1, 0.2 );

	vec3 screen_coord= vec3( viewport_scale.xy * ( 2.0 * gl_FragCoord.xy * inv_screen_size.xy - vec2( 1.0, 1.0 ) ), 1.0 );
	vec3 world_pos= screen_coord * ( LinearFragDepth( gl_FragCoord.xy * inv_screen_size.xy ) - f_depth );
	float depth_delta= length( world_pos );
	float a2= min( depth_delta * inv_max_view_distance, 1.0 );

	vec3 final_color= l * ( water_color * a + ( 1.0 - a ) * a2 * water_deep_color );  
	gl_FragColor= vec4( final_color, a2 );

}
