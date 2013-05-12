#version 120

attribute vec2 coord;
varying vec2 f_tex_coord;

uniform mat4 proj_mat;
uniform vec3 sun_pos;
uniform float aspect;
uniform float sun_size= 0.0625;
void main( void )
{
	f_tex_coord= coord * 0.5 + vec2( 0.5, 0.5 );
	vec4 pos= proj_mat * vec4( sun_pos, 1.0 );
	pos/= pos.w;
	//pos.w= 1.0;
	pos.y+= 2.0 * sun_size * coord.y;
	pos.x += 2.0 * aspect * sun_size * coord.x;
	gl_Position= pos;
}
   