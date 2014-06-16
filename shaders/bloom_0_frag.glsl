#version 330

uniform vec3 inv_screen_size= vec3( 512.0, 384.0, 0.0 );
uniform sampler2D scene_buffer;
uniform float pass_edge;

in vec2 f_tex_coord;
out vec3 color;

//const float tex_coef[7]= float[7]( 0.0625, 0.125, 0.1875, 0.25, 0.1875, 0.125, 0.0625 );
const float tex_coef[9]= float[9]( 0.0546875, 0.0546875, 0.109375, 0.21875, 0.125, 0.21875, 0.109375, 0.0546875, 0.0546875 );
void main()
{
	vec3 c, c2;
	vec2 tc= f_tex_coord;
	float dx= inv_screen_size.x;
	tc.x -= 4.0 * dx;
	int i;
	for( i=0; i< 9; i++, tc.x+= dx )
	{
		c2= texture( scene_buffer, tc ).xyz;
		c+= max( vec3( 0.0, 0.0, 0.0 ), c2 - vec3( 15.0, 15.0, 15.0 ) ) * tex_coef[i];
	}
	color= c;
}

