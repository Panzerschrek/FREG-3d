#version 120


const float block_side_light_k[6]= float[6](
0.87, 0.87, 0.93, 0.93, 1.0, 0.8 );

varying vec2 f_tex_coord;
varying vec2 f_tex_coord_shift;
varying float f_light;

uniform mat4 proj_mat;
uniform float inv_tex_z_size= 1.0/ 32.0;

attribute vec3 coord;
attribute vec3 tex_coord;
attribute float normal;
attribute vec2 light;


uniform float sky_ambient_light= 0.08;
uniform float fire_ambient_light= 0.04;


void main()
{	
	int i_normal= int( normal + 0.00001 );
	f_tex_coord= tex_coord.xy * 0.125 * ( 1.0/16.0 );//16x16 textures in atlas
	f_tex_coord_shift.x= mod( tex_coord.z, 16.0 ) * ( 1.0/16.0 );
	f_tex_coord_shift.y= float( int( tex_coord.z * ( 1.0/16.0 ) ) ) * ( 1.0/16.0 ); 
    	
	f_light= max( sky_ambient_light * light.x + fire_ambient_light * light.y, 0.05 );
	f_light*= block_side_light_k[ i_normal ];
	
	gl_Position= proj_mat * vec4( coord, 1.0 );
}
