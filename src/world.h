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

#include <cmath>
#include <QReadWriteLock>
#include <QThread>
#include "header.h"

class Block;
class Dwarf;
class Inventory;
class Active;
class Shred;

enum deferred_actions {
	DEFERRED_NOTHING,
	DEFERRED_MOVE,
	DEFERRED_JUMP,
	DEFERRED_BUILD,
	DEFERRED_DAMAGE,
	DEFERRED_THROW
}; //enum deferred_actions

const ushort safe_fall_height=5;

const uchar MOON_LIGHT_FACTOR=1;
const uchar  SUN_LIGHT_FACTOR=8;

class World : public QThread {
	Q_OBJECT

	static const ushort time_steps_in_sec=10;

	ulong time;
	ushort timeStep;
	Shred ** shreds;
	//center of active zone, longitude is y, latitude is x
	long longitude, latitude;
	long spawnLongi, spawnLati;
	const QString worldName;
	ushort numShreds; //size of loaded zone
	ushort numActiveShreds; //size of active zone
	QReadWriteLock rwLock;

	bool cleaned;
	ushort sun_moon_x;
	bool ifStar;

	long mapSize;

	long newLati, newLongi;
	ushort newNumShreds, newNumActiveShreds;
	ushort newX, newY, newZ;
	volatile bool toReSet;

	ushort deferredActionX;
	ushort deferredActionY;
	ushort deferredActionZ;
	ushort deferredActionXFrom;
	ushort deferredActionYFrom;
	ushort deferredActionZFrom;
	quint8 deferredActionDir;
	Block * deferredActionWhat;
	int deferredActionData1;
	int deferredActionData2;
	int deferredActionType;

	uchar sunMoonFactor;

	void ReplaceWithNormal(ushort x, ushort y, ushort z);
	Block * ReplaceWithNormal(Block * block);
	void MakeSun();
	void RemSun();
	void LoadAllShreds();
	void SaveAllShreds();

	protected:
	void run();

	//block work section
	public:
	Block * GetBlock(ushort x, ushort y, ushort z) const;
	Shred * GetShred(ushort i, ushort j) const;
	///Puts block to coordinates xyz and activates it.
	void SetBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts block to coordinates and not activates it.
	void PutBlock(Block * block, ushort x, ushort y, ushort z);
	///Puts normal block to coordinates.
	void PutNormalBlock(subs sub, ushort x, ushort y, ushort z);
	static Block * Normal(int sub);
	static void DeleteBlock(Block * block);

	//lighting section
	public:
	uchar Enlightened(ushort x, ushort y, ushort z) const;
	uchar Enlightened(ushort x, ushort y, ushort z, int dir) const;
	uchar SunLight(ushort x, ushort y, ushort z) const;
	uchar FireLight(ushort x, ushort y, ushort z) const;

	private:
	uchar LightMap(ushort x, ushort y, ushort z) const;
	bool SetLightMap(uchar level, ushort x, ushort y, ushort z);

	void ReEnlighten(ushort i, ushort j, ushort k);
	void ReEnlightenAll();
	void ReEnlightenTime();
	///Called from World::ReloadShreds(int), enlighens only need shreds.
	void ReEnlightenMove(int direction);

	void SunShine(ushort i, ushort j);
	void Shine(ushort x, ushort y, ushort z, uchar level, bool init=false);

	//information section
	public:
	QString WorldName() const;
	int Focus(
			ushort, ushort, ushort,
			ushort &,  ushort &, ushort &,
			quint8 dir) const;
	int Focus(
			ushort, ushort, ushort,
			ushort &, ushort &, ushort &) const;
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
	ushort SunMoonX() const;
	quint8 MakeDir(
			ushort x_cent, ushort y_cent,
			ushort x_targ, ushort y_targ) const;
	float Distance(
			ushort x_from, ushort y_from, ushort z_from,
	                ushort x_to,   ushort y_to,   ushort z_to) const;

	//visibility section
	public:
	bool DirectlyVisible(
			float, float, float,
			ushort, ushort, ushort) const;
	bool Visible(
			ushort x_from, ushort y_from, ushort z_from,
			ushort x_to,   ushort y_to,   ushort z_to) const;

	//movement section
	public:
	///Check and move
	int Move(ushort x, ushort y, ushort z, quint8 dir);
	///This CAN move blocks, but not xyz block.
	int CanMove(
		ushort x, ushort y, ushort z,
		ushort x_to, ushort y_to, ushort z_to,
		quint8 dir);
	void NoCheckMove(
		ushort x, ushort y, ushort z,
		ushort x_to, ushort y_to, ushort z_to,
		quint8 dir);
	void Jump(ushort x, ushort y, ushort z);
	void Jump(ushort x, ushort y, ushort z, quint8 dir);
	///Set action that will be executed at start of next physics turn.
	/**It is needed by graphics screen to reduce execution time of
	 * player's actions.
	 * If several actions are set during one physics turn,
	 * only the last will be done.
	 */
	void SetDeferredAction(
			ushort x,
			ushort y,
			ushort z,
			quint8 dir,
			int action,
			ushort x_from=0,
			ushort y_from=0,
			ushort z_from=0,
			Block * what=0,
			int data1=0,
			int data2=0);

	//time section
	public:
	times_of_day PartOfDay() const;
	///This returns seconds from start of current day.
	int TimeOfDay() const;
	ulong Time() const;
	ushort MiniTime() const;

	//interactions section
	public:
	bool Damage(
			ushort x, ushort y, ushort z,
			ushort level=1, int dmg_kind=CRUSH);
	int Use(ushort x, ushort y, ushort z);
	int Build(
			Block * thing,
			ushort x, ushort y, ushort z,
			quint8 dir=UP,
			Block * who=0);
	void Inscribe(ushort x, ushort y, ushort z);
	void Eat(
			ushort i, ushort j, ushort k,
			ushort i_food, ushort j_food, ushort k_food);

	//inventory functions section
	private:
	int Exchange(
			ushort i_from, ushort j_from, ushort k_from,
			ushort i_to,   ushort j_to,   ushort k_to,
			ushort n);
	public:
	int Drop(ushort i, ushort j, ushort k, ushort n);
	int Get( ushort i, ushort j, ushort k, ushort n);
	int GetAll(ushort x_to, ushort y_to, ushort z_to);

	//block information section
	public:
	bool InBounds(ushort i, ushort j, ushort k=0) const;
	QString & FullName(QString &, ushort x, ushort y, ushort z) const;
	int Transparent(ushort x, ushort y, ushort z) const;
	int Durability(ushort x, ushort y, ushort z) const;
	int Kind(ushort x, ushort y, ushort z) const;
	int Sub (ushort x, ushort y, ushort z) const;
	int Movable(ushort x, ushort y, ushort z) const;
	float Weight(ushort x, ushort y, ushort z) const;
	uchar LightRadius(ushort x, ushort y, ushort z) const;
	Inventory * HasInventory(ushort x, ushort y, ushort z) const;

	Active * ActiveBlock(ushort x, ushort y, ushort z) const;

	QString & GetNote(QString &, ushort x, ushort y, ushort z) const;
	int Temperature(ushort x, ushort y, ushort z) const;

	void ReloadAllShreds(
		long lati, long longi,
		ushort new_x, ushort new_y, ushort new_z,
		ushort new_num_shreds);
	void SetNumActiveShreds(ushort num);

	private:
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
	void UpdatedAround(
			ushort, ushort, ushort,
			ushort level);
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
