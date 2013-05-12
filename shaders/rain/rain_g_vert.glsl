#version 330

in vec4 coord;

uniform float time;
uniform float rain_velocity= 2.0;
uniform float particle_zone_size= 48.0;
uniform vec3 particle_zone_coord= vec3( 0.0, 0.0, 0.0 );


uniform mat4 proj_mat;

uniform sampler2DShadow shadow_map;
uniform mat4 shadow_mat;
out float g_light;

void main()
{
	vec3 particle_coord= coord.xyz;
	particle_coord.z-= rain_velocity * time;
	particle_coord= mod( particle_coord - particle_zone_coord, particle_zone_size ) + 		particle_zone_coord;


	vec3 shadow_pos= ( shadow_mat * vec4( particle_coord, 1.0 ) ).xyz;
	shadow_pos.xy= 1.125 * shadow_pos.xy / ( vec2( 0.125, 0.125 ) + abs( shadow_pos.xy ) );
    	shadow_pos*= 0.5;
    	shadow_pos+= vec3( 0.5, 0.5, 0.5 );
	g_light= 10.0 * texture( shadow_map, shadow_pos ) + 5.0;	

	vec4 res_pos= proj_mat * vec4( particle_coord, 1.0 );
	//res_pos.w=abs(res_pos.w ); //test
	gl_Position= res_pos;
}