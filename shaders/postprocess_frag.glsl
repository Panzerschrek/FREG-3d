//postprocessing Ubershader
#version 330
uniform sampler2D scene_buffer;
uniform sampler2D bloom_buffer;
uniform sampler2D depth_buffer;
uniform float adapted_brightness;
uniform float saturation= 1.0;
uniform float time;
uniform vec3 inv_screen_size= vec3( 1.0f/ 1024.0, 1.0f/768.0, 0.0 );

uniform float inv_max_view_distance= 0.03125;//for underwater fog
uniform vec3 underwater_fog_color=10.0 * vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 );

//const float eps= 0.5f;
const float eps= 0.25;
in vec2 f_tex_coord;
in vec2 f_screen_coord;

out vec4 color;

uniform vec3 depth_convert_k;


float GetFragLinearDepth( vec2 tc )
{
    return depth_convert_k.x / ( texture( depth_buffer , tc ).x + depth_convert_k.y );
}

vec3 ToneMapping( vec3 c )
{
    return vec3( 1.0, 1.0, 1.0 ) - exp( - adapted_brightness * c );
}

vec3 InvToneMapping( vec3 c )
{
    return exp( - adapted_brightness * c );
}

bool EdgeDetect()
{
    float depth[5];
    vec2 tc= f_tex_coord;
    tc.x-= inv_screen_size.x;
    depth[0]= GetFragLinearDepth(tc);
    tc.x+= inv_screen_size.x;
    depth[4]= GetFragLinearDepth(tc);
    tc.x+= inv_screen_size.x;
    depth[1]= GetFragLinearDepth(tc);

    tc= f_tex_coord;
    tc.y-= inv_screen_size.y;
    depth[2]= GetFragLinearDepth(tc);
    tc.y+= 2.0f * inv_screen_size.y;
    depth[3]= GetFragLinearDepth(tc);

    float a= abs( ( depth[0] + depth[1] + depth[2] + depth[3] ) * 0.25 - depth[4] );
   // float a= abs( depth[0] + depth[1] - depth[2] - depth[3] ) * 0.25;
    return a > eps;
}


vec3 ColorVision()
{
    vec3 c;
    c.x= texture( scene_buffer, f_tex_coord ).x;
    c.y= texture( scene_buffer, vec2( f_tex_coord.x + inv_screen_size.x * 4, f_tex_coord.y ) ).y;
    c.z= texture( scene_buffer, vec2( f_tex_coord.x - inv_screen_size.x * 4, f_tex_coord.y ) ).z;
#ifdef BLOOM
    c= max( texture( bloom_buffer, f_tex_coord ).xyz, c );
#endif
    c= ToneMapping(c);
    return c;
}

vec3 GetFragColor()
{
    vec3 c;
    c= texture( scene_buffer, f_tex_coord ).xyz;
#ifdef BLOOM
    c= max( texture( bloom_buffer, f_tex_coord ).xyz, c );
#endif
    c= ToneMapping(c);
    return c;
}

vec3 LostFocus()
{
    vec3 c= texture( scene_buffer, vec2( f_tex_coord.x + inv_screen_size.x * 8, f_tex_coord.y ) ).xyz
            +texture( scene_buffer, vec2( f_tex_coord.x - inv_screen_size.x * 8, f_tex_coord.y ) ).xyz;
    c*= 0.5;
#ifdef BLOOM
    c= max( texture( bloom_buffer, f_tex_coord ).xyz, c );
#endif
    return ToneMapping( c );
}



vec3 GetFragMultiColor()
{
#ifdef EDGE_DETECT

#ifdef BLOOM
    return ToneMapping( texture( bloom_buffer, f_tex_coord ).xyz );
#else
    return vec3( 0.0, 0.0, 0.0 );
#endif//BLOOM

#else
    vec3 c;
    vec2 tc= f_tex_coord;

    tc.x-= inv_screen_size.x;
    c= 0.125 * InvToneMapping( texture( scene_buffer, tc ).xyz );
    tc.x+= inv_screen_size.x;
    c+= 0.5 * InvToneMapping( texture( scene_buffer, tc ).xyz );
    tc.x+= inv_screen_size.x;
    c+= 0.125 * InvToneMapping( texture( scene_buffer, tc ).xyz );

    tc= f_tex_coord;
    tc.y-= inv_screen_size.y;
    c+= 0.125 * InvToneMapping( texture( scene_buffer, tc ).xyz );
    tc.y+= 2.0 * inv_screen_size.y;
    c+= 0.125 * InvToneMapping( texture( scene_buffer, tc ).xyz );


#ifdef BLOOM
    vec3 bloom= InvToneMapping(  texture( bloom_buffer, f_tex_coord ).xyz );
    c= vec3( 1.0, 1.0, 1.0 ) - min( bloom, c );
#else
    c= vec3( 1.0, 1.0, 1.0 ) - c;
#endif

    return c;
#endif
}


vec3 Soap()//"Ìûכüצמ" like on PS3
{
    vec3 c;
    vec2 tc2= f_tex_coord, tc;

   //lense effect
   // tc2= tc2 * 2.0 - vec2( 1.0, 1.0 );
   // tc2= 0.5 * 0.25 * tc2 * ( vec2( 3.0, 3.0 ) + abs( tc2 ) ) + vec2( 0.5, 0.5 );

    //tc2.x+= 2.0 * inv_screen_size.x * sin( f_tex_coord.y * 20.0 + time * 2.0 );
    //tc2.y+=  2.0 * inv_screen_size.y * sin( f_tex_coord.x * 20.0 + time * 2.0 );

    tc= tc2;
#ifdef SOAP
    tc.x-= 2.0f * inv_screen_size.x;

    c= texture( scene_buffer, tc ).xyz * 0.1;

    tc.x+=inv_screen_size.x;
    c+= texture( scene_buffer, tc ).xyz * 0.15;

    tc.x+=inv_screen_size.x;
    c+= texture( scene_buffer, tc ).xyz * 0.25;

    tc.x+=inv_screen_size.x;
    c+= texture( scene_buffer, tc ).xyz * 0.15;

    tc.x+=inv_screen_size.x;
    c+= texture( scene_buffer, tc ).xyz * 0.1;

    tc= tc2;
    tc.x+= inv_screen_size.y;
    c+= texture( scene_buffer, tc ).xyz * 0.125;

    tc.x-= 2.0 * inv_screen_size.y;
    c+= texture( scene_buffer, tc ).xyz * 0.125;
#else
   c= texture( scene_buffer, tc2 ).xyz;
#endif
	float depth=  length( vec3( f_screen_coord, 1.0 ) ) * GetFragLinearDepth( tc2 );
	return ToneMapping( mix( c, underwater_fog_color, clamp( depth * inv_max_view_distance, 0.0, 1.0 ) )  );
    //return ToneMapping( mix( c, underwater_fog_color, clamp( GetFragLinearDepth( tc2 ) * inv_max_view_distance, 0.0, 1.0 ) )  );

    //return mix( ToneMapping(c), vec3( 72.0/255.0, 108.0/255.0, 169.0/255.0 ), clamp( -GetFragLinearDepth( tc2 ) * 0.03125, 0.0, 1.0 ) );
}

void main()
{
    vec3 c;
#ifndef UNDERWATER

#ifdef ANTIALIASING
    if( EdgeDetect() )
        c= GetFragMultiColor();
    else
        c= GetFragColor();
#else
    c= GetFragColor();
#endif// ANTIALIASING

#else// UNDERWATER
    c= Soap();
#endif
    float c_gray= dot( c, vec3( 0.299, 0.5876, 0.114 ) );
    c= mix( vec3( c_gray, c_gray, c_gray ), c, saturation );
    color= vec4( c, 1.0 );

   //c_gray= texture( depth_buffer, f_tex_coord ).x;

   //color= vec4( c_gray, c_gray, c_gray, 1.0 );
}
