#version 330
const float tex_k= 1.0/16.0;
uniform float max_view2;
uniform float sun_k= 0.5f;
uniform float light_k= 0.5f;

uniform isamplerBuffer model_buffer;
uniform int texture_buffer_shift= 0;

in vec3 coord;
in vec2 tex_coord;
in vec3 normal;

uniform mat4 proj_mat;
uniform mat3 normal_mat;

out vec3 f_tex_coord;
out vec3 f_normal;
out vec2 f_light;


void main( void )
{
    f_tex_coord.xy= tex_coord.xy / 2048.0f;
    f_normal= normal;

    vec3 p= coord / 1024.0f;
    ivec4 tex_data= texelFetch( model_buffer, gl_InstanceID + texture_buffer_shift );
    p+= vec3( tex_data.xyz );
    f_tex_coord.z = float( tex_data.w & 255 );
    f_light= vec2( float( (tex_data.w >> 8 ) & 15 ), float( tex_data.w >>12 ) );

    gl_Position= proj_mat * vec4( p, 1.0f );
}
