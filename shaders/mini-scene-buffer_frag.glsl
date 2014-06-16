#version 330

out vec4 color;
uniform sampler2D tex;
uniform float adapted_brightness= 1.0;

in vec2 f_tex_coord;

vec3 ToneMapping( vec3 c )
{
    return vec3( 1.0, 1.0, 1.0 ) - exp( - adapted_brightness * c );
}

void main()
{
	color= vec4( ToneMapping ( texture( tex, f_tex_coord ).xxx * 0.3333 ), 1.0 );
}