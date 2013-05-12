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

#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QSettings>
#include <QDir>
#include "header.h"
#include "blocks.h"
#include "Shred.h"
#include "world.h"
#include "BlockManager.h"
#include <qmath.h>

#ifdef Q_OS_WIN32
    int round(float r) { return qFloor(r+0.5); }
#else
    #include <cmath>
#endif

void World::ReplaceWithNormal(const ushort x, const ushort y, const ushort z)
{
	SetBlock(ReplaceWithNormal(GetBlock(x, y, z)), x, y, z);
}

Shred * World::GetShred(const ushort i, const ushort j) const {
	return shreds[ j/SHRED_WIDTH*numShreds + i/SHRED_WIDTH ];
}

QString World::WorldName() const { return worldName; }

ushort World::NumShreds() const { return numShreds; }
ushort World::NumActiveShreds() const { return numActiveShreds; }

long World::GetSpawnLongi() const { return spawnLongi; }
long World::GetSpawnLati()  const { return spawnLati; }
long World::Longitude() const { return longitude; }
long World::Latitude() const { return latitude; }
ushort World::TimeStepsInSec() { return time_steps_in_sec; }

long World::MapSize() const { return mapSize; }

ushort World::SunMoonX() const {
	return ( NIGHT==PartOfDay() ) ?
		TimeOfDay()*SHRED_WIDTH*numShreds/
			SECONDS_IN_NIGHT :
		(TimeOfDay()-SECONDS_IN_NIGHT)*SHRED_WIDTH*numShreds/
			SECONDS_IN_DAYLIGHT;
}

float World::Distance(
		const ushort x_from, const ushort y_from, const ushort z_from,
		const ushort x_to,   const ushort y_to,   const ushort z_to)
const {
	return sqrt( float((x_from-x_to)*(x_from-x_to)+
			   (y_from-y_to)*(y_from-y_to)+
			   (z_from-z_to)*(z_from-z_to)) );
}

times_of_day World::PartOfDay() const {
	ushort time_day=TimeOfDay();
	if (time_day<END_OF_NIGHT)   return NIGHT;
	if (time_day<END_OF_MORNING) return MORNING;
	if (time_day<END_OF_NOON)    return NOON;
	return EVENING;
}

int World::TimeOfDay() const { return time % SECONDS_IN_DAY; }
ulong World::Time() const { return time; }
ushort World::MiniTime() const { return timeStep; }

int World::Drop(
		const ushort i, const ushort j,	const ushort k,
		const ushort n)
{
	ushort i_to, j_to, k_to;
	return ( Focus(i, j, k, i_to, j_to, k_to) ) ?
		5 : Exchange(i, j, k, i_to, j_to, k_to, n);
}

int World::Get(
		const ushort i, const ushort j, const ushort k,
		const ushort n)
{
	ushort i_from, j_from, k_from;
	return ( Focus(i, j, k, i_from, j_from, k_from) ) ? 5 :
		Exchange(i_from, j_from, k_from, i, j, k, n);
}

bool World::InBounds(const ushort i, const ushort j, const ushort k) const {
	const ushort max_x_y=SHRED_WIDTH*numShreds;
	return (i<max_x_y && j<max_x_y && k<HEIGHT);
}

void World::ReloadAllShreds(
	const long lati, const long longi,
	const ushort new_x, const ushort new_y, const ushort new_z,
	const ushort new_num_shreds)
{
	newLati=lati;
	newLongi=longi;
	newX=new_x;
	newY=new_y;
	newZ=new_z;
	if ( numActiveShreds > new_num_shreds )
		numActiveShreds=new_num_shreds;
	newNumShreds=new_num_shreds;
	toReSet=true;
}

void World::WriteLock() { rwLock.lockForWrite(); }
void World::ReadLock() { rwLock.lockForRead(); }
bool World::TryReadLock() { return rwLock.tryLockForRead(); }
void World::Unlock() { rwLock.unlock(); }

void World::EmitNotify(const QString & str) const { emit Notify(str); }

void World::run() {
	QTimer timer;
	connect(&timer, SIGNAL(timeout()),
		this, SLOT(PhysEvents()),
		Qt::DirectConnection);
	timer.start(1000/time_steps_in_sec);
	exec();
}

quint8 World::TurnRight(const quint8 dir) {
	switch (dir) {
		case NORTH: return EAST;
		case EAST: return SOUTH;
		case SOUTH: return WEST;
		case WEST: return NORTH;
		case UP: case DOWN:
			return dir;
		default:
			fprintf(stderr,
				"World::TurnRight:Unlisted dir: %d\n",
				(int)dir);
			return NORTH;
	}
}
quint8 World::TurnLeft(const quint8 dir) {
	switch (dir) {
		case NORTH: return WEST;
		case WEST: return SOUTH;
		case SOUTH: return EAST;
		case EAST: return NORTH;
		case UP: case DOWN:
			return dir;
		default:
			fprintf(stderr, "TurnLeft:Unlisted dir: %d\n",
				(int)dir);
			return NORTH;
	}
}
quint8 World::MakeDir(
		const ushort x_center, const ushort y_center,
		const ushort x_target, const ushort y_target) const
{
	//if (x_center==x_target && y_center==y_target) return HERE;
	if ( abs(x_center-x_target)<=1 && abs(y_center-y_target)<=1 )
		return HERE;
	const float x=x_target-x_center;
	const float y=y_target-y_center;
	if (      y <= 3*x  && y <= -3*x ) return NORTH;
	else if ( y > -3*x  && y < -x/3 )  return NORTH_EAST;
	else if ( y >= -x/3 && y <= x/3 )  return EAST;
	else if ( y > x/3   && y <3 *x )   return SOUTH_EAST;
	else if ( y >= 3*x  && y >= -3*x ) return SOUTH;
	else if ( y <- 3*x  && y >- x/3 )  return SOUTH_WEST;
	else if ( y <=- x/3 && y >= x/3 )  return WEST;
	else return NORTH_WEST;
}

void World::MakeSun() {
	sun_moon_x=SunMoonX();
	ifStar=( STAR==Sub(sun_moon_x, SHRED_WIDTH*numShreds/2, HEIGHT-1) );
	PutNormalBlock(SUN_MOON,
		sun_moon_x, SHRED_WIDTH*numShreds/2, HEIGHT-1);
}

Block * World::GetBlock(const ushort x, const ushort y, const ushort z)
const {
	return GetShred(x, y)->GetBlock(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::SetBlock(
		Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->SetBlock(block, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::PutBlock(
		Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->PutBlock(block, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::PutNormalBlock(
		const subs sub,
		const ushort x, const ushort y, const ushort z)
{
	PutBlock(Normal(sub), x, y, z);
}

Block * World::Normal(const int sub) { return block_manager.NormalBlock(sub); }
void World::DeleteBlock(Block * const block) {
	block_manager.DeleteBlock(block);
}

Block * World::ReplaceWithNormal(Block * const block) {
	if ( block!=Normal(block->Sub()) && *block==*Normal(block->Sub()) ) {
		block_manager.DeleteBlock(block);
		return Normal(block->Sub());
	} else {
		return block;
	}
}

quint8 World::Anti(const quint8 dir) {
	switch (dir) {
		case NORTH: return SOUTH;
		case NORTH_EAST: return SOUTH_WEST;
		case EAST: return WEST;
		case SOUTH_EAST: return NORTH_WEST;
		case SOUTH: return NORTH;
		case SOUTH_WEST: return NORTH_EAST;
		case WEST: return EAST;
		case NORTH_WEST: return SOUTH_EAST;
		case UP: return DOWN;
		case DOWN: return UP;
		default:
			fprintf(stderr,
				"World::Anti(int): unlisted dir: %d\n",
				(int)dir);
			return HERE;
	}
}

void World::ReloadShreds(const int direction) {
	short x, y; //do not make unsigned, values <0 are needed for checks
	RemSun();
	switch ( direction ) {
		case NORTH:
			--longitude;
			for (x=0; x<numShreds; ++x) {
				delete shreds[numShreds*(numShreds-1)+x];
				for (y=numShreds-2; y>=0; --y) {
					(shreds[(y+1)*numShreds+x]=
						shreds[y*numShreds+x])->
						ReloadToNorth();
				}
				shreds[x]=new Shred(this, x, 0,
						longitude-numShreds/2,
						latitude-numShreds/2+x);
			}
		break;
		case SOUTH:
			++longitude;
			for (x=0; x<numShreds; ++x) {
				delete shreds[x];
				for (y=1; y<numShreds; ++y) {
					(shreds[(y-1)*numShreds+x]=
						shreds[y*numShreds+x])->
						ReloadToSouth();
				}
				shreds[numShreds*(numShreds-1)+x]=new
					Shred(this, x, numShreds-1,
						longitude+numShreds/2,
						latitude-numShreds/2+x);

			}
		break;
		case EAST:
			++latitude;
			for (y=0; y<numShreds; ++y) {
				delete shreds[y*numShreds];
				for (x=1; x<numShreds; ++x) {
					(shreds[(x-1)+y*numShreds]=
						shreds[x+y*numShreds])->
						ReloadToEast();
				}
				shreds[numShreds-1+y*numShreds]=new
					Shred(this, numShreds-1, y,
						longitude-numShreds/2+y,
						latitude+numShreds/2);

			}
		break;
		case WEST:
			--latitude;
			for (y=0; y<numShreds; ++y) {
				delete shreds[numShreds-1+y*numShreds];
				for (x=numShreds-2; x>=0; --x) {
					(shreds[(x+1)+y*numShreds]=
						shreds[x+y*numShreds])->
						ReloadToWest();
				}
				shreds[y*numShreds]=new Shred(this, 0, y,
						longitude-numShreds/2+y,
						latitude-numShreds/2);

			}
		break;
		default: fprintf(stderr,
			"World::ReloadShreds(int): invalid direction: %d\n",
			direction);
	}
	MakeSun();
	ReEnlightenMove(direction);
	emit Moved(direction);
}

void World::PhysEvents() {
	WriteLock();

	if ( DEFERRED_NOTHING!=deferredActionType ) {
		switch ( deferredActionType ) {
			case DEFERRED_MOVE:
				Move(
					deferredActionX,
					deferredActionY,
					deferredActionZ,
					deferredActionDir);
			break;
			case DEFERRED_JUMP:
				Jump(
					deferredActionX,
					deferredActionY,
					deferredActionZ,
					deferredActionDir);
			break;
			case DEFERRED_BUILD: {
				if ( DOWN==deferredActionDir &&
						AIR!=Sub(
							deferredActionX,
							deferredActionY,
							deferredActionZ) )
				{
					if ( !Move(
							deferredActionXFrom,
							deferredActionYFrom,
							deferredActionZFrom,
							UP) )
					{
						break;
					} else {
						deferredActionZ += 1;
						deferredActionZFrom += 1;
					}
				}

				const int build=Build(
					deferredActionWhat,
					deferredActionX,
					deferredActionY,
					deferredActionZ,
					World::TurnRight(deferredActionDir),
					GetBlock(
						deferredActionXFrom,
						deferredActionYFrom,
						deferredActionZFrom));
				if ( !build /*no errors*/ ) {
					Inventory * const inv=GetBlock(
							deferredActionXFrom,
							deferredActionYFrom,
							deferredActionZFrom)->
								HasInventory();
					if ( inv )
						inv->Pull(deferredActionData1);
				}
			} break;
			case DEFERRED_DAMAGE:
				if ( InBounds(
							deferredActionX,
							deferredActionY,
							deferredActionZ) &&
						Visible(
							deferredActionXFrom,
							deferredActionYFrom,
							deferredActionZFrom,
							deferredActionX,
							deferredActionY,
							deferredActionZ) )
				{
					Damage(
						deferredActionX,
						deferredActionY,
						deferredActionZ,
						deferredActionData1,
						deferredActionData2);
				}
			break;
			case DEFERRED_THROW:
				if ( !Drop(
						deferredActionXFrom,
						deferredActionYFrom,
						deferredActionZFrom,
						deferredActionData1) )
				{
					//send message to dropper
				}
			break;
			default:
				fprintf(stderr,
					"World::PhysEvents: \
					unlisted deferred_action: %d\n",
					deferredActionType);
		}
		deferredActionType=DEFERRED_NOTHING;
	}
	if ( toReSet ) {
		emit StartReloadAll();
		SaveAllShreds();
		longitude=newLongi;
		latitude=newLati;
		numShreds=newNumShreds;
		toReSet=false;
		LoadAllShreds();
		emit NeedPlayer(newX, newY, newZ);
		emit FinishReloadAll();
	}
	/*static ulong global_step=0;
	fprintf(stderr, "step: %lu\n", global_step);
	++global_step;*/

	const ushort start=numShreds/2-numActiveShreds/2;
	const ushort end=start+numActiveShreds;
	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->PhysEventsFrequent();
	}

	if ( time_steps_in_sec > timeStep ) {
		++timeStep;
		Unlock();
		return;
	}
	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->PhysEventsRare();
	}
	timeStep=0;
	++time;

	//sun/moon moving
	if ( sun_moon_x!=SunMoonX() ) {
		const ushort y=SHRED_WIDTH*numShreds/2;
		PutBlock(Normal(ifStar ? STAR : SKY),
				sun_moon_x, y, HEIGHT-1);
		emit Updated(sun_moon_x, y, HEIGHT-1);
		sun_moon_x=SunMoonX();
		ifStar=( STAR==Sub(sun_moon_x, y, HEIGHT-1) );
		PutBlock(Normal(SUN_MOON),
				sun_moon_x, y, HEIGHT-1);
		emit Updated(sun_moon_x, y, HEIGHT-1);
	}

	switch ( TimeOfDay() ) {
		case END_OF_NIGHT:
			ReEnlightenTime();
			emit Notify(tr("It's morning now."));
		break;
		case END_OF_MORNING:
			emit Notify(tr("It's day now."));
		break;
		case END_OF_NOON:
			emit Notify(tr("It's evening now."));
		break;
		case END_OF_EVENING:
			ReEnlightenTime();
			emit Notify(tr("It's night now."));
		break;
		default: break;
	}
	emit UpdatesEnded();
	//emit ExitReceived(); //close all after 1 turn
	Unlock();
}

bool World::DirectlyVisible(
		float x_from, float y_from, float z_from,
		const ushort x_to, const ushort y_to, const ushort z_to)
const {
	if ( x_from==x_to && y_from==y_to && z_from==z_to ) {
		return true;
	}
	const ushort xdif=abs(x_to-(ushort)x_from);
	const ushort ydif=abs(y_to-(ushort)y_from);
	const ushort zdif=abs(z_to-(ushort)z_from);
	ushort max=(zdif > ydif) ?
		zdif :
		ydif;
	if ( xdif > max ) {
		max=xdif;
	}
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
	//TODO: make this symmetric
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

int World::Move(
		const ushort i, const ushort j, const ushort k,
		const quint8 dir)
{
	ushort newi, newj, newk;
	if ( !Focus(i, j, k, newi, newj, newk, dir) &&
			CanMove(i, j, k, newi, newj, newk, dir) )
	{
		NoCheckMove(i, j, k, newi, newj, newk, dir);
		return 1;
	} else {
		return 0;
	}
}

int World::CanMove(
		const ushort i,    const ushort j,    const ushort k,
		const ushort newi, const ushort newj, const ushort newk,
		const quint8 dir)
{
	Block * const block=GetBlock(i, j, k);
	if ( NOT_MOVABLE==block->Movable() ) {
		return 0;
	}
	Block * block_to=GetBlock(newi, newj, newk);
	if ( ENVIRONMENT==block->Movable() && *block==*block_to ) {
		return 0;
	}
	switch ( block_to->BeforePush(dir, block) ) {
		case DESTROY:
			DeleteBlock(block_to);
			//BeforePush could change block_to:
			PutBlock((block_to=Normal(AIR)), newi, newj, newk);
		break;
		case MOVE_SELF:
			block_to=GetBlock(newi, newj, newk);
		break;
		case JUMP:
			if ( DOWN!=dir && UP!=dir ) {
				Jump(i, j, k, dir);
				return 0;
			}
		break;
		case MOVE_UP:
			if ( DOWN!=dir ) {
				Move(i, j, k, UP);
				return 0;
			}
		break;
		case DAMAGE:
			block->ReceiveSignal(tr("Ouch!"));
			Damage(i, j, k,
				block_to->DamageLevel(),
				block_to->DamageKind());
			return 0;
		break;
		default: break;
	}

	return ( ENVIRONMENT==block_to->Movable() ) ?
		1 :
		Move(newi, newj, newk, dir);
}

void World::NoCheckMove(
		const ushort i,    const ushort j,    const ushort k,
		const ushort newi, const ushort newj, const ushort newk,
		const quint8 dir)
{
	Block * const block=GetBlock(i, j, k);
	Block * const block_to=GetBlock(newi, newj, newk);

	PutBlock(block_to, i, j, k);
	PutBlock(block, newi, newj, newk);

	ReEnlighten(newi, newj, newk);
	ReEnlighten(i, j, k);

	block_to->Move( Anti(dir) );
	block->Move(dir);

	Shred * shred=GetShred(i, j);
	shred->AddFalling(i%SHRED_WIDTH, j%SHRED_WIDTH, k+1);
	shred->AddFalling(i%SHRED_WIDTH, j%SHRED_WIDTH, k);
	shred=GetShred(newi, newj);
	shred->AddFalling(newi%SHRED_WIDTH, newj%SHRED_WIDTH, newk+1);
	shred->AddFalling(newi%SHRED_WIDTH, newj%SHRED_WIDTH, newk);
}

void World::Jump(const ushort x, const ushort y, const ushort z) {
	Jump(x, y, z, GetBlock(x, y, z)->GetDir());
}

void World::Jump(
		const ushort i, const ushort j, ushort k,
		const quint8 dir)
{
	Block * const to_move=GetBlock(i, j, k);
	if ( (AIR!=Sub(i, j, k-1) || !to_move->Weight()) ) {
		const short k_plus=Move(i, j, k, (DOWN==dir) ? DOWN : UP);
		if ( k_plus > 0 ) {
			k+=((DOWN==dir) ? (-1) : 1) * k_plus;
			if ( !Move( i, j, k, dir) ) {
				Move(i, j, k, DOWN);
			}
		}
	}
}

void World::SetDeferredAction(
		const ushort x,
		const ushort y,
		const ushort z,
		const quint8 dir,
		const int action,
		const ushort x_from, //see defaults in world.h
		const ushort y_from,
		const ushort z_from,
		Block * const what,
		const int data1,
		const int data2)
{
	deferredActionX=x;
	deferredActionY=y;
	deferredActionZ=z;
	deferredActionXFrom=x_from;
	deferredActionYFrom=y_from;
	deferredActionZFrom=z_from;
	deferredActionDir=dir;
	deferredActionType=action;
	deferredActionWhat=what;
	deferredActionData1=data1;
	deferredActionData2=data2;
}

int World::Focus(
		const ushort i, const ushort j, const ushort k,
		ushort & i_target, ushort & j_target, ushort & k_target,
		const quint8 dir)
const {
	i_target=i;
	j_target=j;
	k_target=k;
	switch ( dir ) {
		case NORTH: --j_target; break;
		case SOUTH: ++j_target; break;
		case EAST:  ++i_target; break;
		case WEST:  --i_target; break;
		case DOWN:  --k_target; break;
		case UP:    ++k_target; break;
		default:
			fprintf(stderr,
				"World::Focus: unlisted dir: %d\n",
				dir);
			return 2;
	}
	return !InBounds(i_target, j_target, k_target);
}

int World::Focus(
		const ushort i, const ushort j, const ushort k,
		ushort & i_target, ushort & j_target, ushort & k_target)
const {
	return Focus( i, j, k, i_target, j_target, k_target,
		GetBlock(i, j, k)->GetDir() );
}

///Makes damage to block at ijk.
/**
 * Be careful: it can destroy blocks.
 * It can create piles if block drops something or has non-empty inventory.
 * Returns true if block is destroyed, otherwise false.
 */
bool World::Damage(
		const ushort i, const ushort j, const ushort k,
		const ushort dmg, //see default in class declaration
		const int dmg_kind) //see default in class declaration
{
	if ( !InBounds(i, j, k) ) {
		return false;
	}
	Block * temp=GetBlock(i, j, k);
	if ( temp==Normal(temp->Sub()) && AIR!=temp->Sub() ) {
		SetBlock( (temp = block_manager.
			NewBlock(temp->Kind(), temp->Sub()) ), i, j, k );
	}
	if ( temp->Damage(dmg, dmg_kind) > 0 ) {
		ReplaceWithNormal(i, j, k); //checks are inside
		return false;
	}
	Block * const dropped=temp->DropAfterDamage();
	if ( PILE!=temp->Kind() && (temp->HasInventory() || dropped) ) {
		Block * const new_pile=block_manager.NewBlock(PILE, DIFFERENT);
		SetBlock(new_pile, i, j, k);
		Inventory * const inv=temp->HasInventory();
		Inventory * const new_pile_inv=new_pile->HasInventory();
		if ( inv ) {
			new_pile_inv->GetAll(inv);
		}
		if ( !new_pile_inv->Get(dropped) ) {
			DeleteBlock(dropped);
		}
	} else {
		PutNormalBlock(AIR, i, j, k);
	}
	block_manager.DeleteBlock(temp);
	GetShred(i, j)->AddFalling(i%SHRED_WIDTH, j%SHRED_WIDTH, k+1);
	ReEnlighten(i, j, k);
	return true;
}

int World::Use(const ushort i, const ushort j, const ushort k) {
	return ( !InBounds(i, j, k) ) ?
		NO :
		GetBlock(i, j, k)->Use();
}

int World::Build(
		Block * const block,
		const ushort i, const ushort j, const ushort k,
		const quint8 dir, Block * const who) //defaults exist
{
	if ( !InBounds(i, j, k) || AIR!=Sub(i, j, k)  ) {
		if ( who ) {
			who->ReceiveSignal(tr("Cannot build here."));
		}
		return 1;
	}
	if ( !block->CanBeOut() ) {
		if ( who ) {
			who->ReceiveSignal(tr("Cannot build this."));
		}
		return 2;
	}
	block->Restore();
	SetBlock(block, i, j, k);
	block->SetDir(dir);

	ReEnlighten(i, j, k);
	return 0;
}

void World::Inscribe(const ushort i, const ushort j, const ushort k) {
	if ( !InBounds(i, j, k) ) {
		return;
	}
	Block * block=GetBlock(i, j, k);
	if ( block==World::Normal(block->Sub()) )
		SetBlock(block=block_manager.
				NewBlock(block->Kind(), block->Sub()),
			i, j, k);
	QString str="No note received\n";
	emit GetString(str);
	block->Inscribe(str);
	ReplaceWithNormal(i, j, k);
}

void World::Eat(
		const ushort i,      const ushort j,      const ushort k,
		const ushort i_food, const ushort j_food, const ushort k_food)
{
	if ( !InBounds(i, j, k) || !InBounds(i_food, j_food, k_food) ) {
		return;
	}
	if ( GetBlock(i, j, k)->IsAnimal()->
			Eat(GetBlock(i_food, j_food, k_food)) )
	{
		Damage(i_food, j_food, k_food, MAX_DURABILITY, EATEN);
	}
}

int World::Exchange(
		const ushort i_from, const ushort j_from, const ushort k_from,
		const ushort i_to,   const ushort j_to,   const ushort k_to,
		const ushort num)
{
	Inventory * const inv_from=HasInventory(i_from, j_from, k_from);
	if ( !inv_from ) {
		GetBlock(i_from, j_from, k_from)->
			ReceiveSignal(tr("No inventory."));
		return 3;
	}
	if ( AIR==Sub(i_to, j_to, k_to) ) {
		SetBlock(block_manager.NewBlock(PILE, DIFFERENT),
			i_to, j_to, k_to);
	}
	Inventory * const inv_to=HasInventory(i_to, j_to, k_to);
	if ( !inv_to ) {
		GetBlock(i_from, j_from, k_from)->
			ReceiveSignal(tr("No room there."));
		return 4;
	}
	return inv_from->Drop(num, inv_to);
}

int World::GetAll(const ushort x_to, const ushort y_to, const ushort z_to) {
	ushort x_from, y_from, z_from;
	if ( Focus(x_to, y_to, z_to, x_from, y_from, z_from) ) {
		return 5;
	}
	Inventory * const inv_from=HasInventory(x_from, y_from, z_from);
	Inventory * const inv_to=HasInventory(x_to, y_to, z_to);
	if ( !inv_to ) {
		return 6;
	}
	return inv_to->GetAll(inv_from);
}

QString & World::FullName(QString & str,
		const ushort i, const ushort j, const ushort k)
const {
	if ( InBounds(i, j, k) ) {
		str=GetBlock(i, j, k)->FullName(str);
	}
	return str;
}

int World::Transparent(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Transparent(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
int World::Durability(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Durability(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
int World::Kind(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Kind(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
int World::Sub(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Sub(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
int World::Movable(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Movable(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
float World::Weight(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Weight(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

Inventory * World::HasInventory(const ushort i, const ushort j, const ushort k)
const {
	return ( InBounds(i, j, k) ) ?
		GetBlock(i, j, k)->HasInventory() :
		NULL;
}

Active * World::ActiveBlock(const ushort i, const ushort j, const ushort k)
const {
	return ( InBounds(i, j, k) ) ?
		GetBlock(i, j, k)->ActiveBlock() :
		0;
}

int World::Temperature(
		const ushort i_center,
		const ushort j_center,
		const ushort k_center)
const {
	if ( !InBounds(i_center, j_center, k_center) ||
			HEIGHT-1==k_center ) {
		return 0;
	}
	short temperature=GetBlock(i_center, j_center, k_center)->
		Temperature();
	if ( temperature ) {
		return temperature;
	}
	for (short i=i_center-1; i<=i_center+1; ++i)
	for (short j=j_center-1; j<=j_center+1; ++j)
	for (short k=k_center-1; k<=k_center+1; ++k) {
		if ( InBounds(i, j, k) ) {
			temperature+=GetBlock(i, j, k)->Temperature();
		}
	}
	return temperature/2;
}

QString & World::GetNote(QString & str,
		const ushort i, const ushort j, const ushort k)
const {
	return str=( InBounds(i, j, k)  ) ?
		GetBlock(i, j, k)->GetNote(str) :
		"";
}

void World::RemSun() {
	SetBlock(
		block_manager.NormalBlock(ifStar ? STAR : SKY),
		sun_moon_x,
		SHRED_WIDTH*numShreds/2,
		HEIGHT-1);
}

void World::LoadAllShreds() {
	shreds=new Shred *[numShreds*numShreds];
	for (long i=latitude -numShreds/2, x=0; x<numShreds; ++i, ++x)
	for (long j=longitude-numShreds/2, y=0; y<numShreds; ++j, ++y) {
		shreds[y*numShreds+x]=new Shred(this, x, y, j, i);
	}
	MakeSun();
	ReEnlightenTime();
}

void World::SaveAllShreds() {
	RemSun();
	for (ushort i=0; i<numShreds*numShreds; ++i) {
		delete shreds[i];
	}
	delete [] shreds;
}

void World::SetNumActiveShreds(ushort num) {
	WriteLock();
	if ( 1 != num%2 ) {
		emit Notify(QString(
			"Invalid shreds number:%1x%2.").arg(num).arg(num));
		++num;
	}
	if ( !num ) {
		emit Notify(QString(
			"Active shreds number too small: %1x%2.").
				arg(num).
				arg(num));
	} else if ( num > numShreds ) {
		emit Notify(QString(
			"Active shreds number too big: %1x%2.").
				arg(num).
				arg(num));
	} else {
		numActiveShreds=num;
	}
	emit Notify(QString(
		"Active shreds number is %1x%2.").
			arg(numActiveShreds).
			arg(numActiveShreds));
	Unlock();
}

World::World(const QString & world_name) :
		timeStep(0),
		worldName(world_name),
		cleaned(false),
		toReSet(false),
		deferredActionType(DEFERRED_NOTHING)
{
	QSettings settings(QDir::currentPath()+'/'+worldName+"/settings.ini",
		QSettings::IniFormat);
	time      =settings.value("time", END_OF_NIGHT).toLongLong();
	longitude =settings.value("longitude", 0).toLongLong();
	latitude  =settings.value("latitude", 0).toLongLong();
	spawnLongi=settings.value("spawn_longitude", 0).toLongLong();
	spawnLati =settings.value("spawn_latitude", 0).toLongLong();
	numShreds =settings.value("number_of_shreds", 5).toLongLong();
	numActiveShreds=
		settings.value("number_of_active_shreds", 5).toLongLong();

	QDir::current().mkdir(worldName);
	QFile map(worldName+"/map.txt");
	if ( map.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		mapSize=int(qSqrt(1+4*map.size())-1)/2;
	} else if ( map.open(QIODevice::WriteOnly |
			QIODevice::Text) )
	{
		char little_map[]=".\n";
		map.write(little_map);
		mapSize=1;
	}

	if ( 1!=numShreds%2 ) {
		++numShreds;
		fprintf(stderr,
			"Invalid number of shreds. Set to %hu.\n",
			numShreds);
	}
	if ( numShreds<5  ) {
		fprintf(stderr,
			"Number of shreds: to small: %hu. Set to 5.\n",
			numShreds);
		numShreds=5;
	}
	if ( 1!=numActiveShreds%2 ) {
		++numActiveShreds;
		fprintf(stderr,
			"Invalid number of active shreds. Set to %hu.\n",
			numActiveShreds);
	}
	if ( numActiveShreds > numShreds ) {
		fprintf(stderr,
			"numActiveShreds (%hu) was more than numShreds\n",
			numActiveShreds);
		numActiveShreds=numShreds;
	} else if ( numActiveShreds < 3 ) {
		fprintf(stderr,
			"Active shreds number (%hu) too small. Set to 3.\n",
			numActiveShreds);
		numActiveShreds=3;
	}

	LoadAllShreds();
}

World::~World() { CleanAll(); }

void World::CleanAll() {
	WriteLock();
	if ( cleaned ) {
		Unlock();
		return;
	}
	cleaned=true;

	quit();
	Unlock();
	wait();

	SaveAllShreds();

	QSettings sett(QDir::currentPath()+'/'+worldName+"/settings.ini",
		QSettings::IniFormat);
	sett.setValue("time", qlonglong(time));
	sett.setValue("longitude", qlonglong(longitude));
	sett.setValue("latitude", qlonglong(latitude));
	sett.setValue("spawn_longitude", qlonglong(spawnLongi));
	sett.setValue("spawn_latitude", qlonglong(spawnLati));
	sett.setValue("number_of_shreds", numShreds);
	sett.setValue("number_of_active_shreds", numActiveShreds);
}
