#version 330
uniform sampler2D tex;

in vec3 f_color;
in vec2 f_tex_coord;


out vec4 FragColor;
void main()
{
	float c= texelFetch( tex, ivec2(f_tex_coord + vec2( 0.001, 0.001 ) ) , 0 ).x;
	FragColor= vec4( c * f_color, c * 0.5 + 0.5 );
}