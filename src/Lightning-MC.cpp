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

/**\file Lighting-inertia.cpp
 * \brief This file provides simple (also mad) lighting for freg.
 *
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
#include "Active.h"

const uchar FIRE_LIGHT_FACTOR=4;


//const uchar MAX_LIGHT_RADIUS=9;

const uchar MAX_SUN_LIGHT_LEVEL= 9;
const uchar MAX_FIRE_LIGHT_LEVEL= 15;

short World::ClampX(const short x ) const
{
    short tmp;
    if( x < 0 )
        return 0;
    else if ( x >= ( tmp= NumShreds() * SHRED_WIDTH ) )
        return tmp - 1;
    return x;
}
short World::ClampY(const short y ) const
{
    short tmp;
    if( y < 0 )
        return 0;
    else if ( y >= ( tmp= NumShreds() * SHRED_WIDTH ) )
        return tmp - 1;
    return y;
}
short World::ClampZ(const short z ) const
{
    if( z < 0 )
        return 0;
    else if( z >= HEIGHT )
        return HEIGHT - 1;
    return z;
}


uchar World::SunLightSmooth( ushort x, ushort y, ushort z) const
{
	unsigned char light= 0;
	unsigned short count= 0;
	if( GetBlock( x, y, z )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x, y, z );
	}
	if( GetBlock( x+1, y, z )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x+1, y, z );
	}
	if( GetBlock( x, y+1, z )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x, y+1, z );
	}
	if( GetBlock( x+1, y+1, z )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x+1, y+1, z );
	}

	if( GetBlock( x, y, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x, y, z+1 );
	}
	if( GetBlock( x+1, y, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x+1, y, z+1 );
	}
	if( GetBlock( x, y+1, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x, y+1, z+1 );
	}
	if( GetBlock( x+1, y+1, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= SunLight( x+1, y+1, z+1 );
	}

	return  (unsigned char)( (((unsigned short)light)*(12+count) ) / count );
}

uchar World::FireLightSmooth( ushort x, ushort y, ushort z) const
{
	unsigned char light= 0;
	unsigned short count= 0;
	if( GetBlock( x, y, z )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x, y, z );
	}
	if( GetBlock( x+1, y, z )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x+1, y, z );
	}
	if( GetBlock( x, y+1, z )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x, y+1, z );
	}
	if( GetBlock( x+1, y+1, z )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x+1, y+1, z );
	}

	if( GetBlock( x, y, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x, y, z+1 );
	}
	if( GetBlock( x+1, y, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x+1, y, z+1 );
	}
	if( GetBlock( x, y+1, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x, y+1, z+1 );
	}
	if( GetBlock( x+1, y+1, z+1 )->Transparent() > 0 )
	{
		count++;
		light+= FireLight( x+1, y+1, z+1 );
	}

	return  (unsigned char)( (((unsigned short)light)<<4) / count );
}

//use Enlightened instead, which is smart wrapper of this.
uchar World::LightMap(const ushort x, const ushort y, const ushort z)
const
{
    return GetShred(x, y)->Lightmap(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

bool World::SetSunLightMap(const uchar level,
                           const ushort x, const ushort y, const ushort z)
{
    return GetShred(x, y)->
           SetSunLight(x%SHRED_WIDTH, y%SHRED_WIDTH, z, level);
}

bool World::SetFireLightMap(const uchar level,
                            const ushort x, const ushort y, const ushort z)
{
    return GetShred(x, y)->
           SetFireLight(x%SHRED_WIDTH, y%SHRED_WIDTH, z, level);
}

void World::AddFireLight(const short x, const short y , const short z, const uchar l )
{
    SetFireLightMap( l, x, y, z );
    if( l <= 1 )
        return;

    static short block_stack[ MAX_FIRE_LIGHT_LEVEL * MAX_FIRE_LIGHT_LEVEL * MAX_FIRE_LIGHT_LEVEL * 8 ][3];

    int stack_p= 0;
    short cx, cy, cz, c2x, c2y, c2z;
    char cl;

    block_stack[0][0]= ClampX(x);
    block_stack[0][1]= ClampY(y);
    block_stack[0][2]= ClampZ(z);

    while( stack_p >= 0 )
    {
        cx= block_stack[stack_p][0];
        cy= block_stack[stack_p][1];
        cz= block_stack[stack_p][2];
        stack_p--;
        cl= char( FireLight( cx, cy, cz ) );

        if( cl <= 1 )
            continue;

        c2x= ClampX(cx + 1 );//x+1
        if(  cl - FireLight( c2x, cy, cz ) > 1  &&
                Transparent(  c2x, cy, cz )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1, c2x, cy, cz );
            stack_p++;
            block_stack[stack_p][0]= c2x;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= cz;
        }

        c2x= ClampX(cx - 1 );//x-1
        if(  cl - FireLight( c2x, cy, cz ) > 1  &&
                Transparent( c2x, cy, cz )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1, c2x, cy, cz );
            stack_p++;
            block_stack[stack_p][0]= c2x;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= cz;
        }

        c2y= ClampY( cy + 1 );//y+1
        if(  cl - FireLight( cx, c2y, cz ) > 1  &&
                Transparent(  cx, c2y, cz )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1, cx, c2y, cz );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= c2y;
            block_stack[stack_p][2]= cz;
        }

        c2y= ClampY( cy - 1 );//y-1
        if(  cl - FireLight( cx, c2y, cz ) > 1  &&
                Transparent(  cx, c2y, cz )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1, cx, c2y, cz );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= c2y;
            block_stack[stack_p][2]= cz;
        }

        c2z= ClampZ( cz + 1 );//z+1
        if(  cl - FireLight( cx, cy, c2z ) > 1  &&
                Transparent(  cx, cy, c2z )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1,  cx, cy, c2z );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= c2z;
        }

        c2z= ClampZ( cz - 1 );//z1
        if(  cl - FireLight( cx, cy, c2z ) > 1  &&
                Transparent(  cx, cy, c2z )  != BLOCK_OPAQUE )
        {
            SetFireLightMap( cl - 1,  cx, cy, c2z );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= c2z;
        }

    }

}
void World::RemoveFireLight(const short x, const short y, const short z)
{
    unsigned char l= FireLight( x, y, z );
    if( l <= 1 )
        return;

    short cx, cy, cz, c2x, c2y, c2z;
    for( cx= ClampX(x - l + 1), c2x= ClampX( x + l - 1 ); cx<= c2x; cx++ )
        for( cy= ClampY(y - l + 1), c2y= ClampY( y + l - 1 ); cy<= c2y; cy++ )
            for( cz= ClampZ(z - l + 1), c2z= ClampZ( z + l - 1 ); cz<= c2z; cz++ )
                SetFireLightMap( 0, cx, cy, cz );


    cz= ClampZ(z - l ), c2z= ClampZ( z + l );
    for( cx= ClampX(x - l), c2x= ClampX( x + l ); cx<= c2x; cx++ )
        for( cy= ClampY(y - l ), c2y= ClampY( y + l ); cy<= c2y; cy++ )
        {
            AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );
            AddFireLight( cx, cy, c2z, FireLight( cx, cy, c2z ) );
        }

    cy= ClampY(y - l ), c2y= ClampY( y + l );
    for( cx= ClampX(x - l), c2x= ClampX( x + l ); cx<= c2x; cx++ )
        for( cz= ClampZ(z - l), c2z= ClampZ( z + l ); cz<= c2z; cz++ )
        {
            AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );
            AddFireLight( cx, c2y, cz, FireLight( cx, c2y, cz ) );
        }

    cx= ClampX(x - l), c2x= ClampX( x + l );
    for( cy= ClampY(y - l), c2y= ClampY( y + l ); cy<= c2y; cy++ )
        for( cz= ClampZ(z - l), c2z= ClampZ( z + l ); cz<= c2z; cz++ )
        {
            AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );
            AddFireLight( c2x, cy, cz, FireLight( c2x, cy, cz ) );
        }

    unsigned short shred_x= ClampX(x - l + 1)/ SHRED_WIDTH, shred_x2= ClampX(x + l - 1)/ SHRED_WIDTH,
                            shred_y = ClampY(y - l + 1)/ SHRED_WIDTH, shred_y2= ClampY(y + l - 1)/ SHRED_WIDTH;

    cx= ClampX(x - l + 1), c2x= ClampX( x + l - 1 );
    cy= ClampY(y - l + 1), c2y= ClampY( y + l - 1 );
    cz= ClampZ(z - l + 1), c2z= ClampZ( z + l - 1 );

    short light_x, light_y, light_z;
    for( ; shred_x <= shred_x2; shred_x++ )
        for( ; shred_y <= shred_y2; shred_y++ )
        {
            Shred* s= shreds[ shred_y * numShreds + shred_x ];
            QLinkedList<Active *>::const_iterator i= s-> ShiningBegin();


            for ( ; i != s->ShiningEnd(); ++i)
            {
                light_x= ClampX( (*i)->X() );
                light_y= ClampY( (*i)->Y() );
                light_z= ClampZ( (*i)->Z() );
                AddFireLight( light_x, light_y, light_z,
                              (*i)->LightRadius() );
            }
        }
}

void World::RemoveSunLight( short x, short y, short z )
{
    short cx, cy, cz, c2x, c2y, c2z;

    for( cz= z; z>=0 && SunLight( x, y, cz ) > 0 ; z-- )
        SetSunLightMap( 0, x, y, cz );

    SunShineVertical( x, y );
}

//make block emit shining
//receives only non-sun light, from 0 to F
void World::Shine(const ushort i, const ushort j, const ushort k,
                  const uchar level, const bool init)
{
    AddFireLight( ClampX(i), ClampY(j), ClampZ(k), level );
}

void World::SunShineVertical(const short x, const short y, const short z, unsigned char level  )
{
    unsigned char t;
    unsigned char l= level;
    for( short z= HEIGHT - 2; z >= 0 && l > 0; z-- )
    {
        t= Transparent( x, y, z );
        if( t == BLOCK_OPAQUE )
            break;
        else if( t != INVISIBLE )
            l--;
        SetSunLightMap( l, x, y, z );
    }
}

void World::SunShineHorizontal(const short x , const short y , const short z )
{
    unsigned char l= SunLight( x, y, z );
    if( l <= 1 )
        return;

    static short block_stack[ MAX_SUN_LIGHT_LEVEL * MAX_SUN_LIGHT_LEVEL * MAX_SUN_LIGHT_LEVEL * 8 ][3];

    int stack_p= 0;
    short cx, cy, cz, c2x, c2y, c2z;
    char cl;

    block_stack[0][0]= ClampX(x);
    block_stack[0][1]= ClampY(y);
    block_stack[0][2]= ClampZ(z);

    while( stack_p >= 0 )
    {
        cx= block_stack[stack_p][0];
        cy= block_stack[stack_p][1];
        cz= block_stack[stack_p][2];
        stack_p--;
        cl= char( SunLight( cx, cy, cz ) );

        if( cl <= 1 )
            continue;

        c2x= ClampX(cx + 1 );//x+1
        if(  cl - SunLight( c2x, cy, cz ) > 1  &&
                Transparent(  c2x, cy, cz )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1, c2x, cy, cz );
            stack_p++;
            block_stack[stack_p][0]= c2x;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= cz;
        }

        c2x= ClampX(cx - 1 );//x-1
        if(  cl - SunLight( c2x, cy, cz ) > 1  &&
                Transparent( c2x, cy, cz )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1, c2x, cy, cz );
            stack_p++;
            block_stack[stack_p][0]= c2x;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= cz;
        }

        c2y= ClampY( cy + 1 );//y+1
        if(  cl - SunLight( cx, c2y, cz ) > 1  &&
                Transparent(  cx, c2y, cz )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1, cx, c2y, cz );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= c2y;
            block_stack[stack_p][2]= cz;
        }

        c2y= ClampY( cy - 1 );//y-1
        if(  cl - SunLight( cx, c2y, cz ) > 1  &&
                Transparent(  cx, c2y, cz )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1, cx, c2y, cz );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= c2y;
            block_stack[stack_p][2]= cz;
        }

        c2z= ClampZ( cz + 1 );//z+1
        if(  cl - SunLight( cx, cy, c2z ) > 1  &&
                Transparent(  cx, cy, c2z )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1,  cx, cy, c2z );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= c2z;
        }

        c2z= ClampZ( cz - 1 );//z-1
        if(  cl - SunLight( cx, cy, c2z ) > 1  &&
                Transparent(  cx, cy, c2z )  != BLOCK_OPAQUE )
        {
            SetSunLightMap( cl - 1,  cx, cy, c2z );
            stack_p++;
            block_stack[stack_p][0]= cx;
            block_stack[stack_p][1]= cy;
            block_stack[stack_p][2]= c2z;
        }

    }
}

/*void World::UpShine(const ushort x, const ushort y, const ushort z_bottom)
{
    if ( InBounds(x, y) )
    {
        for (ushort z=z_bottom; SetSunLightMap(1, x, y, z); ++z)
        {
            emit Updated(x, y, z);
        }
    }
}*/

//called when onet block is moved, built, or destroyed.
void World::ReEnlighten(const ushort i, const ushort j, const ushort k)
{
    SunShineVertical(i, j);
    SunShineHorizontal( i, j, k );
    RemoveFireLight( i, j, k );
    //Shine(i, j, k, GetBlock(i, j, k)->LightRadius(), true);
    emit Updated(i, j, k);
}

void World::ReEnlightenTime()
{
    sunMoonFactor=( NIGHT==PartOfDay() ) ?
                  MOON_LIGHT_FACTOR : SUN_LIGHT_FACTOR;
}

void World::ReEnlightenAll()
{
    disconnect(this, SIGNAL(Updated(
                                const ushort, const ushort, const ushort)), 0, 0);
    disconnect(this, SIGNAL(UpdatedAround(
                                const ushort, const ushort, const ushort,
                                const ushort)), 0, 0);
    for (ushort i=0; i<NumShreds()*NumShreds(); ++i)
    {
        shreds[i]->ShineAll();
    }
    emit ReConnect();
}

void World::ReEnlightenMove(const int dir)
{
    disconnect(this, SIGNAL(Updated(
                                const ushort,
                                const ushort,
                                const ushort)), 0, 0);
    disconnect(this, SIGNAL(UpdatedAround(
                                const ushort,
                                const ushort,
                                const ushort,
                                const ushort)), 0, 0);

    switch ( dir )
    {
    case NORTH:
        for (ushort i=0; i<NumShreds(); ++i)
        {
            shreds[i]->ShineAll();
            shreds[i+NumShreds()]->ShineAll();
        }
        break;
    case SOUTH:
        for (ushort i=0; i<NumShreds(); ++i)
        {
            shreds[i+NumShreds()*(NumShreds()-1)]->
            ShineAll();
            shreds[i+NumShreds()*(NumShreds()-2)]->
            ShineAll();
        }
        break;
    case EAST:
        for (ushort i=0; i<NumShreds(); ++i)
        {
            shreds[NumShreds()*i+NumShreds()-1]->
            ShineAll();
            shreds[NumShreds()*i+NumShreds()-2]->
            ShineAll();
        }
        break;
    case WEST:
        for (ushort i=0; i<NumShreds(); ++i)
        {
            shreds[NumShreds()*i]->ShineAll();
            shreds[NumShreds()*i+1]->ShineAll();
        }
        break;
    default:
        fprintf(stderr,
                "World::ReEnlightenMove: direction (?): %d\n",
                dir);
    }
    emit ReConnect();
}

uchar World::Enlightened(const ushort x, const ushort y, const ushort z)
const
{
    const uchar light=LightMap(x, y, z);
    return (light & 0x0F) * sunMoonFactor +
           (light & 0xF0) * FIRE_LIGHT_FACTOR;
}

//returns ligting of the block.
//if block side lighting is required, just remove first return.
/*uchar World::Enlightened(const ushort i, const ushort j, const ushort k,
                         const int dir)
const
{
    //provides lighting of block side, not all block
    ushort x, y, z;
    Focus(i, j, k, x, y, z, dir);
    return qMin(Enlightened(i, j, k), Enlightened(x, y, z));
}*/

uchar World::SunLight(const ushort i, const ushort j, const ushort k)
const
{
    return GetShred(i, j)->
           SunLight(i%SHRED_WIDTH, j%SHRED_WIDTH, k );
}

uchar World::FireLight(const ushort i, const ushort j, const ushort k)
const
{
    return GetShred(i, j)->
           FireLight(i%SHRED_WIDTH, j%SHRED_WIDTH, k );
}

void World::ReEnlightenBlockAdd( ushort x, ushort y, ushort z)
{
    unsigned char l= FireLight( x, y, z );
    RemoveFireLight( x, y, z );
    emit UpdatedAround( x, y, z, l );
}
void World::ReEnlightenBlockRemove( ushort x, ushort y, ushort z)
{
    short cx, cy, cz;

    cx= ClampX( x + 1 ), cy= ClampY( y ), cz= ClampZ( z );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    cx= ClampX( x - 1 );// cy= ClampY( y ), cz= ClampZ( z );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    cx= ClampX( x ), cy= ClampY( y + 1 ), cz= ClampZ( z );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    /*cx= ClampX( x ),*/
    cy= ClampY( y - 1 );// cz= ClampZ( z );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    cx= ClampX( x ), cy= ClampY( y ), cz= ClampZ( z + 1 );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    /*cx= ClampX( x ), cy= ClampY( y ),*/
    cz= ClampZ( z - 1 );
    AddFireLight( cx, cy, cz, FireLight( cx, cy, cz ) );

    emit UpdatedAround( x, y, z, FireLight( x, y, z ) + 1 );
}

// Shred methods

uchar Shred::Lightmap(const short x, const short y, const short z) const
{
    return lightMap[x][y][z];
}

uchar Shred::FireLight(const short x, const short y, const short z) const
{
    return (lightMap[x][y][z] & 0xF0 )>>4;
}

uchar Shred::SunLight(const short x, const short y, const short z) const
{
    return lightMap[x][y][z] & 0x0F;
}

uchar Shred::LightLevel(const short x, const short y, const short z) const
{
    const uchar light=lightMap[x][y][z];
    return (light & 0x0F) +
           (light & 0xF0) * FIRE_LIGHT_FACTOR;
}

//fire:sun
bool Shred::SetSunLight(const short x, const short y, const short z,
                        const uchar level)
{
    lightMap[x][y][z]&= 0xF0;
    lightMap[x][y][z]|= level;
}

//fire:sun
bool Shred::SetFireLight(const short x, const short y, const short z,
                         const uchar level)
{
    lightMap[x][y][z]&= 0x0F;
    lightMap[x][y][z]|= level << 4;
}

void Shred::SetLightmap(const short x, const short y, const short z,
                        const uchar level)
{
    lightMap[x][y][z]=level;
}

void Shred::SetAllLightMapNull()
{
    for (ushort i=0; i<SHRED_WIDTH; ++i)
        for (ushort j=0; j<SHRED_WIDTH; ++j)
            for (ushort k=0; k<HEIGHT-1; ++k)
            {
                lightMap[i][j][k]=0;
            }
}

//make all shining blocks of shred shine.
void Shred::ShineAll()
{
    QLinkedList<Active *>::const_iterator i=shiningList.constBegin();
    for ( ; i != shiningList.constEnd(); ++i)
    {
        world->AddFireLight((*i)->X(), (*i)->Y(), (*i)->Z(),
                            (*i)->LightRadius() );
    }
    for (ushort i=shredX*SHRED_WIDTH; i<SHRED_WIDTH*(shredX+1); ++i)
        for (ushort j=shredY*SHRED_WIDTH; j<SHRED_WIDTH*(shredY+1); ++j)
        {
            world->SunShineVertical(i, j);
        }
    for (ushort i=shredX*SHRED_WIDTH; i<SHRED_WIDTH*(shredX+1); ++i)//
        for (ushort j=shredY*SHRED_WIDTH; j<SHRED_WIDTH*(shredY+1); ++j)
            for( short k=HEIGHT-2; k>= 0; k-- )
            {
                world->SunShineHorizontal( i, j, k );
            }
}

void Shred::RemoveAllSunLight() {}
void Shred::RemoveAllFireLight() {}
void Shred::RemoveAllLight() {}
