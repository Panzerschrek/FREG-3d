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

#ifndef WORLD_H
#define WORLD_H

#include <QThread>
#include "header.h"

class WorldMap;
class Block;
class Shred;
class ShredStorage;
class QByteArray;
class QReadWriteLock;

const ushort SAFE_FALL_HEIGHT=5;

const uchar MOON_LIGHT_FACTOR=1;
const uchar  SUN_LIGHT_FACTOR=8;

class World : public QThread {
	/** \class World world.h
	 * \brief World provides global physics and shred connection.
	 *
	 * Designed to be single.
	 */
	Q_OBJECT

	static const ushort TIME_STEPS_IN_SEC=10;

	ulong time;
	ushort timeStep;
	Shred ** shreds;
	//    N
	//    |  E
	// W--+--> latitude ( x for shreds )
	//    |
	//  S v longitude ( y for shreds )
	// center of active zone:
	long longitude, latitude;
	long spawnLongi, spawnLati;
	const QString worldName;
	ushort numShreds; // size of loaded zone
	ushort numActiveShreds; // size of active zone
	QReadWriteLock * const rwLock;

	ushort sun_moon_x;
	bool ifStar;

	WorldMap * const map;

	long newLati, newLongi;
	ushort newX, newY, newZ;
	/// UP for no reset, DOWN for full reset, NSEW for side shift.
	volatile int toResetDir;

	uchar sunMoonFactor;

	ShredStorage * shredStorage;
	Shred * shredMemoryPool;

	// Block work section
	public:
	Block * GetBlock(ushort x, ushort y, ushort z) const;
	Shred * GetShred(ushort i, ushort j) const;

	private:
	/// Puts block to coordinates xyz and activates it.
	void SetBlock(Block * block, ushort x, ushort y, ushort z);
	/// Puts block to coordinates and not activates it.
	void PutBlock(Block * block, ushort x, ushort y, ushort z);
	static Block * Normal(int sub);
	static Block * NewBlock(int kind, int sub);
	static void DeleteBlock(Block * block);
	Block * ReplaceWithNormal(Block * block) const;

	void MakeSun();
	void RemSun();

	public:	// Lighting section
	uchar Enlightened(ushort x, ushort y, ushort z) const;
	uchar Enlightened(ushort x, ushort y, ushort z, int dir) const;
	uchar SunLight   (ushort x, ushort y, ushort z) const;
	uchar FireLight  (ushort x, ushort y, ushort z) const;
	uchar LightMap   (ushort x, ushort y, ushort z) const;
	uchar SunLightSmooth( ushort x, ushort y, ushort z) const;
	uchar FireLightSmooth( ushort x, ushort y, ushort z) const;

	short ClampX(short x) const;
	short ClampY(short y) const;
	short ClampZ(short z) const;

	void SunShineVertical  (short x, short y, short z=HEIGHT-2,
			uchar level= /*MAX_LIGHT_RADIUS*/ 12 );
	void SunShineHorizontal(short x, short y, short z);
	/// If init is false, light will not spread from non-invisible blocks.
	void Shine(ushort x, ushort y, ushort z, uchar level, bool init=false);
	void RemoveSunLight(short x, short y, short z);
	void AddFireLight   (short x, short y, short z, uchar level);
	private:
	bool SetSunLightMap (uchar level, ushort x, ushort y, ushort z);
	bool SetFireLightMap(uchar level, ushort x, ushort y, ushort z);

	void RemoveFireLight(short x, short y, short z);

	/// Called when block is moved.
	void ReEnlighten(ushort x, ushort y, ushort z);
	/// Called when block is built.
	void ReEnlightenBlockAdd(ushort x, ushort y, ushort k);
	/// Called when block is destroyed.
	void ReEnlightenBlockRemove(ushort x, ushort y, ushort k);
	void ReEnlightenAll();
	void ReEnlightenTime();
	/// Called from ReloadShreds(int), enlightens only needed shreds.
	void ReEnlightenMove(int direction);
	void UpShine(ushort x, ushort y, ushort z_bottom);

	public: // Information section
	QString WorldName() const;
	/// True on error, false if focus is received to _targ successfully.
	bool Focus(ushort x, ushort y, ushort z,
			ushort & x_targ, ushort & y_targ, ushort & z_targ,
			quint8 dir) const;
	ushort NumShreds() const;
	static quint8 TurnRight(quint8 dir);
	static quint8 TurnLeft (quint8 dir);
	static quint8 Anti(quint8 dir);
	long GetSpawnLongi() const;
	long GetSpawnLati()  const;
	long Longitude() const;
	long Latitude() const;
	static ushort TimeStepsInSec();

	char TypeOfShred(long longi, long lati);
	long MapSize() const;

	QByteArray * GetShredData(long longi, long lati);
	void SetShredData(QByteArray *, long longi, long lati);

	private:
	bool Focus(ushort x, ushort y, ushort z,
			ushort & x_targ, ushort & y_targ,
			ushort & z_targ) const;
	ushort SunMoonX() const;

	// Visibility section
	public:
	bool DirectlyVisible(float x_from, float y_from, float z_from,
			ushort x_to, ushort y_to, ushort z_to) const;
	bool Visible(ushort x_from, ushort y_from, ushort z_from,
			ushort x_to, ushort y_to, ushort z_to) const;
	private:
	bool PositiveVisible(float x_from, float y_from, float z_from,
			ushort x_to, ushort y_to, ushort z_to) const;
	bool NegativeVisible(float x_from, float y_from, float z_from,
			short x_to, short y_to, short z_to) const;

	// Movement section
	public:
	/// Check and move
	bool Move(ushort x, ushort y, ushort z, quint8 dir);
	/// This CAN move blocks, but not xyz block.
	bool CanMove(ushort x, ushort y, ushort z,
			ushort x_to, ushort y_to, ushort z_to, quint8 dir);
	void NoCheckMove(ushort x, ushort y, ushort z,
			ushort x_to, ushort y_to, ushort z_to, quint8 dir);
	void Jump(ushort x, ushort y, ushort z, quint8 dir);

	// Time section
	public:
	int PartOfDay() const;
	/// This returns seconds from start of current day.
	int TimeOfDay() const;
	/// Returns time in seconds since world creation.
	ulong Time() const;
	QString TimeOfDayStr() const;
	/// Returns number of physics steps since second start.
	ushort MiniTime() const;

	// Interactions section
	public:
	void Damage(ushort x, ushort y, ushort z, ushort level, int dmg_kind);
	void DestroyAndReplace(ushort x, ushort y, ushort z);
	bool Build(Block * thing,
			ushort x, ushort y, ushort z,
			quint8 dir=UP,
			Block * who=0,
			bool anyway=false);
	/// Returns true on success. Gets a string and inscribes block.
	bool Inscribe(ushort x, ushort y, ushort z);

	// Inventory functions section
	private:
	void Exchange(Block * block_from, Block * block_to,
			ushort src, ushort dest, ushort num);
	public:
	void Drop(Block * from,
			ushort x_to, ushort y_to, ushort z_to,
			ushort src, ushort dest, ushort num);
	void Get(Block * to,
			ushort x_from, ushort y_from, ushort z_from,
			ushort src, ushort dest, ushort num);
	void GetAll(ushort x_to, ushort y_to, ushort z_to);

	// Block information section
	public:
	// For more information, use World::GetBlock(x, y, z) and ->.
	bool InBounds   (ushort x, ushort y, ushort z=0) const;
	int  Transparent(ushort x, ushort y, ushort z) const;
	int  Sub        (ushort x, ushort y, ushort z) const;
	int  Temperature(ushort x, ushort y, ushort z) const;

	// World section
	public:
	void ReloadAllShreds(long lati, long longi,
		ushort new_x, ushort new_y, ushort new_z);
	private:
	void SetNumActiveShreds(ushort num);
	/// Also saves all shreds.
	void DeleteAllShreds();
	void LoadAllShreds();
	void ReloadShreds(int direction);
	void run();
	Shred ** FindShred(ushort x, ushort y) const;

	public:
	QReadWriteLock * GetLock() const;
	void WriteLock();
	void ReadLock();
	bool TryReadLock();
	void Unlock();

	public:
	World(const QString &);
	~World();

	public slots:
	void CleanAll();
	void PhysEvents();
	void SetReloadShreds(int direction);

	signals:
	void Notify(const QString &) const;
	void GetString(QString &) const;
	void Updated(ushort, ushort, ushort);
	void UpdatedAll();
	void UpdatedAround(ushort x, ushort y, ushort z, ushort level);
	/// Emitted when world active zone moved to int direction.
	void Moved(int);
	void ReConnect();
	/// This is emitted when a pack of updates is complete.
	void UpdatesEnded();
	void NeedPlayer(ushort, ushort, ushort);
	void StartReloadAll();
	void FinishReloadAll();
	void ExitReceived();
}; // class World

extern World * world;

#endif
