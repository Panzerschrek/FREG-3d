#version 330
const float tex_k= 1.0/16.0;
uniform float max_view2;
uniform float sky_ambient_light= 0.5f;
uniform float fire_ambient_light= 0.2f;

uniform isamplerBuffer model_buffer;
uniform int texture_buffer_shift= 0;

in vec3 coord;
in vec2 tex_coord;
in vec3 normal;

uniform mat4 proj_mat;
uniform vec3 cam_pos;
uniform mat4 shadow_mat;

out vec3 f_tex_coord;
out vec3 f_normal;
out float f_light;
out float fog_k;
out vec3 f_position;
out float vert_color;

void main( void )
{
    f_tex_coord.xy= tex_coord.xy / 2048.0f;
    fog_k= 1.0;
    f_normal= normal;

    vec3 p= coord / 1024.0f;
    ivec4 tex_data= texelFetch( model_buffer, gl_InstanceID + texture_buffer_shift );
    p+= vec3( tex_data.xyz );
    f_tex_coord.z = float( tex_data.w & 255 );
    f_light= 16.0 * sky_ambient_light * float( (tex_data.w >> 8 ) & 15 ) + fire_ambient_light * float( tex_data.w >>12 );

    f_position= ( shadow_mat * vec4( p, 1.0 ) ).xyz;
    //f_position.xy= 1.25 * f_position.xy / ( vec2( 0.25, 0.25 ) + abs( f_position.xy ) );
    f_position.xy= 1.125f * f_position.xy / ( vec2( 0.125f, 0.125f ) + abs( f_position.xy ) );
    f_position*= 0.5;
    f_position+= vec3( 0.5, 0.5, 0.5 );

    gl_Position= proj_mat * vec4( p, 1.0f );
}
