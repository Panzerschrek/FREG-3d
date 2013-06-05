#version 330

const vec3 normals[6]= vec3[6](
vec3( 1.0, 0.0, 0.0 ), vec3( -1.0, 0.0, 0.0 ),
vec3( 0.0, 1.0, 0.0 ), vec3( 0.0, -1.0, 0.0 ),
vec3( 0.0, 0.0, 1.0 ), vec3( 0.0, 0.0, -1.0 ));

const float block_side_light_k[6]= float[6](
0.87f, 0.87f, 0.93f, 0.93f, 1.0f, 0.8f );

const float tex_k= 1.0f/16.0f;
uniform float max_view2;
//uniform float sun_k= 0.4;
//uniform float light_k= 0.4;

uniform float sky_ambient_light= 0.5f;
uniform float fire_ambient_light= 0.2f;
uniform float time;

in vec3 coord;
in vec3 tex_coord;
in int normal;
in vec2 light;

uniform mat4 proj_mat;
uniform vec3 cam_pos;
uniform mat4 shadow_mat;

out vec2 f_tex_coord;
out vec3 f_normal;
out float f_light;
out float fog_k;
out vec3 f_position;
out vec3 f_world_position;
out float f_depth;


float Noise()
{
	return sin( ( coord.x + time ) * 0.5 * 3.1415926535 ) *
	sin( ( coord.y + time )* 0.5 * 3.1415926535 ) * 0.1 - 0.15;
}

void main( void )
{

    f_tex_coord= coord.xy * 0.125;


    vec3 len2= cam_pos - coord;
    fog_k= dot( len2, len2 );
    fog_k= min( 0.25f * fog_k *  max_view2, 1.0f );

    f_light= fire_ambient_light * light.y + sky_ambient_light * light.x;
    f_light*= block_side_light_k[ normal ];
    f_normal= normals[ normal ];

    vec3 m_coord= coord;
    m_coord.z+= Noise();

    f_position= ( shadow_mat * vec4( coord, 1.0 ) ).xyz;
    //f_position.xy= 1.25 * f_position.xy / ( vec2( 0.25, 0.25 ) + abs( f_position.xy ) );
    f_position.xy= 1.125f * f_position.xy / ( vec2( 0.125f, 0.125f ) + abs( f_position.xy ) );
    f_position*= 0.5;
    f_position+= vec3( 0.5, 0.5, 0.5 );

    f_world_position= m_coord;
    vec4 final_coord;
    gl_Position= final_coord= proj_mat * vec4( m_coord, 1.0f );
    f_depth= final_coord.w;
}
