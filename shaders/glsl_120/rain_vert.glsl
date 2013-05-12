#version 120

attribute vec4 coord;

uniform float time;
uniform float rain_velocity= 2.0;
uniform float particle_zone_size= 48.0;
uniform vec3 particle_zone_coord= vec3( 0.0, 0.0, 0.0 );

uniform vec3 particle_size;

uniform mat4 proj_mat;

varying float particle_aspect;
void main()
{
	vec3 particle_coord= coord.xyz;
	particle_coord.z-= rain_velocity * time;
	particle_coord= mod( particle_coord - particle_zone_coord, particle_zone_size ) + particle_zone_coord;


	vec4 res_pos= proj_mat * vec4( particle_coord, 1.0 );
	gl_Position= res_pos;
	gl_PointSize=  particle_size.y / res_pos.w;
}