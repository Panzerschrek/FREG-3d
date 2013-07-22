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

/**\file Lighting.cpp
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

const uchar FIRE_LIGHT_FACTOR=4;

const uchar MAX_LIGHT_RADIUS=15;

//private. use Enlightened instead, which is smart wrapper of this.
uchar World::LightMap(const ushort x, const ushort y, const ushort z)
const {
	return GetShred(x, y)->LightMap(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

bool World::SetSunLightMap(const uchar level,
		const ushort x, const ushort y, const ushort z)
{
	return GetShred(x, y)->
		SetSunLightMap(level, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

bool World::SetFireLightMap(const uchar level,
		const ushort x, const ushort y, const ushort z)
{
	return GetShred(x, y)->
		SetFireLightMap(level, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

//private. make block emit shining
//receives only non-sun light, from 0 to F
void World::Shine(const ushort i, const ushort j, const ushort k,
		const uchar level,
		const bool init) //see default in class
{
	if ( !InBounds(i, j, k) || 0==level ) {
		return;
	}
	if ( INVISIBLE!=Transparent(i, j, k) ) {
		if ( SetFireLightMap(level << 4, i, j, k) ) {
			emit Updated(i, j, k);
		}
		if ( !init ) {
			return;
		}
	}
	Shine(i-1, j, k, level-1);
	Shine(i+1, j, k, level-1);
	Shine(i, j-1, k, level-1);
	Shine(i, j+1, k, level-1);
	Shine(i, j, k-1, level-1);
	Shine(i, j, k+1, level-1);
}

//private
void World::SunShine(const ushort x, const ushort y) {
	/* 2 1 3
	 *   *   First, light goes down, then divides to 4 branches
	 * ^ | ^ to N-S-E-W, and goes up.
	 * | | |
	 * | | |
	 * |<v>|
	 *   #     */
	ushort light_lev=MAX_LIGHT_RADIUS;
	ushort z=HEIGHT-2;
	for ( ; SetSunLightMap(light_lev, x, y, z); --z) {
		emit Updated(x, y, z);
		const ushort transparent=Transparent(x, y, z);
		if ( BLOCK_TRANSPARENT==transparent ) {
			--light_lev;
		} else if ( BLOCK_OPAQUE==transparent ) {
			break;
		}
	}
	UpShine(x-1, y, z);
	UpShine(x+1, y, z);
	UpShine(x, y-1, z);
	UpShine(x, y+1, z);
}

void World::UpShine(const ushort x, const ushort y, const ushort z_bottom) {
	if ( InBounds(x, y) ) {
		for (ushort z=z_bottom; SetSunLightMap(1, x, y, z); ++z) {
			emit Updated(x, y, z);
		}
	}
}

//private. called when onet block is moved, built, or destroyed.
void World::ReEnlighten(const ushort i, const ushort j, const ushort k) {
	SunShine(i, j);
	Shine(i, j, k, GetBlock(i, j, k)->LightRadius(), true);
	emit Updated(i, j, k);
}

//private. called when world is created.
void World::ReEnlightenTime() {
	for (ushort i=0; i<NumShreds()*NumShreds(); ++i) {
		shreds[i]->SetAllLightMap(0);
	}
	sunMoonFactor=( NIGHT==PartOfDay() ) ?
		MOON_LIGHT_FACTOR : SUN_LIGHT_FACTOR;
	ReEnlightenAll();
}

//private. called from ReEnlightenTime
void World::ReEnlightenAll() {
	disconnect(this, SIGNAL(Updated(
		const ushort, const ushort, const ushort)), 0, 0);
	disconnect(this, SIGNAL(UpdatedAround(
		const ushort, const ushort, const ushort,
		const ushort)), 0, 0);
	for (ushort i=0; i<NumShreds()*NumShreds(); ++i) {
		shreds[i]->ShineAll();
	}
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
				shreds[i+NumShreds()*(NumShreds()-1)]->
					ShineAll();
				shreds[i+NumShreds()*(NumShreds()-2)]->
					ShineAll();
			}
		break;
		case EAST:
			for (ushort i=0; i<NumShreds(); ++i) {
				shreds[NumShreds()*i+NumShreds()-1]->
					ShineAll();
				shreds[NumShreds()*i+NumShreds()-2]->
					ShineAll();
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
				"World::ReEnlightenMove: direction (?): %d\n",
				dir);
	}
	emit ReConnect();
}

uchar World::Enlightened(const ushort x, const ushort y, const ushort z)
const {
	const uchar light=LightMap(x, y, z);
	return (light & 0x0F) * sunMoonFactor +
	       (light & 0xF0) * FIRE_LIGHT_FACTOR;
}

//returns ligting of the block.
//if block side lighting is required, just remove first return.
uchar World::Enlightened(const ushort i, const ushort j, const ushort k,
		const int dir)
const {
	//provides lighting of block side, not all block
	ushort x, y, z;
	Focus(i, j, k, x, y, z, dir);
	return qMin(Enlightened(i, j, k), Enlightened(x, y, z));
}

uchar World::SunLight(const ushort i, const ushort j, const ushort k)
const {
	return (LightMap(i, j, k) & 0x0F) * sunMoonFactor;
}

uchar World::FireLight(const ushort i, const ushort j, const ushort k)
const {
	return (LightMap(i, j, k) & 0xF0) * FIRE_LIGHT_FACTOR;
}

uchar Shred::LightRadius(const ushort x, const ushort y, const ushort z)
const {
	return blocks[x][y][z]->LightRadius();
}

uchar Shred::LightMap(const ushort i, const ushort j, const ushort k)
const {
	return lightMap[i][j][k];
}

//fire:sun
bool Shred::SetSunLightMap(const uchar level,
		const ushort x, const ushort y, const ushort z)
{
	if ( ( lightMap[x][y][z] & 0x0F ) < level ) {
		(lightMap[x][y][z] &= 0xF0) |= level;
		return true;
	} else {
		return false;
	}
}

//fire:sun
bool Shred::SetFireLightMap(const uchar level,
		const ushort x, const ushort y, const ushort z)
{
	if ( ( lightMap[x][y][z] & 0xF0 ) < level ) {
		(lightMap[x][y][z] &= 0x0F) |= level;
		return true;
	} else {
		return false;
	}
}

void Shred::SetAllLightMap(const uchar level) {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j)
	for (ushort k=0; k<HEIGHT-1; ++k) {
		lightMap[i][j][k]=level;
	}
}

//make all shining blocks of shred shine.
void Shred::ShineAll() {
	for (ushort j=0; j<shiningList.size(); ++j) {
		Active const * const temp=shiningList.at(j);
		world->Shine(temp->X(), temp->Y(), temp->Z(),
			temp->LightRadius(), true);
	}
	for (ushort i=shredX*SHRED_WIDTH; i<SHRED_WIDTH*(shredX+1); ++i)
	for (ushort j=shredY*SHRED_WIDTH; j<SHRED_WIDTH*(shredY+1); ++j) {
		world->SunShine(i, j);
	}
}
