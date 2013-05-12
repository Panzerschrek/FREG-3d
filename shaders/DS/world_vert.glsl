#version 330

const vec3 normals[6]= vec3[6](
vec3( 1.0, 0.0, 0.0 ), vec3( -1.0, 0.0, 0.0 ),
vec3( 0.0, 1.0, 0.0 ), vec3( 0.0, -1.0, 0.0 ),
vec3( 0.0, 0.0, 1.0 ), vec3( 0.0, 0.0, -1.0 ));


const float block_side_light_k[6]= float[6](
0.87f, 0.87f, 0.93f, 0.93f, 1.0f, 0.8f );

in vec3 coord;
in vec3 tex_coord;
in int normal;
in vec2 light;

uniform mat4 proj_mat;
uniform mat3 normal_mat;

out vec3 f_tex_coord;
out vec3 f_normal;
out vec2 f_light;


void main( void )
{
    f_tex_coord.xy= tex_coord.xy * 0.125;
    f_tex_coord.z= tex_coord.z;
    f_normal= normals[ normal ];
    f_light= light * block_side_light_k[ normal ];
    gl_Position= proj_mat * vec4( coord, 1.0f );
}   