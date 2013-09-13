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

#include <QTimer>
#include <QDir>
#include <QSettings>
#include <QWriteLocker>
#include <QReadWriteLock>
#include <memory>
#include "Animal.h"
#include "Inventory.h"
#include "Shred.h"
#include "world.h"
#include "worldmap.h"
#include "BlockManager.h"
#include "ShredStorage.h"

const ushort MIN_WORLD_SIZE = 7;

World * world;

Shred * World::GetShred(ushort x, ushort y) const {
	return shreds[ (Shred::CoordOfShred(y))*NumShreds() +
	               (Shred::CoordOfShred(x)) ];
}

int World::TimeOfDay() const { return time % SECONDS_IN_DAY; }
long World::GetSpawnLongi() const { return spawnLongi; }
long World::GetSpawnLati()  const { return spawnLati; }
long World::Longitude() const { return longitude; }
long World::Latitude()  const { return latitude; }
long World::MapSize() const { return map->MapSize(); }
ulong World::Time() const { return time; }
ushort World::TimeStepsInSec() { return TIME_STEPS_IN_SEC; }
ushort World::MiniTime() const { return timeStep; }
ushort World::NumShreds() const { return numShreds; }
QString World::WorldName() const { return worldName; }

char World::TypeOfShred(const long longi, const long lati) {
	return map->TypeOfShred(longi, lati);
}

QByteArray * World::GetShredData(long longi, long lati) {
	return shredStorage->GetShredData(longi, lati);
}

void World::SetShredData(QByteArray * const data,
		const long longi, const long lati)
{
	shredStorage->SetShredData(data, longi, lati);
}

ushort World::SunMoonX() const {
	return ( NIGHT==PartOfDay() ) ?
		TimeOfDay()*SHRED_WIDTH*numShreds/
			SECONDS_IN_NIGHT :
		(TimeOfDay()-SECONDS_IN_NIGHT)*SHRED_WIDTH*numShreds/
			SECONDS_IN_DAYLIGHT;
}

int World::PartOfDay() const {
	const ushort time_day=TimeOfDay();
	if ( time_day < END_OF_NIGHT )   return NIGHT;
	if ( time_day < END_OF_MORNING ) return MORNING;
	if ( time_day < END_OF_NOON )    return NOON;
	return EVENING;
}

QString World::TimeOfDayStr() const {
	return tr("Time is %1:%2.").
		arg(TimeOfDay()/60).
		arg(TimeOfDay()%60, 2, 10, QChar('0'));
}

void World::Drop(Block * const block_from,
		const ushort x_to, const ushort y_to, const ushort z_to,
		const ushort src, const ushort dest, const ushort num)
{
	Block * block_to=GetBlock(x_to, y_to, z_to);
	if ( AIR==block_to->Sub() ) {
		SetBlock((block_to=NewBlock(PILE, DIFFERENT)),
			x_to, y_to, z_to);
	} else if ( WATER==block_to->Sub() ) {
		Block * const pile = NewBlock(PILE, DIFFERENT);
		SetBlock(pile, x_to, y_to, z_to);
		pile->HasInventory()->Get(block_to);
		block_to = pile;
	}
	Exchange(block_from, block_to, src, dest, num);
	emit Updated(x_to, y_to, z_to);
}

void World::Get(Block * const block_to,
		const ushort x_from, const ushort y_from, const ushort z_from,
		const ushort src, const ushort dest, const ushort num)
{
	Block * const block_from=GetBlock(x_from, y_from, z_from);
	Inventory * const inv=block_from->HasInventory();
	if ( inv && inv->Access() )
	{
		Exchange(block_from, block_to, src, dest, num);
	}
}

bool World::InBounds(const ushort x, const ushort y, const ushort z) const {
	static const ushort max_xy = SHRED_WIDTH*NumShreds();
	return ( x<max_xy && y<max_xy && z<HEIGHT );
}

void World::ReloadAllShreds(const long lati, const long longi,
	const ushort new_x, const ushort new_y, const ushort new_z)
{
	newLati  = lati;
	newLongi = longi;
	newX = new_x;
	newY = new_y;
	newZ = new_z;
	toResetDir = DOWN; // full reset
}

QReadWriteLock * World::GetLock() const { return rwLock; }
void World::WriteLock() { rwLock->lockForWrite(); }
void World::ReadLock()  { rwLock->lockForRead(); }
bool World::TryReadLock() { return rwLock->tryLockForRead(); }
void World::Unlock() { rwLock->unlock(); }

void World::run() {
	QTimer timer;
	connect(&timer, SIGNAL(timeout()),
		this, SLOT(PhysEvents()),
		Qt::DirectConnection);
	timer.start(1000/TimeStepsInSec());
	exec();
}

quint8 World::TurnRight(const quint8 dir) {
	switch ( dir ) {
	default:
		fprintf(stderr, "World::TurnRight:Unlisted dir: %d\n",
			(int)dir);
	// no break;
	case WEST:  return NORTH;
	case NORTH: return EAST;
	case EAST:  return SOUTH;
	case SOUTH: return WEST;
	case UP:
	case DOWN:  return dir;
	}
}
quint8 World::TurnLeft(const quint8 dir) {
	switch ( dir ) {
	default:
		fprintf(stderr, "TurnLeft:Unlisted dir: %d\n",
			(int)dir);
	// no break;
	case EAST:  return NORTH;
	case NORTH: return WEST;
	case WEST:  return SOUTH;
	case SOUTH: return EAST;
	case UP:
	case DOWN:  return dir;
	}
}

void World::MakeSun() {
	ifStar = ( STAR==Sub( (sun_moon_x=SunMoonX()), SHRED_WIDTH*numShreds/2,
		HEIGHT-1) );
	PutBlock(Normal(SUN_MOON),
		sun_moon_x, SHRED_WIDTH*numShreds/2, HEIGHT-1);
}

Block * World::GetBlock(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->
		GetBlock(Shred::CoordInShred(x), Shred::CoordInShred(y), z);
}

void World::SetBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->SetBlock(block,
		Shred::CoordInShred(x), Shred::CoordInShred(y), z);
}

void World::PutBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->PutBlock(block,
		Shred::CoordInShred(x), Shred::CoordInShred(y), z);
}

Block * World::Normal(const int sub) {
	return block_manager.NormalBlock(sub);
}
Block * World::NewBlock(const int kind, const int sub) {
	return block_manager.NewBlock(kind, sub);
}
void World::DeleteBlock(Block * const block) {
	Active * const active = block->ActiveBlock();
	if ( active ) {
		active->SetToDelete();
	} else {
		block_manager.DeleteBlock(block);
	}
}

Block * World::ReplaceWithNormal(Block * const block) const {
	if ( block!=Normal(block->Sub()) && *block==*Normal(block->Sub()) ) {
		const int sub=block->Sub();
		DeleteBlock(block);
		return Normal(sub);
	} else {
		return block;
	}
}

quint8 World::Anti(const quint8 dir) {
	switch ( dir ) {
	case NORTH: return SOUTH;
	case EAST:  return WEST;
	case SOUTH: return NORTH;
	case WEST:  return EAST;
	case UP:    return DOWN;
	case DOWN:  return UP;
	default:
		fprintf(stderr, "World::Anti(int): unlisted dir: %d\n",
			(int)dir);
		return HERE;
	}
}

Shred ** World::FindShred(const ushort x, const ushort y) const {
	return &shreds[x+y*numShreds];
}

void World::ReloadShreds(const int direction) {
	short x, y; // do not make unsigned, values <0 are needed for checks
	RemSun();
	switch ( direction ) {
	case NORTH:
		--longitude;
		for (x=0; x<numShreds; ++x) {
			const Shred * const shred = *FindShred(x, numShreds-1);
			Shred * const memory = shred->GetShredMemory();
			shred->~Shred();
			for (y=numShreds-1; y>0; --y) {
				( *FindShred(x, y) = *FindShred(x, y-1) )->
					ReloadToNorth();
			}
			*FindShred(x, 0) = new(memory)
				Shred(x, 0,
					longitude-numShreds/2,
					latitude -numShreds/2+x, memory);
		}
	break;
	case SOUTH:
		++longitude;
		for (x=0; x<numShreds; ++x) {
			const Shred * const shred = *FindShred(x, 0);
			Shred * const memory = shred->GetShredMemory();
			shred->~Shred();
			for (y=0; y<numShreds-1; ++y) {
				( *FindShred(x, y) = *FindShred(x, y+1) )->
					ReloadToSouth();
			}
			*FindShred(x, numShreds-1) = new(memory)
				Shred(x, numShreds-1,
					longitude+numShreds/2,
					latitude -numShreds/2+x, memory);
		}
	break;
	case EAST:
		++latitude;
		for (y=0; y<numShreds; ++y) {
			const Shred * const shred = *FindShred(0, y);
			Shred * const memory = shred->GetShredMemory();
			shred->~Shred();
			for (x=0; x<numShreds-1; ++x) {
				( *FindShred(x, y) = *FindShred(x+1, y) )->
					ReloadToEast();
			}
			*FindShred(numShreds-1, y) = new(memory)
				Shred(numShreds-1, y,
					longitude-numShreds/2+y,
					latitude +numShreds/2, memory);
		}
	break;
	case WEST:
		--latitude;
		for (y=0; y<numShreds; ++y) {
			const Shred * const shred = *FindShred(numShreds-1, y);
			Shred * const memory = shred->GetShredMemory();
			shred->~Shred();
			for (x=numShreds-1; x>0; --x) {
				( *FindShred(x, y) = *FindShred(x-1, y) )->
					ReloadToWest();
			}
			*FindShred(0, y) = new(memory)
				Shred(0, y,
					longitude-numShreds/2+y,
					latitude -numShreds/2, memory);
		}
	break;
	default: fprintf(stderr,
		"World::ReloadShreds(int): invalid direction: %d\n",
		direction);
	}
	shredStorage->Shift(direction, longitude, latitude);
	MakeSun();
	ReEnlightenMove(direction);
	emit Moved(direction);
} // void World::ReloadShreds(int direction)

void World::SetReloadShreds(const int direction) { toResetDir=direction; }

void World::PhysEvents() {
	const QWriteLocker writeLock(rwLock);
	switch ( toResetDir ) {
	case UP: break; // no reset
	case DOWN: // full reset
		emit StartReloadAll();
		DeleteAllShreds();
		longitude = newLongi;
		latitude  = newLati;
		LoadAllShreds();
		emit NeedPlayer(newX, newY, newZ);
		emit UpdatedAll();
		emit FinishReloadAll();
		toResetDir = UP; // set no reset
	break;
	default:
		ReloadShreds(toResetDir);
		toResetDir = UP; // set no reset
	}

	static const ushort start = numShreds/2-numActiveShreds/2;
	static const ushort end   = start+numActiveShreds;
	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->PhysEventsFrequent();
	}

	if ( TimeStepsInSec() > timeStep ) {
		++timeStep;
		for (ushort i=start; i<end; ++i)
		for (ushort j=start; j<end; ++j) {
			shreds[i+j*NumShreds()]->DeleteDestroyedActives();
		}
		return;
	} // else:

	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->PhysEventsRare();
	}
	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->DeleteDestroyedActives();
	}
	timeStep = 0;
	++time;
	// sun/moon moving
	if ( sun_moon_x != SunMoonX() ) {
		const ushort y = SHRED_WIDTH*numShreds/2;
		PutBlock(Normal(ifStar ? STAR : SKY), sun_moon_x, y, HEIGHT-1);
		emit Updated(sun_moon_x, y, HEIGHT-1);
		sun_moon_x = SunMoonX();
		ifStar = ( STAR==Sub(sun_moon_x, y, HEIGHT-1) );
		PutBlock(Normal(SUN_MOON), sun_moon_x, y, HEIGHT-1);
		emit Updated(sun_moon_x, y, HEIGHT-1);
	}
	switch ( TimeOfDay() ) {
	case END_OF_NIGHT:
		ReEnlightenTime();
		emit Notify(tr("It's morning now."));
	break;
	case END_OF_MORNING: emit Notify(tr("It's day now.")); break;
	case END_OF_NOON:    emit Notify(tr("It's evening now.")); break;
	case END_OF_EVENING:
		ReEnlightenTime();
		emit Notify(tr("It's night now."));
	break;
	}
	emit UpdatesEnded();
	// emit ExitReceived(); // close all after 1 turn
} // void World::PhysEvents()

bool World::DirectlyVisible(float x_from, float y_from, float z_from,
		const ushort x_to, const ushort y_to, const ushort z_to)
const {
	return ( x_from==x_to && y_from==y_to && z_from==z_to ) || (
		( x_to<x_from && y_to<y_from ) ||
		( x_to>x_from && y_to<y_from && y_to>(2*x_from-x_to-3) ) ||
		( x_to<x_from && y_to>y_from && y_to>(2*x_from-x_to-3) ) ) ?
			NegativeVisible(x_from, y_from, z_from,
				x_to, y_to, z_to) :
			PositiveVisible(x_from, y_from, z_from,
				x_to, y_to, z_to);
}

bool World::NegativeVisible(float x_from, float y_from, float z_from,
		short x_to, short y_to, const short z_to)
const {
	// this function is like World::PositiveVisible,
	// except those 4 lines:
	x_from=-x_from;
	y_from=-y_from;
	x_to=-x_to;
	y_to=-y_to;
	const ushort max=qMax(qAbs(x_to-(short)x_from),
		qMax(qAbs(z_to-(short)z_from), qAbs(y_to-(short)y_from)));
	const float x_step=(x_to-x_from)/max;
	const float y_step=(y_to-y_from)/max;
	const float z_step=(z_to-z_from)/max;
	for (ushort i=1; i<max; ++i) {
		if ( BLOCK_OPAQUE==Transparent(
				-qRound(x_from+=x_step),
				-qRound(y_from+=y_step),
				qRound(z_from+=z_step)) )
		{
			return false;
		}
	}
	return true;
}

bool World::PositiveVisible(float x_from, float y_from, float z_from,
		const ushort x_to, const ushort y_to, const ushort z_to)
const {
	const ushort max=qMax(qAbs(x_to-(short)x_from),
		qMax(qAbs(z_to-(short)z_from), qAbs(y_to-(short)y_from)));
	const float x_step=(x_to-x_from)/max;
	const float y_step=(y_to-y_from)/max;
	const float z_step=(z_to-z_from)/max;
	for (ushort i=1; i<max; ++i) {
		if ( BLOCK_OPAQUE==Transparent(
				qRound(x_from+=x_step),
				qRound(y_from+=y_step),
				qRound(z_from+=z_step)) )
		{
		   	return false;
		}
	}
	return true;
}

bool World::Visible(
		const ushort x_from, const ushort y_from, const ushort z_from,
		const ushort x_to,   const ushort y_to,   const ushort z_to)
const {
	short temp;
	return (
		(DirectlyVisible(x_from, y_from, z_from, x_to, y_to, z_to)) ||
		(Transparent(x_to+(temp=(x_to>x_from) ? (-1) : 1), y_to, z_to)
			&& DirectlyVisible(
				x_from,    y_from, z_from,
				x_to+temp, y_to,   z_to)) ||
		(Transparent(x_to, y_to+(temp=(y_to>y_from) ? (-1) : 1), z_to)
			&& DirectlyVisible(
				x_from, y_from,    z_from,
				x_to,   y_to+temp, z_to)) ||
		(Transparent(x_to, y_to, z_to+(temp=(z_to>z_from) ? (-1) : 1))
			&& DirectlyVisible(
				x_from, y_from, z_from,
				x_to,   y_to,   z_to+temp)) );
}

bool World::Move(const ushort x, const ushort y, const ushort z,
		const quint8 dir)
{
	ushort newx, newy, newz;
	Active * active;
	Block * block;
	if ( !Focus(x, y, z, newx, newy, newz, dir) &&
			CanMove(x, y, z, newx, newy, newz, dir) &&
			(DOWN==dir || !(block=GetBlock(x, y, z))->Weight() ||
			!( (active=block->ActiveBlock()) &&
				active->IsFalling() &&
				AIR==Sub(x, y, z-1) &&
				AIR==Sub(newx, newy, newz-1))) )
	{
		NoCheckMove(x, y, z, newx, newy, newz, dir);
		return true;
	} else {
		return false;
	}
}

bool World::CanMove(const ushort x, const ushort y, const ushort z,
		const ushort newx, const ushort newy, const ushort newz,
		const quint8 dir)
{
	if ( !InBounds(x, y, z) ) {
		return false;
	}
	Block * const block = GetBlock(x, y, z);
	if ( NOT_MOVABLE == block->Movable() ) {
		return false;
	}
	Block * block_to = GetBlock(newx, newy, newz);
	if ( ENVIRONMENT == block->Movable() ) {
		if ( *block == *block_to ) {
			return false;
		} else if ( MOVABLE == block_to->Movable() ) {
			NoCheckMove(x, y, z, newx, newy, newz, dir);
			return true;
		}
	}
	switch ( block_to->BeforePush(dir, block) ) {
	case MOVE_SELF: block_to = GetBlock(newx, newy, newz); break;
	case DESTROY:
		DeleteBlock(block_to);
		PutBlock(Normal(AIR), newx, newy, newz);
		return true;
	break;
	case JUMP:
		if ( DOWN!=dir && UP!=dir ) {
			Jump(x, y, z, dir);
			return false;
		}
	break;
	case MOVE_UP:
		if ( DOWN!=dir && UP!=dir ) {
			Move(x, y, z, UP);
			return false;
		}
	break;
	case DAMAGE:
		Damage(x, y, z,
			block_to->DamageLevel(),
			block_to->DamageKind());
		return false;
	break;
	}
	return ( ENVIRONMENT==block_to->Movable() );/* ||
		( (block->Weight() > block_to->Weight()) &&
			Move(newx, newy, newz, dir) ) );*/
} // bool World::CanMove(ushort x, y, z, newx, newy, newz, quint8 dir)

void World::NoCheckMove(const ushort x, const ushort y, const ushort z,
		const ushort newx, const ushort newy, const ushort newz,
		const quint8 dir)
{
	Block * const block = GetBlock(x, y, z);
	Block * const block_to = GetBlock(newx, newy, newz);

	PutBlock(block_to, x, y, z);
	PutBlock(block, newx, newy, newz);

	if ( block_to->Transparent() != block->Transparent() ) {
		ReEnlighten(newx, newy, newz);
		ReEnlighten(x, y, z);
	}

	block_to->Move( Anti(dir) );
	block->Move(dir);

	Shred * shred = GetShred(x, y);
	shred->AddFalling(Shred::CoordInShred(x), Shred::CoordInShred(y), z+1);
	shred->AddFalling(Shred::CoordInShred(x), Shred::CoordInShred(y), z);
	shred = GetShred(newx, newy);
	shred->AddFalling(Shred::CoordInShred(newx), Shred::CoordInShred(newy),
		newz+1);
	shred->AddFalling(Shred::CoordInShred(newx), Shred::CoordInShred(newy),
		newz);
}

void World::Jump(const ushort x, const ushort y, const ushort z,
		const quint8 dir)
{
	if ( !(AIR==Sub(x, y, z-1) && GetBlock(x, y, z)->Weight()) &&
			Move(x, y, z, UP) )
	{
		Move(x, y, z+1, dir);
	}
}

bool World::Focus(const ushort x, const ushort y, const ushort z,
		ushort & x_to, ushort & y_to, ushort & z_to, const quint8 dir)
const {
	x_to = x;
	y_to = y;
	z_to = z;
	switch ( dir ) {
	case NORTH: --y_to; break;
	case SOUTH: ++y_to; break;
	case EAST:  ++x_to; break;
	case WEST:  --x_to; break;
	case DOWN:  --z_to; break;
	case UP:    ++z_to; break;
	default:
		fprintf(stderr, "World::Focus: unlisted dir: %d\n", dir);
		return true;
	}
	return !InBounds(x_to, y_to, z_to);
}

bool World::Focus(const ushort x, const ushort y, const ushort z,
		ushort & x_target, ushort & y_target, ushort & z_target)
const {
	return Focus( x, y, z, x_target, y_target, z_target,
		GetBlock(x, y, z)->GetDir() );
}

void World::Damage(const ushort x, const ushort y, const ushort z,
		const ushort dmg, const int dmg_kind)
{
	Block * temp = GetBlock(x, y, z);
	if ( temp==Normal(temp->Sub()) && AIR!=temp->Sub() ) {
		SetBlock((temp=NewBlock(temp->Kind(), temp->Sub())), x, y, z);
	}
	if ( temp->Damage(dmg, dmg_kind) > 0 ) {
		temp->ReceiveSignal(SOUND_STRINGS[1]); // "Ouch!"
		if ( block_manager.MakeId(BLOCK, STONE)==temp->GetId() &&
				temp->Durability()!=MAX_DURABILITY )
		{ // convert stone into ladder
			DeleteBlock(temp);
			SetBlock(NewBlock(LADDER, STONE), x, y, z);
			emit ReEnlighten(x, y, z);
		} else {
			SetBlock(ReplaceWithNormal(temp), x, y, z);
		}
	}
}

void World::DestroyAndReplace(const ushort x, const ushort y, const ushort z) {
	Block * const temp = GetBlock(x, y, z);
	if ( temp->Durability() > 0 ) {
		return;
	}
	Block * const dropped = temp->DropAfterDamage();
	Shred * const shred = GetShred(x, y);
	const ushort x_in_shred = Shred::CoordInShred(x);
	const ushort y_in_shred = Shred::CoordInShred(y);
	if ( PILE!=temp->Kind() && (temp->HasInventory() || dropped) ) {
		Block * const new_pile=( ( dropped && PILE==dropped->Kind() ) ?
			dropped : NewBlock(PILE, DIFFERENT) );
		shred->SetBlock(new_pile, x_in_shred, y_in_shred, z);
		Inventory * const inv = temp->HasInventory();
		Inventory * const new_pile_inv = new_pile->HasInventory();
		if ( inv ) {
			new_pile_inv->GetAll(inv);
		}
		if ( dropped && PILE!=dropped->Kind() &&
				!new_pile_inv->Get(dropped) )
		{
			DeleteBlock(dropped);
		}
		shred->AddFalling(x_in_shred, y_in_shred, z);
	} else {
		PutBlock(Normal(AIR), x, y, z);
	}
	const int old_transparency = temp->Transparent();
	const uchar old_light = temp->LightRadius();
	DeleteBlock(temp);
	shred->AddFalling(x_in_shred, y_in_shred, z+1);
	if ( old_transparency != INVISIBLE ) {
		ReEnlightenBlockRemove(x, y, z);
	}
	if ( old_light ) {
		RemoveFireLight(x, y, z);
	}
} // void World::DestroyAndReplace(ushort x, y, z)

bool World::Build(Block * block,
		const ushort x, const ushort y, const ushort z,
		const quint8 dir, Block * const who, const bool anyway)
{
	Block * const target_block = GetBlock(x, y, z);
	if ( ENVIRONMENT!=target_block->Movable() && !anyway ) {
		if ( who ) {
			who->ReceiveSignal(tr("Cannot build here."));
		}
		return false;
	} // else:
	const int old_transparency = target_block->Transparent();
	DeleteBlock(target_block);
	block->Restore();
	block->SetDir(dir);
	block = ReplaceWithNormal(block);
	SetBlock(block, x, y, z);
	if ( old_transparency != block->Transparent() ) {
		ReEnlightenBlockAdd(x, y, z);
	}
	const uchar block_light = block->LightRadius();
	if ( block_light ) {
		AddFireLight(x, y, z, block_light);
	}
	return true;
}

bool World::Inscribe(const ushort x, const ushort y, const ushort z) {
	Block * block=GetBlock(x, y, z);
	if ( LIQUID==block->Kind() || AIR==block->Sub() ) {
		return false;
	}
	if ( block==Normal(block->Sub()) ) {
		SetBlock(block=NewBlock(block->Kind(), block->Sub()), x, y, z);
	}
	QString str=tr("No note received.");
	emit GetString(str);
	if ( block->Inscribe(str) ) {
		return true;
	} else {
		SetBlock(ReplaceWithNormal(block), x, y, z);
		return false;
	}
}

void World::Exchange(Block * const block_from, Block * const block_to,
		const ushort src, const ushort dest, const ushort num)
{
	Inventory * const inv_from=block_from->HasInventory();
	if ( !inv_from ) {
		block_from->ReceiveSignal(tr("No inventory."));
		return;
	}
	if ( !inv_from->Number(src) ) {
		block_from->ReceiveSignal(tr("Nothing here."));
		block_to  ->ReceiveSignal(tr("Nothing here."));
		return;
	}
	Inventory * const inv_to=block_to->HasInventory();
	if ( !inv_to ) {
		block_from->ReceiveSignal(tr("No room there."));
		return;
	}
	if ( inv_from->Drop(src, dest, num, inv_to) ) {
		block_from->ReceiveSignal(tr("Your bag is lighter now."));
		block_to  ->ReceiveSignal(tr("Your bag is heavier now."));
	}
}

void World::GetAll(const ushort x_to, const ushort y_to, const ushort z_to) {
	ushort x_from, y_from, z_from;
	if ( Focus(x_to, y_to, z_to, x_from, y_from, z_from) ) {
		return;
	}
	Inventory * const inv_to=GetBlock(x_to, y_to, z_to)->HasInventory();
	if ( inv_to ) {
		inv_to->GetAll(GetBlock(x_from, y_from, z_from)->
			HasInventory());
	}
}

int World::Transparent(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->
		GetBlock(Shred::CoordInShred(x), Shred::CoordInShred(y), z)->
			Transparent();
}
int World::Sub(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->
		Sub(Shred::CoordInShred(x), Shred::CoordInShred(y), z);
}

int World::Temperature(const ushort x, const ushort y, const ushort z) const {
	if ( HEIGHT-1 == z ) {
		return 0;
	}
	short temperature = GetBlock(x, y, z)->Temperature();
	if ( temperature ) {
		return temperature;
	}
	for (short i=x-1; i<=x+1; ++i)
	for (short j=y-1; j<=y+1; ++j)
	for (short k=z-1; k<=z+1; ++k) {
		if ( InBounds(i, j, k) ) {
			temperature += GetBlock(i, j, k)->Temperature();
		}
	}
	return temperature/2;
}

void World::RemSun() {
	SetBlock(
		Normal(ifStar ? STAR : SKY),
		sun_moon_x,
		SHRED_WIDTH*numShreds/2,
		HEIGHT-1);
}

void World::LoadAllShreds() {
	shreds = new Shred *[(ulong)numShreds*(ulong)numShreds];
	shredMemoryPool = static_cast<Shred *>
		(operator new (sizeof(Shred)*numShreds*numShreds));
	for (long i=latitude -numShreds/2, x=0; x<numShreds; ++i, ++x)
	for (long j=longitude-numShreds/2, y=0; y<numShreds; ++j, ++y) {
		shreds[y*numShreds+x] = new(shredMemoryPool+y*numShreds+x)
			Shred( x, y, j, i, (shredMemoryPool+y*numShreds+x) );
	}
	MakeSun();
	sunMoonFactor=( NIGHT==PartOfDay() ) ?
		MOON_LIGHT_FACTOR : SUN_LIGHT_FACTOR;
	ReEnlightenAll();
}

void World::DeleteAllShreds() {
	RemSun();
	for (ushort i=0; i<NumShreds()*NumShreds(); ++i) {
		shreds[i]->~Shred();
	}
	operator delete(shredMemoryPool);
	delete [] shreds;
}

void World::SetNumActiveShreds(const ushort num) {
	const QWriteLocker writeLocker(rwLock);
	numActiveShreds = num;
	if ( 1 != numActiveShreds%2 ) {
		++numActiveShreds;
		emit Notify(tr("Invalid active shreds number. Set to %1.").
			arg(numActiveShreds));
	}
	if ( numActiveShreds < 3 ) {
		numActiveShreds = 3;
		emit Notify(tr("Active shreds number too small, set to 3."));
	} else if ( numActiveShreds > numShreds ) {
		numActiveShreds=numShreds;
		emit Notify(tr("Active shreds number too big. Set to %1.").
			arg(numActiveShreds));
	}
	emit Notify(tr("Active shreds number is %1x%1.").arg(numActiveShreds));
}

World::World(const QString & world_name) :
		timeStep(0),
		worldName(world_name),
		rwLock(new QReadWriteLock()),
		map(new WorldMap(&world_name)),
		toResetDir(UP)
{
	puts(qPrintable(tr("Loading world settings...")));
	world = this;
	QSettings game_settings("freg.ini", QSettings::IniFormat);
	numShreds = game_settings.value("number_of_shreds", MIN_WORLD_SIZE).
		toLongLong();
	if ( 1 != numShreds%2 ) {
		++numShreds;
		fprintf(stderr, "Invalid number of shreds. Set to %hu.\n",
			numShreds);
	}
	if ( numShreds < MIN_WORLD_SIZE ) {
		fprintf(stderr,
			"Number of shreds: to small: %hu. Set to %hu.\n",
			numShreds, MIN_WORLD_SIZE);
		numShreds = MIN_WORLD_SIZE;
	}
	SetNumActiveShreds(game_settings.value("number_of_active_shreds",
		numShreds).toUInt());
	game_settings.setValue("number_of_shreds", numShreds);
	game_settings.setValue("number_of_active_shreds", numActiveShreds);

	QDir::current().mkpath(worldName+"/texts");
	QSettings settings(worldName+"/settings.ini", QSettings::IniFormat);
	time = settings.value("time", END_OF_NIGHT).toLongLong();
	spawnLongi = settings.value( "spawn_longitude",
		int(qrand() % MapSize()) ).toLongLong();
	spawnLati  = settings.value( "spawn_latitude",
		int(qrand() % MapSize()) ).toLongLong();
	settings.setValue("spawn_longitude", qlonglong(spawnLongi));
	settings.setValue("spawn_latitude", qlonglong(spawnLati));
	longitude = settings.value("longitude", int(spawnLongi)).toLongLong();
	latitude  = settings.value("latitude",  int(spawnLati )).toLongLong();

	shredStorage = new ShredStorage(numShreds+2, longitude, latitude);
	puts(qPrintable(tr("Loading world...")));
	LoadAllShreds();
	emit UpdatedAll();
} // World::World(const QString & world_name)

World::~World() { CleanAll(); }

void World::CleanAll() {
	static bool cleaned = false;
	if ( cleaned ) {
		return;
	}
	cleaned = true;

	WriteLock();
	quit();
	wait();
	Unlock();

	DeleteAllShreds();
	delete map;
	delete shredStorage;
	delete rwLock;

	QSettings settings(worldName+"/settings.ini", QSettings::IniFormat);
	settings.setValue("time", qlonglong(time));
	settings.setValue("longitude", qlonglong(longitude));
	settings.setValue("latitude", qlonglong(latitude));
}
