	/* freg, Free-Roaming Elementary Game with open and interactive world
	*  Copyright (C) 2012-2013 Alexander 'mmaulwurff' Kromm
	*  mmaulwurff@gmail.com
	*
	* This file is part of FREG.
	*
	* FREG is free software: you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation, either version 3 of the License, or
	* (at your option) any later version.
	*
	* FREG is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*
	* You should have received a copy of the GNU General Public License
	* along with FREG. If not, see <http://www.gnu.org/licenses/>.
	*/

#include "Shred.h"

void Shred::ShredNominalAmplitudeAndLevel(const char,
		ushort * const, float * const)
const {}

void Shred::ShredLandAmplitudeAndLevel(const long, const long,
		ushort * const, float * const)
const {}

void Shred::AddWater() {}

ushort Shred::FlatUndeground(short) {
	NormalUnderground();
	return HEIGHT/2;
}

void Shred::NormalUnderground(const ushort depth, const int sub) {
	for (ushort x=0; x<SHRED_WIDTH; ++x)
	for (ushort y=0; y<SHRED_WIDTH; ++y) {
		ushort z=1;
		for ( ; z<HEIGHT/2-depth-6; ++z) {
			PutNormalBlock(STONE, x, y, z);
		}
		PutNormalBlock((qrand()%2 ? STONE : sub), x, y, z++);
		for ( ; z<=HEIGHT/2-depth; ++z) {
			PutNormalBlock(sub, x, y, z);
		}
	}
}

void Shred::Plain() {
	NormalUnderground();
	RandomDrop(qrand()%4, BUSH, WOOD);
	RandomDrop(qrand()%4, RABBIT, A_MEAT);
	PlantGrass();
}

void Shred::Forest() {
	NormalUnderground();
	ushort number_of_trees=0;
	for (long i=longitude-1; i<=longitude+1; ++i)
	for (long j=latitude -1; j<=latitude +1; ++j) {
		if ( SHRED_FOREST==TypeOfShred(i, j) ) {
			++number_of_trees;
		}
	}
	for (ushort i=0; i<number_of_trees; ++i) {
		const ushort x=qrand()%(SHRED_WIDTH-2);
		const ushort y=qrand()%(SHRED_WIDTH-2);
		for (ushort k=HEIGHT-2; ; --k) {
			const int sub=Sub(x, y, k);
			if ( sub!=AIR && sub!=WATER ) {
				if ( sub!=GREENERY && sub!=WOOD ) {
					Tree(x, y, k+1, 4+qrand()%5);
				}
				break;
			}
		}
	}
	PlantGrass();
}

void Shred::Water() {
	ushort depth=0;
	for (long longi=longitude-1; longi<=longitude+1; ++longi)
	for (long lati =latitude -1; lati <=latitude +1; ++lati ) {
		if ( SHRED_WATER==TypeOfShred(longi, lati) ) {
			depth+=2;
		}
	}
	NormalUnderground(depth);
	for (ushort i=HEIGHT/2-depth; i<HEIGHT/2; ++i) {
		CoverWith(LIQUID, WATER);
	}
}

void Shred::Hill() {
	NormalUnderground();
	ushort x, y, z;
	for (y=0; y<SHRED_WIDTH; ++y) { //north-south '/\'
		for (x=0; x<SHRED_WIDTH; ++x)
		for (z=0; z<SHRED_WIDTH/2-2; ++z) {
			if ( z <= -qAbs(x-SHRED_WIDTH/2)+
					SHRED_WIDTH/2-2 )
			{
				PutNormalBlock(SOIL, x, y, z+HEIGHT/2);
			}
		}
	}
	for (x=0; x<SHRED_WIDTH; ++x) { //east-west '/\'
		for (y=0; y<SHRED_WIDTH; ++y)
		for (z=0; z<SHRED_WIDTH/2-2; ++z) {
			if ( z <= -qAbs(y-SHRED_WIDTH/2)+
					SHRED_WIDTH/2-2 )
			{
				PutNormalBlock(SOIL, x, y, z+HEIGHT/2);
			}
		}
	}
	PlantGrass();
}

void Shred::Mountain() {
	NormalUnderground();
	/* ###
	 * #~#??? east bridge
	 * ###
	 *  ?
	 *  ? south bridge
	 *  ?
	 * */
	const ushort mount_top=3*HEIGHT/4;
	NormalCube(0, 0, 1,
		SHRED_WIDTH/2, SHRED_WIDTH/2, mount_top, STONE);
	//south bridge
	if ( SHRED_MOUNTAIN==TypeOfShred(longitude+1, latitude) ) {
		NormalCube(qrand()%(SHRED_WIDTH/2-1), SHRED_WIDTH/2, mount_top,
			2, SHRED_WIDTH/2, 1, STONE);
	}
	//east bridge
	if ( SHRED_MOUNTAIN==TypeOfShred(longitude, latitude+1) ) {
		NormalCube(SHRED_WIDTH/2, qrand()%(SHRED_WIDTH/2-1), mount_top,
			SHRED_WIDTH/2, 2, 1, STONE);
	}
	//water pool
	if ( !(qrand()%10) ) {
		for (ushort i=1; i<SHRED_WIDTH/2-1; ++i)
		for (ushort j=1; j<SHRED_WIDTH/2-1; ++j)
		for (ushort k=mount_top-3; k<=mount_top; ++k) {
			SetNewBlock(LIQUID, WATER, i, j, k);
		}
	}
	//cavern
	if ( !(qrand()%10) ) {
		NormalCube(SHRED_WIDTH/4-2, SHRED_WIDTH/4-2,
			HEIGHT/2+1, 4, 4, 3, AIR);
		const int entries=qrand()%15+1;
		if ( entries & 1 ) { // north entry
			NormalCube(SHRED_WIDTH/4-1, 0, HEIGHT/2+1,
				2, 2, 2, AIR);
		}
		if ( entries & 2 ) { // east entry
			NormalCube(SHRED_WIDTH/2-4, SHRED_WIDTH/4-1,
				HEIGHT/2+1, 2, 2, 2, AIR);
		}
		if ( entries & 4 ) { // south entry
			NormalCube(SHRED_WIDTH/4-1, SHRED_WIDTH/2-2,
				HEIGHT/2+1, 2, 2, 2, AIR);
		}
		if ( entries & 8 ) { // west entry
			NormalCube(0, SHRED_WIDTH/4-1, HEIGHT/2+1,
				2, 2, 2, AIR);
		}
	}
	PlantGrass();
} //Shred::Mountain

void Shred::Desert() {
	NormalUnderground(4, SAND);
	for (ushort i=0; i<4; ++i) {
		CoverWith(ACTIVE, SAND);
	}
}

