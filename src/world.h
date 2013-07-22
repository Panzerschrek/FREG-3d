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

#ifndef WORLD_H
#define WORLD_H

#include <QReadWriteLock>
#include <QThread>
#include <QByteArray>
#include <QSettings>
#include "header.h"

class QTextStream;
class Block;
class Shred;
class DeferredAction;

const ushort SAFE_FALL_HEIGHT=5;

const uchar MOON_LIGHT_FACTOR=1;
const uchar  SUN_LIGHT_FACTOR=8;

class World : public QThread {
	Q_OBJECT

	static const ushort TIME_STEPS_IN_SEC=10;

	ulong time;
	ushort timeStep;
	Shred ** shreds;
	//center of active zone, longitude is y, latitude is x
	long longitude, latitude;
	long spawnLongi, spawnLati;
	const QString worldName;
	ushort numShreds; //size of loaded zone
	ushort maxXY; //for InBounds optimization
	ushort numActiveShreds; //size of active zone
	QReadWriteLock rwLock;

	bool cleaned;
	ushort sun_moon_x;
	bool ifStar;

	long mapSize;
	QByteArray worldMap;
	QTextStream * worldMapStream;

	long newLati, newLongi;
	ushort newNumShreds, newNumActiveShreds;
	ushort newX, newY, newZ;
	volatile bool toReSet;

	uchar sunMoonFactor;

	QSettings settings;
	QSettings game_settings;

	QList<DeferredAction *> defActions;

	//block work section
	public:
	Block * GetBlock(ushort x, ushort y, ushort z) const;
	Shred * GetShred(ushort i, ushort j) const;

	void AddDeferredAction(DeferredAction *);
	void RemDeferredAction(DeferredAction *);
	private:
	///Puts block to coordinates xyz and activates it.
	void SetBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts block to coordinates and not activates it.
	void PutBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts normal block to coordinates.
	void PutNormalBlock(subs sub, ushort x, ushort y, ushort z);
	static Block * Normal(int sub);
	static Block * NewBlock(int kind, int sub);
	static void DeleteBlock(Block * block);
	Block * ReplaceWithNormal(Block * block) const;

	void MakeSun();
	void RemSun();

	//lighting section
	public:
	uchar Enlightened(ushort x, ushort y, ushort z) const;
	uchar Enlightened(ushort x, ushort y, ushort z, int dir) const;
	uchar SunLight(ushort x, ushort y, ushort z) const;
	uchar FireLight(ushort x, ushort y, ushort z) const;


	uchar LightMap(ushort x, ushort y, ushort z) const;
	private:bool SetSunLightMap(uchar level, ushort x, ushort y, ushort z);
	bool SetFireLightMap(uchar level, ushort x, ushort y, ushort z);

	void ReEnlighten(ushort i, ushort j, ushort k);
	void ReEnlightenAll();
	void ReEnlightenTime();
	///Called from World::ReloadShreds(int), enlighens only need shreds.
	void ReEnlightenMove(int direction);

	void SunShine(ushort x, ushort y);
	void UpShine(ushort x, ushort y, ushort z_bottom);
	public:
	///If init is false, light will not spread from non-invisible blocks.
	void Shine(ushort x, ushort y, ushort z, uchar level, bool init=false);

	//information section
	public:
	QString WorldName() const;
	///True on error, false if focus is received to _targ successfully.
	bool Focus(ushort x, ushort y, ushort z,
			ushort & x_targ, ushort & y_targ, ushort & z_targ,
			quint8 dir) const;
	private:
	bool Focus(ushort x, ushort y, ushort z,
			ushort & x_targ,
			ushort & y_targ,
			ushort & z_targ) const;
	public:
	ushort NumShreds() const;
	ushort NumActiveShreds() const;
	static quint8 TurnRight(quint8 dir);
	static quint8 TurnLeft (quint8 dir);
	static quint8 Anti(quint8 dir);
	long GetSpawnLongi() const;
	long GetSpawnLati()  const;
	long Longitude() const;
	long Latitude() const;
	static ushort TimeStepsInSec();

	private:
	long MapSize() const;
	QTextStream * MapStream();
	ushort SunMoonX() const;

	//visibility section
	public:
	bool DirectlyVisible(
			float x_from, float y_from, float z_from,
			ushort x_to,  ushort y_to,  ushort z_to) const;
	bool Visible(
			ushort x_from, ushort y_from, ushort z_from,
			ushort x_to,   ushort y_to,   ushort z_to) const;
	private:
	bool PositiveVisible(
			float  x_from, float  y_from, float  z_from,
			ushort x_to,   ushort y_to,   ushort z_to) const;
	bool NegativeVisible(
			float x_from, float y_from, float z_from,
			short x_to,   short y_to,   short z_to) const;

	//movement section
	public:
	///Check and move
	bool Move(ushort x, ushort y, ushort z, quint8 dir);
	///This CAN move blocks, but not xyz block.
	bool CanMove(
		ushort x,    ushort y,    ushort z,
		ushort x_to, ushort y_to, ushort z_to,
		quint8 dir);
	void NoCheckMove(
		ushort x,    ushort y,    ushort z,
		ushort x_to, ushort y_to, ushort z_to,
		quint8 dir);
	void Jump(ushort x, ushort y, ushort z, quint8 dir);

	//time section
	public:
	times_of_day PartOfDay() const;
	///This returns seconds from start of current day.
	int TimeOfDay() const;
	///Returns time in seconds since world creation.
	ulong Time() const;
	QString TimeOfDayStr() const;
	///Returns number of physics steps since second start.
	ushort MiniTime() const;

	//interactions section
	public:
	void Damage(ushort x, ushort y, ushort z, ushort level, int dmg_kind);
	void DestroyAndReplace(ushort x, ushort y, ushort z);
	bool Build(Block * thing,
			ushort x, ushort y, ushort z,
			quint8 dir=UP,
			Block * who=0,
			bool anyway=false);
	void Inscribe(ushort x, ushort y, ushort z);
	///No bounds checks inside, use carefully.
	void Eat(ushort i, ushort j, ushort k,
			ushort i_food, ushort j_food, ushort k_food);

	//inventory functions section
	private:
	void Exchange(
			Block * block_from, Block * block_to,
			ushort src, ushort dest, ushort num);
	public:
	void Drop(Block * from,
			ushort x_to, ushort y_to, ushort z_to,
			ushort src, ushort dest, ushort num);
	void Get(Block * to,
			ushort x_from, ushort y_from, ushort z_from,
			ushort src, ushort dest, ushort num);
	void GetAll(ushort x_to, ushort y_to, ushort z_to);

	//block information section
	public:
	//For more information, use World::GetBlock(x, y, z) and ->.
	bool InBounds   (ushort x, ushort y, ushort z=0) const;
	int  Transparent(ushort x, ushort y, ushort z) const;
	int  Sub        (ushort x, ushort y, ushort z) const;
	int  Temperature(ushort x, ushort y, ushort z) const;

	//world section
	public:
	void ReloadAllShreds(long lati, long longi,
		ushort new_x, ushort new_y, ushort new_z,
		ushort new_num_shreds);
	void SetNumActiveShreds(ushort num);
	private:
	///Also saves all shreds.
	void DeleteAllShreds();
	void LoadAllShreds();
	void run();

	friend class Shred;

	public:
	void WriteLock();
	void ReadLock();
	bool TryReadLock();
	void Unlock();

	void EmitNotify(const QString & str) const;

	public:
	World(const QString &);
	~World();

	public slots:
	void CleanAll();
	void ReloadShreds(int);
	void PhysEvents();

	signals:
	void Notify(const QString &) const;
	void GetString(QString &) const;
	void Updated(ushort, ushort, ushort);
	void UpdatedAll();
	void UpdatedAround(ushort x, ushort y, ushort z, ushort level);
	///Emitted when world active zone moved to int direction.
	void Moved(int);
	void ReConnect();
	///This is emitted when a pack of updates is complete.
	void UpdatesEnded();
	void NeedPlayer(ushort, ushort, ushort);
	void StartReloadAll();
	void FinishReloadAll();
	void ExitReceived();
}; //class world

#endif
