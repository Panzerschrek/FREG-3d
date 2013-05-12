#version 120

uniform sampler2D tex;

varying float particle_aspect;
void main()
{
	vec4 c= texture2D( tex, gl_PointCoord );
	if( c.a < 0.01 )
		discard;
	gl_FragColor= c;
}