#version 330   


in vec3 coord;
uniform mat4 proj_mat;
void main( void )
{
	vec4 pos= proj_mat * vec4( coord, 1.0f );
	//pos.xy=  1.25 * pos.xy / ( abs( pos.xy ) + vec2( 0.25f, 0.25f ) );
	pos.xy= 1.125 * pos.xy / ( abs( pos.xy ) + vec2( 0.125, 0.125 ) );
	pos.z+= 0.005;
	gl_Position= pos;
}   
   