#version 330

in vec2 coord;
out vec2 f_tex_coord;
out float angle_cos;

uniform mat4 proj_mat;
uniform vec3 sun_pos;
uniform float aspect;
uniform float sun_size= 0.0625;
void main( void )
{
	f_tex_coord= coord * 0.5 + vec2( 0.5, 0.5 );
	vec4 pos= proj_mat * vec4( sun_pos, 1.0 );
	pos/= pos.w;
	pos.y+= sun_size * coord.y;
	pos.x += aspect * sun_size * coord.x;
	gl_Position= pos;

	angle_cos= normalize( sun_pos ).z;
}
   