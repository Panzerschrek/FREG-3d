/*
*This file is part of FREG.
*
*FREG is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*FREG is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with FREG. If not, see <http://www.gnu.org/licenses/>.
*/

/* This file provides simple (also mad) lighting for freg.
 * It has light inertia, meaning that block, enlightened by outer
 * source, will remain enlightened when light source is removed.
 * Light is divided to sunlight and other light. Sunlight is
 * changing over the day.
 * LightMap is uchar:
 * & 0xF0 bits are for non-sun light,
 * & 0x0F bits for sun.
 */
#include "world.h"
#include "Shred.h"

#ifndef max
#define max( x, y ) ((x) > (y) ? (x) : (y))
#define min( x, y ) ((x) > (y) ? (y) : (x))
#endif


const uchar MOON_LIGHT_FACTOR=1;
const uchar  SUN_LIGHT_FACTOR=8;
const uchar FIRE_LIGHT_FACTOR=4;

//private
uchar World::LightRadius(
    const ushort x,
    const ushort y,
    const ushort z) const
{
    return GetShred(x, y)->
           LightRadius(x%shred_width, y%shred_width, z);
}

//private. use Enlightened instead, which is smart wrapper of this.
uchar World::LightMap(
    const ushort x,
    const ushort y,
    const ushort z) const
{
    return GetShred(x, y)->
           LightMap(x%shred_width, y%shred_width, z);
}

//private
bool World::SetLightMap(
    const uchar level,
    const ushort x,
    const ushort y,
    const ushort z)
{
    return GetShred(x, y)->
           SetLightMap(level, x%shred_width, y%shred_width, z);
}


void World::Shine(
    const ushort i,
    const ushort j,
    const ushort k,
    const uchar level,
    const bool init) //see default in class
{
    if( init )
        AddFireLightMC( i, j, k, level );
}

void World::SunShine(
    const ushort i,
    const ushort j)
{
	SunShineMC( i, j );
}

//private. called when onet block is moved, built, or destroyed.
void World::ReEnlighten(
		const ushort i,
		const ushort j,
		const ushort k)
{

	unsigned char l= LightRadius(i, j, k);
	if( l > 0 )
		Shine( l );
	SunShine(i, j);
	emit UpdatedAround(i, j, k, l + 1);
}

//private. called when world is created.
void World::ReEnlightenTime() {
	for (ushort i=0; i<NumShreds()*NumShreds(); ++i)
		shreds[i]->SetAllLightMap();
	ReEnlightenAll();
}


//private. called from ReEnlightenTime and from World::ReloadShreds
void World::ReEnlightenAll() {
	disconnect(this, SIGNAL(Updated(
		const ushort,
		const ushort,
		const ushort)), 0, 0);
	disconnect(this, SIGNAL(UpdatedAround(
		const ushort,
		const ushort,
		const ushort,
		const ushort)), 0, 0);

	for (ushort i=0; i<NumShreds()*NumShreds(); ++i)
		shreds[i]->ShineAll();

	emit UpdatedAll();
	emit ReConnect();
}

void World::ReEnlightenMove(const int dir) {
	disconnect(this, SIGNAL(Updated(
		const ushort,
		const ushort,
		const ushort)), 0, 0);
	disconnect(this, SIGNAL(UpdatedAround(
		const ushort,
		const ushort,
		const ushort,
		const ushort)), 0, 0);

	switch ( dir ) {
		case NORTH:
			for (ushort i=0; i<NumShreds(); ++i) {
				shreds[i]->ShineAll();
				shreds[i+NumShreds()]->ShineAll();
			}
		break;
		case SOUTH:
			for (ushort i=0; i<NumShreds(); ++i) {
				shreds[i+NumShreds()*(NumShreds()-1)]->ShineAll();
				shreds[i+NumShreds()*(NumShreds()-2)]->ShineAll();
			}
		break;
		case EAST:
			for (ushort i=0; i<NumShreds(); ++i) {
				shreds[NumShreds()*i+NumShreds()-1]->ShineAll();
				shreds[NumShreds()*i+NumShreds()-2]->ShineAll();
			}
		break;
		case WEST:
			for (ushort i=0; i<NumShreds(); ++i) {
				shreds[NumShreds()*i]->ShineAll();
				shreds[NumShreds()*i+1]->ShineAll();
			}
		break;
		default:
			fprintf(stderr,
				"World::ReEnlightenMove: unlisted direction: %d\n",
				dir);
	}
	emit ReConnect();
}

void World::RemoveFireLightMC( short x, short y, short z )
{
    //printf( "clear light\n" );
    short x0, x1, y0, y1, z0, z1;

    short d= max_light_radius;//GetFireLightSafe( x, y, z ) - 1;
    if( d <= 0 )
        return;

    x0= max( 0, x - d );
    x1= min( x + d, numShreds * shred_width - 1 );
    y0= max( 0, y - d );
    y1= min( y + d, numShreds * shred_width - 1 );
    z0= max( 0, z - d );
    z1= min( z + d, height - 2 );//небо не переосвещается

    //зануление света в кубе вокруг источника
    short i, j, k;
    for( i= x0; i<= x1; i++ )
        for( j= y0; j<= y1; j++ )
            for( k= z0; k<= z1; k++ )
                SetFireLightSafe( i, j, k, 0 );

    //переосвещение всеми источениками в этом кубе
    for( i= x0; i<= x1; i++ )
        for( j= y0; j<= y1; j++ )
            for( k= z0; k<= z1; k++ )
            {
                unsigned char l= ushort( ceil( LightRadius( i, j, k ) ) );
                if( l > 1 )
                    AddFireLightMC( i, j, k, l );
            }



    //обход всех сторон этого куба и переосвещение его объёма от них
    if( x0 > 0 )
    {
        for( j= y0 - 1; j<= ( y1 + 1 ); j++ )
            for( k= z0 - 1; z<= ( z1 + 1 ); z++ )
                AddFireLightMC(  x0 - 1 , j, k, GetFireLightSafe( x0 - 1, j, k ) );
    }
    if( x1 < mapSize * shred_width - 1 )
    {
        for( j= y0 - 1; j<= ( y1 + 1 ); j++ )
            for( k= z0 - 1; z<= ( z1 + 1 ); z++ )
                AddFireLightMC(  x1 + 1 , j, k, GetFireLightSafe( x1 + 1, j, k ) );
    }

    if( y0 > 0 )
    {
        for( i= x0 - 1; i<= ( x1 + 1 ); i++ )
            for( k= z0 - 1; z<= ( z1 + 1 ); z++ )
                AddFireLightMC(  i , y0 - 1, k, GetFireLightSafe( i,  y0 - 1, k ) );
    }
    if( y1 < mapSize * shred_width - 1 )
    {
        for( i= x0 - 1; i<= ( x1 + 1 ); i++ )
            for( k= z0 - 1; z<= ( z1 + 1 ); z++ )
                AddFireLightMC(  i , y1 + 1, k, GetFireLightSafe( i,  y1 + 1, k ) );
    }

    if( z0 > 0 )
    {
        for( i= x0 - 1; i<= ( x1 + 1 ); i++ )
            for( j= y0 - 1; j<= ( y1 + 1 ); j++ )
                AddFireLightMC( i, j, z0 - 1, GetFireLightSafe( i, j, z0 - 1 ) );
    }
    if( z0 < height - 2 )
    {
        for( i= x0 - 1; i<= ( x1 + 1 ); i++ )
            for( j= y0 - 1; j<= ( y1 + 1 ); j++ )
                AddFireLightMC( i, j, z1 + 1, GetFireLightSafe( i, j, z1 + 1 ) );
    }
}

void World::RemoveSunLightMC( short x, short y )
{
    for( short z= height - 2; z>=0; z-- )
        SetSunLightSafe( x, y, z, 0 );
}

void World::SunShineMC( short x, short y  )
{
    short z= height - 2;
    unsigned char l = max_light_radius;
    unsigned char t;
    do
    {
        t= TransparentSafe( x, y, z );
        if( t == 0 )
            break;
        else if( t == 1 || t == 6 )
        {
            l--;
            if( l== 0 )
                break;
        }

        SetSunLightSafe( x, y, z, l );
        z--;
    }
    while( 1 );
}


void World::SunHorizontalShineMC( short x, short y )
{
}

void World::AddFireLightMC( short x, short y, short z, unsigned char l )
{
    short stack[4096][3];
    short n= 0;
    char l2, l1= l;

    SetFireLightSafe( x, y, z, l1 );
    do
    {
        l1= GetFireLightSafe( x, y, z );
        if( l1 == 0 )
            goto next_step;

        l2= GetFireLightSafe( x + 1, y, z );
        if( l1 - l2 > 1  && TransparentSafe( x + 1, y, z ) )
        {
            SetFireLightSafe( x + 1, y, z, l1 - 1 );
            stack[n][0]= x + 1;
            stack[n][1]= y;
            stack[n][2]= z;
            n++;
        }

        l2= GetFireLightSafe( x - 1, y, z );
        if( l1 - l2 > 1 && TransparentSafe( x - 1, y, z ) )
        {
            SetFireLightSafe( x - 1, y, z, l1 - 1 );
            stack[n][0]= x - 1;
            stack[n][1]= y;
            stack[n][2]= z;
            n++;
        }

        l2= GetFireLightSafe( x, y + 1, z );
        if( l1 - l2 > 1  && TransparentSafe( x, y + 1, z ) )
        {
            SetFireLightSafe( x, y + 1, z, l1 - 1 );
            stack[n][0]= x;
            stack[n][1]= y + 1;
            stack[n][2]= z;
            n++;
        }
        l2= GetFireLightSafe( x, y - 1, z );
        if( l1 - l2 > 1 && TransparentSafe( x, y - 1, z ) )
        {
            SetFireLightSafe( x, y - 1, z, l1 - 1 );
            stack[n][0]= x;
            stack[n][1]= y - 1;
            stack[n][2]= z;
            n++;
        }
        l2= GetFireLightSafe( x, y, z + 1 );
        if( l1 - l2 > 1 && TransparentSafe( x, y, z + 1 ) )
        {
            SetFireLightSafe( x, y, z + 1, l1 - 1 );
            stack[n][0]= x;
            stack[n][1]= y;
            stack[n][2]= z + 1;
            n++;
        }
        l2= GetFireLightSafe( x, y, z - 1 );
        if( l1 - l2 > 1 && TransparentSafe( x, y, z - 1 ) )
        {
            SetFireLightSafe( x, y, z - 1, l1 - 1 );
            stack[n][0]= x;
            stack[n][1]= y;
            stack[n][2]= z - 1;
            n++;
        }

next_step:
        n--;//выборка из стека
        x= stack[n][0];
        y= stack[n][1];
        z= stack[n][2];
    }
    while( n >= 0 );
}

unsigned char World::GetFireLightSafe( short x, short y, short z )
{

    if( x < 0 )x= 0;
    else if( x >= numShreds*shred_width ) x= numShreds*shred_width - 1;
    if( y < 0 )y= 0;
    else if( y >= numShreds*shred_width ) y= numShreds*shred_width - 1;
    if( z< 0 )z=0;
    else if( z > height -  2) z= height - 2;

    return GetShred( x, y )->LightMap( x&15, y&15, z ) >>4;
}
unsigned char World::GetSunLightSafe( short x, short y, short z )
{

    if( x < 0 )x= 0;
    else if( x >= numShreds*shred_width ) x= numShreds*shred_width - 1;
    if( y < 0 )y= 0;
    else if( y >= numShreds*shred_width ) y= numShreds*shred_width - 1;
    if( z< 0 )z=0;
    else if( z > height -  2) z= height - 2;

    return GetShred( x, y )->LightMap( x&15, y&15, z ) & 15;
}


void Shred::SetSunLight( ushort x, ushort y, ushort z, uchar l )
{
    register unsigned char tmp;
    tmp= LightMap( x, y, z );
    tmp&= 0xF0;
    tmp|= l;
    SetLightMap( tmp, x, y, z );
}
void Shred::SetFireLight( ushort x, ushort y, ushort z, uchar l )
{
    register unsigned char tmp;
    tmp= LightMap( x, y, z );
    tmp&= 0x0F;
    tmp|= l<<4;
    SetLightMap( tmp, x, y, z );
}

void World::SetSunLightSafe( short x, short y, short z, unsigned char l )
{
    if( x < 0 )x= 0;
    else if( x >= numShreds*shred_width ) x= numShreds*shred_width - 1;
    if( y < 0 )y= 0;
    else if( y >= numShreds*shred_width ) y= numShreds*shred_width - 1;
    if( z< 0 )z=0;
    else if( z > height -  2) z= height - 2;
    GetShred( x, y )->SetSunLight( x&15, y&15, z, l );
}

void World::SetFireLightSafe( short x, short y, short z, unsigned char l )
{
    if( x < 0 )x= 0;
    else if( x >= numShreds*shred_width ) x= numShreds*shred_width - 1;
    if( y < 0 )y= 0;
    else if( y >= numShreds*shred_width ) y= numShreds*shred_width - 1;
    if( z< 0 )z=0;
    else if( z > height -  2) z= height - 2;
    GetShred( x, y )->SetFireLight( x&15, y&15, z, l );
}

unsigned char World::TransparentSafe( short x, short y, short z )
{
    if( x < 0 )x= 0;
    else if( x >= numShreds*shred_width ) x= numShreds*shred_width - 1;
    if( y < 0 )y= 0;
    else if( y >= numShreds*shred_width ) y= numShreds*shred_width - 1;
    if( z< 0 )z=0;
    else if( z > height -  2) z= height - 2;
    return Transparent( x, y, z );
}

