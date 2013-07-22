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
#include <QDir>
#include "blocks.h"
#include "Shred.h"
#include "world.h"
#include "BlockManager.h"
#include <qmath.h>
#include <DeferredAction.h>

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
ushort World::TimeStepsInSec() { return TIME_STEPS_IN_SEC; }

long World::MapSize() const { return mapSize; }
QTextStream * World::MapStream() { return worldMapStream; }

ushort World::SunMoonX() const {
	return ( NIGHT==PartOfDay() ) ?
		TimeOfDay()*SHRED_WIDTH*numShreds/
			SECONDS_IN_NIGHT :
		(TimeOfDay()-SECONDS_IN_NIGHT)*SHRED_WIDTH*numShreds/
			SECONDS_IN_DAYLIGHT;
}

times_of_day World::PartOfDay() const {
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

int World::TimeOfDay() const { return time % SECONDS_IN_DAY; }
ulong World::Time() const { return time; }
ushort World::MiniTime() const { return timeStep; }

void World::Drop(Block * const block_from,
		const ushort x_to, const ushort y_to, const ushort z_to,
		const ushort src, const ushort dest, const ushort num)
{
	Block * block_to=GetBlock(x_to, y_to, z_to);
	if ( AIR==block_to->Sub() ) {
		SetBlock((block_to=NewBlock(PILE, DIFFERENT)),
			x_to, y_to, z_to);
	}
	Exchange(block_from, block_to, src, dest, num);
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

bool World::InBounds(const ushort i, const ushort j, const ushort k) const {
	return ( i<maxXY && j<maxXY && k<HEIGHT );
}

void World::ReloadAllShreds(const long lati, const long longi,
	const ushort new_x, const ushort new_y, const ushort new_z,
	const ushort new_num_shreds)
{
	newLati=lati;
	newLongi=longi;
	newX=new_x;
	newY=new_y;
	newZ=new_z;
	if ( numActiveShreds > new_num_shreds ) {
		numActiveShreds=new_num_shreds;
	}
	newNumShreds=new_num_shreds;
	maxXY=SHRED_WIDTH*numShreds;
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
	timer.start(1000/TIME_STEPS_IN_SEC);
	exec();
}

quint8 World::TurnRight(const quint8 dir) {
	switch ( dir ) {
		default:
			fprintf(stderr,
				"World::TurnRight:Unlisted dir: %d\n",
				(int)dir);
		//no break;
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
		//no break;
		case EAST:  return NORTH;
		case NORTH: return WEST;
		case WEST:  return SOUTH;
		case SOUTH: return EAST;
		case UP:
		case DOWN:  return dir;
	}
}

void World::MakeSun() {
	ifStar=( STAR==Sub( (sun_moon_x=SunMoonX()), SHRED_WIDTH*numShreds/2,
		HEIGHT-1) );
	PutNormalBlock(SUN_MOON,
		sun_moon_x, SHRED_WIDTH*numShreds/2, HEIGHT-1);
}

Block * World::GetBlock(const ushort x, const ushort y, const ushort z)
const {
	return GetShred(x, y)->GetBlock(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::SetBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->SetBlock(block, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::PutBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	GetShred(x, y)->PutBlock(block, x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

void World::PutNormalBlock(const subs sub,
		const ushort x, const ushort y, const ushort z)
{
	PutBlock(Normal(sub), x, y, z);
}

Block * World::Normal(const int sub) {
	return block_manager.NormalBlock(sub);
}
Block * World::NewBlock(const int kind, const int sub) {
	return block_manager.NewBlock(kind, sub);
}
void World::DeleteBlock(Block * const block) {
	block_manager.DeleteBlock(block);
}

void World::AddDeferredAction(DeferredAction * const action) {
	defActions.append(action);
}

void World::RemDeferredAction(DeferredAction * const action) {
	defActions.removeOne(action);
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
		case NORTH_EAST: return SOUTH_WEST;
		case NORTH_WEST: return SOUTH_EAST;
		case SOUTH_EAST: return NORTH_WEST;
		case SOUTH_WEST: return NORTH_EAST;
		case UP:   return DOWN;
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
} //World::ReloadShreds

void World::PhysEvents() {
	WriteLock();

	for (int i=0; i<defActions.size(); ++i) {
		defActions.at(i)->MakeAction();
		RemDeferredAction(defActions.at(i));
	}

	if ( toReSet ) {
		emit StartReloadAll();
		DeleteAllShreds();
		longitude=newLongi;
		latitude=newLati;
		numShreds=newNumShreds;
		toReSet=false;
		LoadAllShreds();
		emit NeedPlayer(newX, newY, newZ);
		emit UpdatedAll();
		emit FinishReloadAll();
	}
	/*static ulong global_step=0;
	fprintf(stderr, "step: %lu\n", global_step);
	++global_step;
	emit Notify(QString("tic-tac: %1").arg(time));*/

	const ushort start=numShreds/2-numActiveShreds/2;
	const ushort end=start+numActiveShreds;
	for (ushort i=start; i<end; ++i)
	for (ushort j=start; j<end; ++j) {
		shreds[i+j*NumShreds()]->PhysEventsFrequent();
	}

	if ( TIME_STEPS_IN_SEC > timeStep ) {
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
		PutNormalBlock((ifStar ? STAR : SKY), sun_moon_x, y, HEIGHT-1);
		emit Updated(sun_moon_x, y, HEIGHT-1);
		sun_moon_x=SunMoonX();
		ifStar=( STAR==Sub(sun_moon_x, y, HEIGHT-1) );
		PutNormalBlock(SUN_MOON, sun_moon_x, y, HEIGHT-1);
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
	}
	emit UpdatesEnded();
	//emit ExitReceived(); //close all after 1 turn
	Unlock();
} //World::PhysEvents

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
	//this function is like World::PositiveVisible,
	//except those 4 lines:
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

bool World::Move(const ushort i, const ushort j, const ushort k,
		const quint8 dir)
{
	ushort newi, newj, newk;
	if ( !Focus(i, j, k, newi, newj, newk, dir) &&
			CanMove(i, j, k, newi, newj, newk, dir) &&
			(DOWN==dir || !GetBlock(i, j, k)->Weight() || !(
				AIR==Sub(i, j, k-1) &&
				AIR==Sub(newi, newj, newk-1))) )
	{
		NoCheckMove(i, j, k, newi, newj, newk, dir);
		return true;
	} else {
		return false;
	}
}

bool World::CanMove(
		const ushort i,    const ushort j,    const ushort k,
		const ushort newi, const ushort newj, const ushort newk,
		const quint8 dir)
{
	Block * const block=GetBlock(i, j, k);
	if ( NOT_MOVABLE==block->Movable() ) {
		return false;
	}
	Block * block_to=GetBlock(newi, newj, newk);
	if ( ENVIRONMENT==block->Movable() && *block==*block_to ) {
		return false;
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
				return false;
			}
		break;
		case MOVE_UP:
			if ( DOWN!=dir ) {
				Move(i, j, k, UP);
				return false;
			}
		break;
		case DAMAGE:
			block->ReceiveSignal(tr("Ouch!"));
			Damage(i, j, k,
				block_to->DamageLevel(),
				block_to->DamageKind());
			return false;
		break;
	}
	return (( ENVIRONMENT==block_to->Movable() ) ||
		Move(newi, newj, newk, dir));
} //World::CanMove

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

	Shred * shred=GetShred(i, j);
	shred->AddFalling(i%SHRED_WIDTH, j%SHRED_WIDTH, k+1);
	shred->AddFalling(i%SHRED_WIDTH, j%SHRED_WIDTH, k);
	shred=GetShred(newi, newj);
	shred->AddFalling(newi%SHRED_WIDTH, newj%SHRED_WIDTH, newk+1);
	shred->AddFalling(newi%SHRED_WIDTH, newj%SHRED_WIDTH, newk);

	block_to->Move( Anti(dir) );
	block->Move(dir);
}

void World::Jump(const ushort x, const ushort y, const ushort z,
		const quint8 dir)
{
	if ( !(AIR==Sub(x, y, z-1) && GetBlock(x, y, z)->Weight()) &&
			Move(x, y, z, UP) &&
			!Move(x, y, z+1, dir) )
	{
		NoCheckMove(x, y, z+1, x, y, z, DOWN);
		Move(x, y, z, dir);
	}
}

bool World::Focus(const ushort i, const ushort j, const ushort k,
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
			return true;
	}
	return !InBounds(i_target, j_target, k_target);
}

bool World::Focus(const ushort i, const ushort j, const ushort k,
		ushort & i_target, ushort & j_target, ushort & k_target)
const {
	return Focus( i, j, k, i_target, j_target, k_target,
		GetBlock(i, j, k)->GetDir() );
}

void World::Damage(const ushort i, const ushort j, const ushort k,
		const ushort dmg, const int dmg_kind)
{
	Block * temp=GetBlock(i, j, k);
	if ( temp==Normal(temp->Sub()) && AIR!=temp->Sub() ) {
		SetBlock((temp=NewBlock(temp->Kind(), temp->Sub())), i, j, k );
	}
	if ( temp->Damage(dmg, dmg_kind) > 0 ) {
		if ( STONE==temp->Sub() && BLOCK==temp->Kind() &&
				temp->Durability()!=MAX_DURABILITY )
		{
			DeleteBlock(temp);
			temp=NewBlock(LADDER, STONE);
			emit ReEnlighten(i, j, k);
		} else {
			temp=ReplaceWithNormal(temp); //checks are inside
		}
		SetBlock(temp, i, j, k);
	}
}

void World::DestroyAndReplace(const ushort x, const ushort y, const ushort z) {
	Block * const temp=GetBlock(x, y, z);
	if ( temp->Durability() > 0 ) {
		return;
	}
	Block * const dropped=temp->DropAfterDamage();
	if ( PILE!=temp->Kind() && (temp->HasInventory() || dropped) ) {
		Block * const new_pile=( ( dropped && PILE==dropped->Kind() ) ?
			dropped : NewBlock(PILE, DIFFERENT) );
		SetBlock(new_pile, x, y, z);
		Inventory * const inv=temp->HasInventory();
		Inventory * const new_pile_inv=new_pile->HasInventory();
		if ( inv ) {
			new_pile_inv->GetAll(inv);
		}
		if ( dropped &&
				PILE!=dropped->Kind() &&
				!new_pile_inv->Get(dropped) )
		{
			DeleteBlock(dropped);
		}
	} else {
		PutNormalBlock(AIR, x, y, z);
	}
	DeleteBlock(temp);
	GetShred(x, y)->AddFalling(x%SHRED_WIDTH, y%SHRED_WIDTH, z+1);
	ReEnlighten(x, y, z);
}

bool World::Build(Block * block,
		const ushort i, const ushort j, const ushort k,
		const quint8 dir,
		Block * const who,
		const bool anyway)
{
	Block * const target_block=GetBlock(i, j, k);
	if ( !(ENVIRONMENT==target_block->Movable() || anyway) ) {
		if ( who ) {
			who->ReceiveSignal(tr("Cannot build here."));
		}
		return false;
	}
	DeleteBlock(target_block);
	block->Restore();
	block->SetDir(dir);
	block=ReplaceWithNormal(block);
	SetBlock(block, i, j, k);
	ReEnlighten(i, j, k);
	return true;
}

void World::Inscribe(const ushort x, const ushort y, const ushort z) {
	Block * block=GetBlock(x, y, z);
	if ( LIQUID==block->Kind() || AIR==block->Sub() ) {
		return;
	}
	if ( block==Normal(block->Sub()) ) {
		SetBlock(block=NewBlock(block->Kind(), block->Sub()), x, y, z);
	}
	QString str=tr("No note received.");
	emit GetString(str);
	block->Inscribe(str);
	SetBlock(ReplaceWithNormal(block), x, y, z);
}

void World::Eat(
		const ushort i,      const ushort j,      const ushort k,
		const ushort i_food, const ushort j_food, const ushort k_food)
{
	if ( GetBlock(i, j, k)->IsAnimal()->Eat(Sub(i_food, j_food, k_food)) )
	{
		Damage(i_food, j_food, k_food, MAX_DURABILITY, EATEN);
		DestroyAndReplace(i_food, j_food, k_food);
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
	return GetShred(x, y)->Transparent(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}
int World::Sub(const ushort x, const ushort y, const ushort z) const {
	return GetShred(x, y)->Sub(x%SHRED_WIDTH, y%SHRED_WIDTH, z);
}

int World::Temperature(
		const ushort i_center,
		const ushort j_center,
		const ushort k_center)
const {
	if ( HEIGHT-1==k_center ) {
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

void World::RemSun() {
	SetBlock(
		Normal(ifStar ? STAR : SKY),
		sun_moon_x,
		SHRED_WIDTH*numShreds/2,
		HEIGHT-1);
}

void World::LoadAllShreds() {
	shreds=new Shred *[(ulong)numShreds*(ulong)numShreds];
	for (long i=latitude -numShreds/2, x=0; x<numShreds; ++i, ++x)
	for (long j=longitude-numShreds/2, y=0; y<numShreds; ++j, ++y) {
		shreds[y*numShreds+x]=new Shred(this, x, y, j, i);
	}
	MakeSun();
	sunMoonFactor=( NIGHT==PartOfDay() ) ?
		MOON_LIGHT_FACTOR : SUN_LIGHT_FACTOR;
	ReEnlightenAll();
}

void World::DeleteAllShreds() {
	RemSun();
	for (ushort i=0; i<numShreds*numShreds; ++i) {
		delete shreds[i];
	}
	delete [] shreds;
}

void World::SetNumActiveShreds(ushort num) {
	WriteLock();
	if ( 1 != num%2 ) {
		emit Notify(tr("Invalid shreds number:%1x%1.").arg(num));
		++num;
	}
	if ( num < 3 ) {
		emit Notify(tr("Active shreds number too small: %1x%1.").
			arg(num));
	} else if ( num > numShreds ) {
		emit Notify(tr("Active shreds number too big: %1x%1.").
			arg(num));
	} else {
		numActiveShreds=num;
	}
	emit Notify(tr("Active shreds number is %1x%1.").
		arg(numActiveShreds));
	Unlock();
}

World::World(const QString & world_name) :
		timeStep(0),
		worldName(world_name),
		cleaned(false),
		toReSet(false),
		settings(QDir::currentPath()+'/'+worldName+"/settings.ini",
			QSettings::IniFormat),
		game_settings(QDir::currentPath()+"/freg.ini",
			QSettings::IniFormat)
{
	numShreds =
		game_settings.value("number_of_shreds", 5).toLongLong();
	maxXY=SHRED_WIDTH*numShreds;
	numActiveShreds=
		game_settings.value("number_of_active_shreds", 5).toLongLong();

	QDir::current().mkdir(worldName);
	QFile map(worldName+"/map.txt");
	if ( map.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		mapSize=int(qSqrt(1+4*map.size())-1)/2;
	} else {
		const ushort max_command_length=50;
		char command[max_command_length];
		const ushort map_size=75;
		#ifdef Q_OS_LINUX
			snprintf(command,
				max_command_length,
				"./mapgen -s %hu -r %d -f %s",
				map_size,
				qrand(),
				qPrintable(worldName+"/map.txt"));
		#endif
		#ifdef Q_OS_WIN32
			snprintf(command,
				max_command_length,
				"mapgen.exe -s %hu -r %d -f %s",
				map_size,
				qrand(),
				qPrintable(worldName+"/map.txt"));
		#endif
		system(command);
		if ( map.open(QIODevice::ReadOnly | QIODevice::Text) ) {
			mapSize=map_size;
		} else {
			mapSize=1;
			worldMap=".";
		}
	}
	worldMap=map.readAll();
	worldMapStream=new QTextStream(worldMap, QIODevice::ReadOnly);

	time=settings.value("time", END_OF_NIGHT).toLongLong();
	spawnLongi=settings.value("spawn_longitude",
		int(qrand()%mapSize)).toLongLong();
	spawnLati =settings.value("spawn_latitude",
		int(qrand()%mapSize)).toLongLong();
	longitude =settings.value("longitude", int(spawnLongi)).toLongLong();
	latitude  =settings.value("latitude",  int(spawnLati )).toLongLong();

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
	emit UpdatedAll();
} //World::World

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

	DeleteAllShreds();

	settings.setValue("time", qlonglong(time));
	settings.setValue("longitude", qlonglong(longitude));
	settings.setValue("latitude", qlonglong(latitude));
	settings.setValue("spawn_longitude", qlonglong(spawnLongi));
	settings.setValue("spawn_latitude", qlonglong(spawnLati));
	game_settings.setValue("number_of_shreds", numShreds);
	game_settings.setValue("number_of_active_shreds", numActiveShreds);
}
