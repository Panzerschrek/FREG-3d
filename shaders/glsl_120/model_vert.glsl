#version 120
uniform float sun_k= 0.08;
uniform float light_k= 0.04;

uniform vec3 pos;
uniform vec3 light;
uniform float inv_tex_z_size= 1.0/32.0;

attribute vec3 coord;
attribute vec2 tex_coord;
attribute vec3 normal;

uniform mat4 proj_mat;

varying vec2 f_tex_coord;
varying vec2 f_tex_coord_shift;
varying float f_light;

void main( void )
{
    f_tex_coord= ( tex_coord.xy / 2048.0 ) * ( 1.0/16.0 );
    f_tex_coord_shift.x= mod( light.z, 16.0 ) * ( 1.0/16.0 );
    f_tex_coord_shift.y= float( int( light.z * ( 1.0/16.0 ) ) ) * ( 1.0/16.0 ); 

    vec3 p= coord / 1024.0 + pos;
    f_light= sun_k * light.x + light_k * light.y;
    f_light*= ( normal.z * 2.0 + 6.0 ) * 0.125;
    gl_Position= proj_mat * vec4( p, 1.0f );
}
