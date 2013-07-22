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

const bool FLAT_GENERATION=false;

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

	//needed in Shred::ReloadTo... for active blocks not to reload twice
	//when they are registered both in frequent and rare lists.
	QList<Active *> activeListAll;
	QList<Active *> activeListFrequent;
	QList<Active *> activeListRare;
	QList<Active *> fallList;
	QList<Active *> shiningList;

	///Lowest nullstone and sky are not in bounds.
	bool InBounds(ushort x, ushort y, ushort z) const;

	public:
	///Returns y (line) shred coordinate on world map.
	long Longitude() const;
	///Returns x (column) shred coordinate on world map.
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
	void AddShining(Active *);
	void RemShining(Active *);

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
	void PutNormalBlock(int sub, ushort x, ushort y, ushort z);
	static Block * Normal(int sub);

	uchar LightMap(ushort x, ushort y, ushort z) const;
	bool SetSunLightMap(uchar level, ushort x, ushort y, ushort z);
	bool SetFireLightMap(uchar level, ushort x, ushort y, ushort z);
	void SetAllLightMap(uchar level);
	void ShineAll();

	int Sub(ushort x, ushort y, ushort z) const;
	int Kind(ushort x, ushort y, ushort z) const;
	int Durability(ushort x, ushort y, ushort z) const;
	int Movable(ushort x, ushort y, ushort z) const;
	int Transparent(ushort x, ushort y, ushort z) const;
	ushort Weight(ushort x, ushort y, ushort z) const;
	uchar LightRadius(ushort x, ushort y, ushort z) const;

	bool LoadShred(QFile &);

	Shred(World *,
			ushort shred_x, ushort shred_y,
			long longi, long lati);
	~Shred();

	void SetNewBlock(int kind, int sub, ushort x, ushort y, ushort z,
			int dir=UP);
	private:
	void RegisterBlock(Block *, ushort x, ushort y, ushort z);

	private:
	QString FileName() const;
	public: //TODO: make private (made public for map in screen)
	char TypeOfShred(long longi, long lati) const;
	private:

	void NormalUnderground(ushort depth=0, int sub=SOIL);
	void CoverWith(int kind, int sub);
	///Puts num things(kind-sub) in random places on shred surface.
	/**If on_water is false, this will not drop things on water,
	 * otherwise on water too.
	 */
	void RandomDrop(ushort num, int kind, int sub, bool on_water=false);

	void PlantGrass();
	void TestShred();
	void NullMountain();
	void Plain();
	void Forest();
	void Water();
	void Pyramid();
	void Mountain();
	void Hill();
	void Desert();
	//block combinations section (trees, buildings, etc):
	bool Tree(ushort x, ushort y, ushort z, ushort height);
	private:
	void NormalCube(ushort x_start, ushort y_start, ushort z_start,
			ushort x_end, ushort y_end, ushort z_end,
			int sub);

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
