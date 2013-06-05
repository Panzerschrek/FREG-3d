#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int SKY_SIZE= 1024;
int STAR_COUNT= 32768;
int STAR_COLOR_COMPONENT_DELTA= 32;

/*
x+ 0
x- 1
y+ 2
y- 3
z+ 4
z- 5
*/
int rSaveTextureTGA( const char* file_name, unsigned char* data, unsigned int sizeX, unsigned int sizeY )
{
    unsigned char  TGAheader[12]= {0,0,2,0,0,0,0,0,0,0,0,0};
    FILE* file_tga=fopen( file_name,"wb" );
    if (file_tga==NULL)
        return 1;
    unsigned char o_0=0;
    fwrite(TGAheader,1,12,file_tga);
    fwrite(&sizeX,1,2,file_tga);
    fwrite(&sizeY,1,2,file_tga);

    unsigned char bit=24;
    fwrite(&bit,1,1,file_tga);
    fwrite(&o_0,1,1,file_tga);
    int imagesize=sizeX*sizeY * 3;

    //int step=bit/8;

    fwrite(data,1,imagesize,file_tga);
    fclose(file_tga);


    return 0;
}


unsigned char* sky_cubemap[6];

inline float sign(float x )
{
    if( x > 0.0f )
        return 1.0f;
    else if( x < 0.0f )
        return -1.0f;
    else return 0.0f;
}

void WriteToCubeMap( float x, float y, float z, unsigned char* color )
{
    unsigned int side;

    float ax= fabs(x), ay= fabs(y), az= fabs(z);
    float tx, ty;
    unsigned int ix, iy;

    if( ax > ay && ax > az )
    {
        side= ( x >=0.0f ) ? 0 : 1;
        tx= -y * sign(x)/ax;
        ty= z/ax;

    }
    else if( ay >= ax && ay > az )
    {
        side= ( y >=0.0f ) ? 2 : 3;
        tx= x * sign(y)/ay;
        ty= z/ay;
    }
    else//if( az >= ax && az >= ay )
    {
        side= ( z >=0.0f ) ? 4 : 5;
        tx= x/az;
        ty= -y * sign(z)/az;
    }

    ix=(unsigned int)( ( tx * 0.5f + 0.5f ) * float( SKY_SIZE ) );
    iy=(unsigned int)( ( ty * 0.5f + 0.5f ) * float( SKY_SIZE ) );

    sky_cubemap[ side ][     3 * ( ix + iy * SKY_SIZE ) ]= color[0];
    sky_cubemap[ side ][ 1 + 3 * ( ix + iy * SKY_SIZE ) ]= color[1];
    sky_cubemap[ side ][ 2 + 3 * ( ix + iy * SKY_SIZE ) ]= color[2];

}

void GenSky()
{
    int i, j;
   // float u, v;
    float x, y, z;
    float r;
    unsigned char star_color[]= { 255,255,255 };
    unsigned char star_intencity;
    int hr_color;

    for( i= 0; i< STAR_COUNT; i++ )
    {
        //u= float( rand() ) / float( RAND_MAX )  * 2.0f * M_PI;
        //v= ( float( rand() ) / float( RAND_MAX )  -0.5f ) * M_PI;
       /* x= cos( v ) * cos( u );
        y= cos( v ) * sin( u );
        z= sin( v );*/
        x= float( rand() ) / float( RAND_MAX ) * 2.0f - 1.0f;
        y= float( rand() ) / float( RAND_MAX ) * 2.0f - 1.0f;
        z= float( rand() ) / float( RAND_MAX ) * 2.0f - 1.0f;
        r= sqrt( x * x + y * y + z * z );
        if( r > 1.0f )
        	continue;//discard vector inside unit sphere
        r= 1.0f/r;
        x*= r;
        y*= r;
        z*= r;

        star_intencity= rand()&255;
        for( j= 0; j< 3; j++ )
        {
            hr_color= star_intencity + ( rand() % STAR_COLOR_COMPONENT_DELTA ) - STAR_COLOR_COMPONENT_DELTA/2;
            if( hr_color < 0 ) hr_color= 0;
            else if( hr_color > 255 ) hr_color= 255;
            star_color[j]= (unsigned char) hr_color;
        }


        WriteToCubeMap( x, y, z, star_color );
    }
}


int main( int argc, char* argv[])
{
    int i, j;


	//char command_buffer[128];
	int param;
    for( i= 1; i< argc; i++ )
    {
		if( !strcmp( argv[i], "--stars" ) )
		{
			i++;
			sscanf( argv[i], "%d", &param );
			if( param > 0 && param < (1<<30) )
				STAR_COUNT= param;
		}
		else if( !strcmp( argv[i], "--color_delta" ) )
		{
			i++;
			sscanf( argv[i], "%d", &param );
			if( param > 0 && param < 128 )
				STAR_COLOR_COMPONENT_DELTA= param;
		}
		else if( !strcmp( argv[i], "--cubemap_size" ) )
		{
			i++;
			sscanf( argv[i], "%d", &param );
			if( param > 0 && param < 4096 )
				SKY_SIZE= param;
		}

    }


    for( i= 0; i< 6; i++ )
    {
        sky_cubemap[i]= new unsigned char[ SKY_SIZE * SKY_SIZE * 3 ];
        for( j= 0; j< SKY_SIZE * SKY_SIZE * 3; j++ )
            sky_cubemap[i][j]= 0;
    }

    GenSky();

    const char* file_name_mask= "sky%d.tga";
    char file_name[32];

    for( i= 0; i< 6; i++ )
    {
        sprintf( file_name, file_name_mask, i );
        rSaveTextureTGA( file_name, sky_cubemap[i], SKY_SIZE, SKY_SIZE );
    }
}
