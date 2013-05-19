#version 120

const float block_side_light_k[6]= float[6](
0.87f, 0.87f, 0.93f, 0.93f, 1.0f, 0.8f );

varying vec2 f_tex_coord;
varying float f_light;
varying vec3 f_world_position;

uniform mat4 proj_mat;
uniform float inv_tex_z_size= 1.0/ 32.0;
uniform float time;

attribute vec3 coord;
attribute vec3 tex_coord;
attribute float normal;
attribute vec2 light;


uniform float sky_ambient_light= 0.08;
uniform float fire_ambient_light= 0.04;


float Noise()
{
	return sin( ( coord.x + time ) * 0.5 * 3.1415926535 ) * 
	sin( ( coord.y + time )* 0.5 * 3.1415926535 ) * 0.1 - 0.15;
}

void main( void )
{
   	int i_normal= int( normal + 0.00001 );
	f_tex_coord= coord.xy * 0.125;
	f_light= max( sky_ambient_light * light.x + fire_ambient_light * light.y, 0.05 );
	f_light*= block_side_light_k[ i_normal ];

	vec3 m_coord= coord;
	m_coord.z+= Noise();
	f_world_position= m_coord;
	gl_Position= proj_mat * vec4( m_coord, 1.0 );
}   