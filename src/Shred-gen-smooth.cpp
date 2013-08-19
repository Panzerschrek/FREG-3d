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

#include "Shred.h"

const ushort SEA_LEVEL    = 58;
const ushort SEABED_LEVEL = 48;
const ushort PLANE_LEVEL  = 64;
const ushort HILL_LEVEL   = 76;
const ushort MOUNTAIN_LEVEL=88;

const float SEABED_AMPLITUDE = 12.0f;
const float PLANE_AMPLITUDE  =  8.0f;
const float HILL_AMPLITUDE   = 29.0f;
const float MOUNTAIN_AMPLITUDE=50.0f;

/* landscape generation:
 * [ level - amplitude; level + amplitude ]
 * faster- [ level - amplitude/2; level + amplitude/2 ]
 * perlin [ -1; 1]
 * h= level + amplitude * perlin
*/

float Noise2(const int x, const int y) { //range - [-1;1]
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
		1073741823.5f - 1.0f;
}

float InterpolatedNoise(const short x, const short y) { //range - [-1;1]
	const short X = x >> 6;
	const short Y = y >> 6;

	const float noise[4]={
		Noise2(X, Y),
		Noise2(X + 1, Y),
		Noise2(X + 1, Y + 1),
		Noise2(X, Y + 1)
	};

	const float dx = (float (x) - float (X << 6))*0.015625f;
	const float dy = (float (y) - float (Y << 6))*0.015625f;

	const float interp_x[2]={
		dy * noise[3] + (1.0f - dy) * noise[0],
		dy * noise[2] + (1.0f - dy) * noise[1]
	};
	return interp_x[1] * dx + interp_x[0] * (1.0f - dx);
}

float FinalNoise(short x, short y) { //range [-1;1]  middle= 0
	float r = 0.5f * InterpolatedNoise(x, y);

	x <<= 1, y <<= 1;
	r += 0.25f * InterpolatedNoise(x, y);

	x <<= 1, y <<= 1;
	return (r += 0.125f * InterpolatedNoise(x, y));
}

void Shred::ShredNominalAmplitudeAndLevel(const char shred_type,
		ushort * const l, float * const a)
const {
	switch (shred_type) {
		case SHRED_TESTSHRED:
		case SHRED_PYRAMID:
		case SHRED_FOREST:
		case SHRED_NULLMOUNTAIN:
		case SHRED_DESERT:
		default:
			*l = PLANE_LEVEL;
			*a = PLANE_AMPLITUDE;
		break;

		case SHRED_WATER:
			*l = SEABED_LEVEL;
			*a = SEABED_AMPLITUDE;
		break;

		case SHRED_MOUNTAIN:
			*l = MOUNTAIN_LEVEL;
			*a = MOUNTAIN_AMPLITUDE;
		break;
		case SHRED_HILL:
			*l = HILL_LEVEL;
			*a = HILL_AMPLITUDE;
		break;
	}
}

void Shred::ShredLandAmplitudeAndLevel(const long longi, const long lati,
		ushort * const l, float * const a)
const {
	const char shred_types[8]={
		TypeOfShred(longi + 1, lati),
		TypeOfShred(longi - 1, lati),
		TypeOfShred(longi, lati + 1),
		TypeOfShred(longi, lati - 1),

		TypeOfShred(longi + 1, lati + 1),
		TypeOfShred(longi - 1, lati - 1),
		TypeOfShred(longi - 1, lati + 1),
		TypeOfShred(longi + 1, lati - 1)
	};
	float a2;
	ushort l2;

	const char this_shred_type = TypeOfShred(longi, lati);
	ShredNominalAmplitudeAndLevel(this_shred_type, &l2, &a2);
	float amplitude, level;
	const float this_shred_amplitude = amplitude = a2;
	const float this_shred_level = level = float (l2);
	for (int i = 0; i < 8; ++i) {
		if (shred_types[i] != this_shred_type) {
			ShredNominalAmplitudeAndLevel(shred_types[i], &l2,&a2);
			amplitude += (a2 - this_shred_amplitude) * (1.0f/9.0f);
			level += (float (l2) - this_shred_level)*(1.0f / 9.0f);
		}
	}
	*a = amplitude;
	*l = ushort(level);
}

void Shred::AddWater() {
	for (long longi=longitude-1; longi<=longitude+1; ++longi)
	for (long lati =latitude -1; lati <=latitude +1; ++lati ) {
		if ( SHRED_WATER==TypeOfShred(longi, lati) ) {
			for (ushort i = 0; i < SHRED_WIDTH; ++i)
			for (ushort j = 0; j < SHRED_WIDTH; ++j) {
				for (ushort k=SEA_LEVEL;
						AIR==Sub(i, j, k); --k)
				{
					SetNewBlock(LIQUID, WATER, i, j, k);
				}
			}
			return;
		}
	}
}

ushort Shred::FlatUndeground(short depth) {
	ushort level;
	float amplitude;
	ShredLandAmplitudeAndLevel(longitude, latitude, &level, &amplitude);

	if ( level >= HEIGHT-2 ) {
		level=HEIGHT-2;
	}
	for (ushort i = 0; i < SHRED_WIDTH; ++i)
	for (ushort j = 0; j < SHRED_WIDTH; ++j) {
		ushort k;
		for (k = 1; k <= level - depth - 6; ++k) {
			PutNormalBlock(STONE, i, j, k);
		}
		for (; k <= level; ++k) {
			PutNormalBlock(SOIL, i, j, k);
		}
	}
	return level;
}

void Shred::NormalUnderground(const ushort depth, const int sub) {
	float amplitude[9];
	ushort level[9];
	const struct {
		ushort * lev;
		float * ampl;
	} ampllev[]={
		/* 7 1 6
		 * 3 8 2
		 * 4 0 5 */
		{&level[7], &amplitude[7]},
		{&level[1], &amplitude[1]},
		{&level[6], &amplitude[6]},
		{&level[3], &amplitude[3]},
		{&level[8], &amplitude[8]},
		{&level[2], &amplitude[2]},
		{&level[4], &amplitude[4]},
		{&level[0], &amplitude[0]},
		{&level[5], &amplitude[5]},
	};
	for (int i=-1; i<=1; ++i)
	for (int j=-1; j<=1; ++j) {
		ShredLandAmplitudeAndLevel(i+longitude, j+latitude,
			ampllev[(i+1)*3+j+1].lev,
			ampllev[(i+1)*3+j+1].ampl);
	}
	for (ushort i = 0; i < SHRED_WIDTH; ++i)
	for (ushort j = 0; j < SHRED_WIDTH; ++j) {
		/* 3+---+2
		 *  |...|
		 *  |...|
		 * 0+---+1 */
		//interpolate land amplitude and level
		float il[4], ia[4];
		float ik_x, ik_y;
		if (j < SHRED_WIDTH / 2 && i < SHRED_WIDTH / 2) {
			il[0] = float (level[3]), ia[0] = amplitude[3];
			il[1] = float (level[8]), ia[1] = amplitude[8];
			il[2] = float (level[1]), ia[2] = amplitude[1];
			il[3] = float (level[7]), ia[3] = amplitude[7];
			ik_x = float (i + SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j + SHRED_WIDTH / 2) * 0.0625f;
		} else if (j < SHRED_WIDTH / 2 && i >= SHRED_WIDTH / 2) {
			il[0] = float (level[8]), ia[0] = amplitude[8];
			il[1] = float (level[2]), ia[1] = amplitude[2];
			il[2] = float (level[6]), ia[2] = amplitude[6];
			il[3] = float (level[1]), ia[3] = amplitude[1];
			ik_x = float (i - SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j + SHRED_WIDTH / 2) * 0.0625f;
		} else if (j >= SHRED_WIDTH / 2 && i < SHRED_WIDTH / 2) {
			il[0] = float (level[4]), ia[0] = amplitude[4];
			il[1] = float (level[0]), ia[1] = amplitude[0];
			il[2] = float (level[8]), ia[2] = amplitude[8];
			il[3] = float (level[3]), ia[3] = amplitude[3];
			ik_x = float (i + SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j - SHRED_WIDTH / 2) * 0.0625f;
		} else {
			il[0] = float (level[0]), ia[0] = amplitude[0];
			il[1] = float (level[5]), ia[1] = amplitude[5];
			il[2] = float (level[2]), ia[2] = amplitude[2];
			il[3] = float (level[8]), ia[3] = amplitude[8];
			ik_x = float (i - SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j - SHRED_WIDTH / 2) * 0.0625f;
		}
		il[0] = il[0] * ik_y + (1.0f - ik_y) * il[3];
		il[1] = il[1] * ik_y + (1.0f - ik_y) * il[2];
		const float interp_level = il[0]*(1.0f - ik_x) + il[1] * ik_x;

		ia[0] = ia[0] * ik_y + (1.0f - ik_y) * ia[3];
		ia[1] = ia[1] * ik_y + (1.0f - ik_y) * ia[2];
		const float interp_amplitude = ia[0]*(1.0f-ik_x) + ia[1]*ik_x;

		const ushort h = qBound(2,
			short (interp_level + FinalNoise(
				latitude *16 + i,
				longitude*16 + j) * interp_amplitude) - depth,
			HEIGHT - 2);
		short dirt_h;
		if (h < 80) {
			dirt_h = 5;
			dirt_h+= short( 3.0f * FinalNoise(
				( latitude *16+i )*4,
				( longitude*16+j )*4 ) );
		} else if (h < 100) {
			dirt_h = (100 - h) * 6 / 20;
			dirt_h+= short( 3.0f * FinalNoise(
				( latitude *16+i )*4,
				( longitude*16+j )*4 ) );
			if( dirt_h < 0 ) {
				dirt_h = 0;
			}
		} else {
			dirt_h = 0;
		}

		ushort k;
		for (k = 1; k < h - dirt_h && h < HEIGHT - 1; ++k) {
			PutNormalBlock(STONE, i, j, k);
		}
		for (; k < h; ++k) {
			PutNormalBlock(sub, i, j, k);
		}
	}
} //Shred::NormalUnderground

void Shred::Plain() {
	NormalUnderground();
	AddWater();
	RandomDrop(qrand()%4, BUSH, WOOD);
	RandomDrop(qrand()%4, RABBIT, A_MEAT);
	PlantGrass();
}

void Shred::Forest() {
	NormalUnderground();
	AddWater();
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
	NormalUnderground();
	AddWater();
	PlantGrass();
}

void Shred::Hill() {
	NormalUnderground();
	PlantGrass();
}

void Shred::Mountain() {
	NormalUnderground();
	PlantGrass();
}

void Shred::Desert() {
	NormalUnderground(4, SAND);
	for (ushort i=0; i<4; ++i) {
		CoverWith(ACTIVE, SAND);
	}
	AddWater();
}

