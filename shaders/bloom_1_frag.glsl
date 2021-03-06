#version 330

uniform vec3 inv_screen_size= vec3( 512.0f, 384.0f, 0.0f );
uniform sampler2D bloom_0_buffer;

in vec2 f_tex_coord;
out vec3 color;

//const float tex_coef[7]= float[7]( 0.15f, 0.2f, 0.3f, 0.4f, 0.3f, 0.2f, 0.15f );
//const float tex_coef[7]= float[7]( 0.0625, 0.125, 0.1875, 0.25, 0.1875, 0.125, 0.0625 );
const float tex_coef[9]= float[9]( 0.0546875, 0.0546875, 0.109375, 0.21875, 0.125, 0.21875, 0.109375, 0.0546875, 0.0546875 );
void main()
{
	vec3 c, c2;
	vec2 tc= f_tex_coord;
	float dy=  inv_screen_size.y;
	tc.y -= 4.0 * dy;
	int i;
	for( i=0; i< 9; i++, tc.y+= dy )
	{
		c2= texture( bloom_0_buffer, tc ).xyz;
		c+= c2 * tex_coef[i];
	}
	color= c;
}

