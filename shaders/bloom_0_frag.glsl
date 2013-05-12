#version 330

uniform vec3 inv_screen_size= vec3( 512.0f, 384.0f, 0.0f );
uniform sampler2D scene_buffer;
uniform float pass_edge;

in vec2 f_tex_coord;
out vec3 color;

const float tex_coef[7]= float[7]( 0.0625f, 0.125f, 0.1875f, 0.25f, 0.1875f, 0.125f, 0.0625f );
void main()
{
	vec3 c, c2;
	vec2 tc= f_tex_coord;
	float dx= inv_screen_size.x;
	tc.x -= 3.0 * dx;
	int i;
	for( i=0; i< 7; i++, tc.x+= dx )
	{
		c2= texture( scene_buffer, tc ).xyz;
		c+= max( vec3( 0.0f, 0.0f, 0.0f ), c2 - vec3( 15.0f, 15.0f, 15.0f ) ) * tex_coef[i];
	}
	color= c;
}

