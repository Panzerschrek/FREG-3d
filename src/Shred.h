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

#ifndef SHRED_H
#define SHRED_H

#include <QList>
#include "blocks.h"

class QFile;
class World;

const char DEFAULT_SHRED='.';
const char OUT_BORDER_SHRED='~';

class Shred {
	Block * blocks[SHRED_WIDTH][SHRED_WIDTH][HEIGHT];
	uchar lightMap[SHRED_WIDTH][SHRED_WIDTH][HEIGHT];
	World * const world;
	const long longitude, latitude;
	ushort shredX, shredY;

	QList<Active *> activeListAll;
	QList<Active *> activeListFrequent;
	QList<Active *> activeListRare;
	QList<Active *> fallList;

	public:
	long Longitude() const;
	long Latitude()  const;
	ushort ShredX() const;
	ushort ShredY() const;
	void PhysEventsFrequent();
	void PhysEventsRare();

	void AddActive(Active *);
	void RemActive(Active *);
	void AddFalling(Active *);
	void RemFalling(Active *);
	void AddFalling(ushort x, ushort y, ushort z);

	World * GetWorld() const;

	void ReloadToNorth();
	void ReloadToEast();
	void ReloadToSouth();
	void ReloadToWest();

	Block * GetBlock(ushort x, ushort y, ushort z) const;
	///Puts block to coordinates xyz and activates it
	void SetBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts block to coordinates, not activates it (e.g. in World::Move)
	void PutBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts normal block to coordinates
	void PutNormalBlock(int sub, ushort x, ushort y, ushort z, int dir=UP);
	static Block * Normal(int sub, int dir=UP);

	uchar LightMap(ushort x, ushort y, ushort z) const;
	bool SetLightMap(uchar level, ushort x, ushort y, ushort z);
	void SetAllLightMap(uchar level);
	void ShineAll();

	int Sub(ushort x, ushort y, ushort z) const;
	int Kind(ushort x, ushort y, ushort z) const;
	int Durability(ushort x, ushort y, ushort z) const;
	int Movable(ushort x, ushort y, ushort z) const;
	int Transparent(ushort x, ushort y, ushort z) const;
	ushort Weight(ushort x, ushort y, ushort z) const;
	uchar LightRadius(ushort x, ushort y, ushort z) const;

	int LoadShred(QFile &);

	Shred(World *,
			ushort shred_x, ushort shred_y,
			long longi, long lati);
	~Shred();

	void SetNewBlock(int kind, int sub, ushort x, ushort y, ushort z);
	private:
	void RegisterBlock(Block *, ushort x, ushort y, ushort z);

	private:
	QString FileName() const;
	char TypeOfShred(long longi, long lati) const;

	void NormalUnderground(ushort depth=0, int sub=SOIL);
	void CoverWith(int kind, int sub, ushort thickness);

	void PlantGrass();
	void TestShred();
	void NullMountain();
	void Plain();
	void Forest(long, long);
	void Water( long, long);
	void Pyramid();
	void Mountain();
	void Hill();
	void Desert();
	//block combinations section (trees, buildings, etc):
	bool Tree(ushort x, ushort y, ushort z, ushort height);

	//land generation
	private:
	void ShredLandAmplitudeAndLevel(
			long longi, long lati,
			ushort* l,
			float* a) const;
	void ShredNominalAmplitudeAndLevel(
			char shred_type,
			ushort* l,
			float* a) const;
	void AddWater();
	ushort FlatUndeground(short depth=0);
};//class Shred
#endif
