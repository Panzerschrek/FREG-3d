#version 120

varying vec2 f_tex_coord;

uniform vec3 inv_screen_size;
uniform sampler2D scene_buffer;
uniform float time;

void main()
{
	vec3 c;
    vec2 tc2= f_tex_coord, tc;

    tc2.x+= 2.0 * inv_screen_size.x * sin( f_tex_coord.y * 20.0 + time * 2.0 );
    tc2.y+=  2.0 * inv_screen_size.y * sin( f_tex_coord.x * 20.0 + time * 2.0 );

    tc= tc2;

    tc.x-= 2.0f * inv_screen_size.x;

    c+= texture2D( scene_buffer, tc ).xyz * 0.1;

    tc.x+=inv_screen_size.x;
    c+= texture2D( scene_buffer, tc ).xyz * 0.15;

    tc.x+=inv_screen_size.x;
    c+= texture2D( scene_buffer, tc ).xyz * 0.25;

    tc.x+=inv_screen_size.x;
    c+= texture2D( scene_buffer, tc ).xyz * 0.15;

    tc.x+=inv_screen_size.x;
    c+= texture2D( scene_buffer, tc ).xyz * 0.1;

    tc= tc2;
    tc.x+= inv_screen_size.y;
    c+= texture2D( scene_buffer, tc ).xyz * 0.125;

    tc.x-= 2.0 * inv_screen_size.y;
    c+= texture2D( scene_buffer, tc ).xyz * 0.125;


    c*= vec3( 0.6f, 0.6f, 0.8f );

    gl_FragColor= vec4( c, 1.0 );
}
