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

in vec3 coord;
in vec3 tex_coord;
in int normal;
in vec2 light;

uniform mat4 proj_mat;
uniform vec3 cam_pos;
uniform mat4 shadow_mat;

out vec3 f_tex_coord;
out vec3 f_normal;
out float f_light;
out float fog_k;
out vec3 f_position;
out vec3 f_world_position;


void main( void )
{
    f_tex_coord.xy= tex_coord.xy * 0.125;
    f_tex_coord.z= tex_coord.z;
	
    vec3 len2= cam_pos - coord;
    fog_k= dot( len2, len2 );
    fog_k= min( 0.25f * fog_k *  max_view2, 1.0f );

    f_light= fire_ambient_light * light.y + sky_ambient_light * light.x;
    //f_light*= block_side_light_k[ normal ];
    f_normal= normals[ normal ];

    f_position= ( shadow_mat * vec4( coord, 1.0 ) ).xyz;
    //f_position.xy= 1.25 * f_position.xy / ( vec2( 0.25, 0.25 ) + abs( f_position.xy ) );
    f_position.xy= 1.125f * f_position.xy / ( vec2( 0.125f, 0.125f ) + abs( f_position.xy ) );
    f_position*= 0.5;
    f_position+= vec3( 0.5, 0.5, 0.5 );

    f_world_position= coord;
    gl_Position= proj_mat * vec4( coord, 1.0f );

}   