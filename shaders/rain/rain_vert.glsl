#version 330

in vec2 coord;

uniform float time;
uniform float rain_velocity= 2.0;
uniform float particle_zone_size= 48.0;
uniform vec3 particle_zone_coord= vec3( 0.0, 0.0, 0.0 );

uniform vec3 particle_size;
uniform samplerBuffer particle_coords;

uniform mat4 proj_mat;

uniform sampler2DShadow shadow_map;
uniform mat4 shadow_mat;

out vec2 f_tex_coord;
out vec3 f_color;
out float f_light;
void main()
{
	vec3 particle_coord= texelFetch( particle_coords, gl_InstanceID  ).xyz;
	particle_coord.z-= rain_velocity * time;
	particle_coord= mod( particle_coord - particle_zone_coord, particle_zone_size ) + 	particle_zone_coord;
	

	f_tex_coord= coord * 0.5 + vec2( 0.5, 0.5 );	

	vec4 res_pos= proj_mat * vec4( particle_coord, 1.0 );
	res_pos.xy+= coord.xy * particle_size.xy;
	gl_Position= res_pos;

	vec3 shadow_pos= ( shadow_mat * vec4( particle_coord, 1.0 ) ).xyz;
	shadow_pos.xy= 1.125 * shadow_pos.xy / ( vec2( 0.125, 0.125 ) + abs( shadow_pos.xy ) );
    	shadow_pos*= 0.5;
    	shadow_pos+= vec3( 0.5, 0.5, 0.5 );
	f_light= 10.0 * texture( shadow_map, shadow_pos ) + 5.0;

	f_color= vec3( 0.5, 0.5, 1.0 );
}