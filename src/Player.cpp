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
#include <QString>
#include <QSettings>
#include <QDir>
#include "blocks.h"
#include "Player.h"
#include "world.h"
#include "Shred.h"
#include "BlockManager.h"
#include "DeferredAction.h"

short Player::X() const { return x; }
short Player::Y() const { return y; }
short Player::Z() const { return z; }
long Player::GlobalX() const {
	return (GetShred()->Latitude() -x/SHRED_WIDTH)*SHRED_WIDTH+x;
}
long Player::GlobalY() const {
	return (GetShred()->Longitude()-y/SHRED_WIDTH)*SHRED_WIDTH+y;
}

bool Player::IsRightActiveHand() const {
	return Dwarf::IN_RIGHT==GetActiveHand();
}
ushort Player::GetActiveHand() const {
	return ( DWARF==GetP()->Kind() ) ?
		((Dwarf *)GetP())->GetActiveHand() : 0;
}
void Player::SetActiveHand(const bool right) {
	if ( DWARF==GetP()->Kind() ) {
		((Dwarf *)GetP())->SetActiveHand(right);
	}
}

Shred * Player::GetShred() const { return world->GetShred(x, y); }
World * Player::GetWorld() const { return world; }

bool Player::GetCreativeMode() const { return creativeMode; }
void Player::SetCreativeMode(const bool turn) {
	creativeMode=turn;
	disconnect(player, 0, 0, 0);
	Active * const prev_player=player;
	SetPlayer(x, y, z);
	player->SetDir(prev_player->GetDir());
	Inventory * const inv=PlayerInventory();
	if ( inv ) {
		inv->GetAll(prev_player->HasInventory());
	}
	if ( !creativeMode ) {
		block_manager.DeleteBlock(prev_player);
	}
	emit Updated();
}

int Player::UsingSelfType() const { return usingSelfType; }
int Player::UsingType() const { return usingType; }

Active * Player::GetP() const { return player; }

short Player::HP() const {
	return ( !player || creativeMode ) ?
		-1 :
		player ? player->Durability() : 0;
}

short Player::Breath() const {
	if ( !player || creativeMode ) {
		return -1;
	}
	Animal const * const animal=player->IsAnimal();
	return ( animal ? animal->Breath() : -1 );
}

short Player::Satiation() const {
	if ( !player || creativeMode ) {
		return -1;
	}
	Animal const * const animal=player->IsAnimal();
	return ( animal ? animal->Satiation() : -1 );
}
ushort Player::SatiationPercent() const {
	const short satiation=Satiation();
	return ( -1==satiation ) ?
		50 :
		satiation*100/SECONDS_IN_DAY;
}

Inventory * Player::PlayerInventory() {
	return ( player ? player->HasInventory() : 0 );
}

long Player::GetLongitude() const { return GetShred()->Longitude(); }
long Player::GetLatitude()  const { return GetShred()->Latitude();  }

void Player::UpdateXYZ() {
	if ( player ) {
		x=player->X();
		y=player->Y();
		z=player->Z();
	}
}

void Player::Focus(ushort & i_target, ushort & j_target, ushort & k_target)
const {
	world->Focus(x, y, z, i_target, j_target, k_target, GetDir());
}

void Player::Examine(const short i, const short j, const short k) const {
	world->ReadLock();

	emit Notify("------");
	const Block * const block=world->GetBlock(i, j, k);
	const int sub=block->Sub();
	emit Notify( block->FullName() );
	if ( creativeMode ) { //know more
		emit Notify(tr(
		"Light:%1, fire:%2, sun:%3. Transp:%4. Norm:%5. Dir:%6.").
			arg(world->Enlightened(i, j, k)).
			arg(world->FireLight(i, j, k)/16).
			arg(world->SunLight(i, j, k)).
			arg(block->Transparent()).
			arg(block==block_manager.NormalBlock(sub)).
			arg(block->GetDir()));
	}
	if ( AIR==sub || SKY==sub || SUN_MOON==sub ) {
		world->Unlock();
		return;	
	}
	QString str;
	if ( ""!=(str=block->GetNote()) ) {
		emit Notify(tr("Inscription: ")+str);
	}
	emit Notify(tr("Temperature: %1.").arg(world->Temperature(i, j, k)));
	emit Notify(tr("Durability: %1.").arg(block->Durability()));
	emit Notify(tr("Weight: %1.").arg(block->Weight()));
	world->Unlock();
}

void Player::Jump() {
	if ( !player ) {
		return;
	}
	usingType=USAGE_TYPE_NO;
	if ( GetCreativeMode() ) {
		if ( (UP==dir && z<HEIGHT-2) || (DOWN==dir && z>1) ) {
			player->GetDeferredAction()->SetGhostMove();
		}
	} else {
		player->GetDeferredAction()->SetJump();
	}
}

void Player::Move(const int dir) {
	if ( player ) {
		SetDir(dir);
		if ( GetCreativeMode() ) {
			player->GetDeferredAction()->SetGhostMove();
		} else {
			player->GetDeferredAction()->SetMove();
		}
	}
}

void Player::StopUseAll() { usingType = usingSelfType = USAGE_TYPE_NO; }

void Player::Backpack() {
	if ( PlayerInventory() ) {
		usingSelfType=( USAGE_TYPE_OPEN==usingSelfType ) ?
			USAGE_TYPE_NO :
			USAGE_TYPE_OPEN;
	}
}

void Player::Use(const short x, const short y, const short z) {
	world->WriteLock();
	const int us_type=world->GetBlock(x, y, z)->Use();
	usingType=( us_type==usingType ) ? USAGE_TYPE_NO : us_type;
	world->Unlock();
}

void Player::Inscribe(const short x, const short y, const short z) const {
	world->WriteLock();
	if ( player ) {
		world->Inscribe(x, y, z);
	} else {
		emit Notify(tr("No player."));
	}
	world->Unlock();
}

Block * Player::ValidBlock(const ushort num) const {
	if ( !player ) {
		emit Notify("Player does not exist.");
		return 0;
	}
	Inventory * const inv=player->HasInventory();
	if ( !inv ) {
		emit Notify("Player has no inventory.");
		return 0;
	}
	if ( num>=inv->Size() ) {
		emit Notify("No such place.");
		return 0;
	}
	Block * const block=inv->ShowBlock(num);
	if ( !block ) {
		emit Notify("Nothing here.");
		return 0;
	}
	return block;
}

void Player::Use(const ushort num) {
	world->WriteLock();
	Block * const block=ValidBlock(num);
	if ( block ) {
		block->Use(player);
	}
	world->Unlock();
}

void Player::Throw(const short x, const short y, const short z,
		const ushort src, const ushort dest, const ushort num)
{
	player->GetDeferredAction()->SetThrow(x, y, z, src, dest, num);
}

void Player::Obtain(const short x, const short y, const short z,
		const ushort src, const ushort dest, const ushort num)
{
	world->WriteLock();
	world->Get(player, x, y, z, src, dest, num);
	Block * const using_block=world->GetBlock(x, y, z);
	if ( using_block->HasInventory()->IsEmpty() &&
			PILE==using_block->Kind())
	{
		usingType=USAGE_TYPE_NO;
	}
	emit Updated();
	world->Unlock();
}

bool Player::Wield(const ushort num) {
	world->WriteLock();
	if ( ValidBlock(num) ) {
		for (ushort i=0; i<=Dwarf::ON_LEGS; ++i ) {
			InnerMove(num, i);
		}
		emit Updated();
	}
	world->Unlock();
	return false;
}

void Player::MoveInsideInventory(const ushort num_from, const ushort num_to,
		const ushort num)
{
	world->WriteLock();
	if ( ValidBlock(num_from) ) {
		InnerMove(num_from, num_to, num);
	}
	world->Unlock();
}

void Player::InnerMove(const ushort num_from, const ushort num_to,
		const ushort num)
{
	PlayerInventory()->MoveInside(num_from, num_to, num);
	emit Updated();
}

void Player::Inscribe(const ushort num) {
	world->WriteLock();
	if ( ValidBlock(num) ) {
		QString str;
		emit GetString(str);
		PlayerInventory()->InscribeInv(num, str);
	}
	world->Unlock();
}

void Player::Eat(const ushort num) {
	world->WriteLock();
	Block * const food=ValidBlock(num);
	if ( food ) {
		Animal * const animal=player->IsAnimal();
		if ( animal ) {
			if ( animal->Eat(food->Sub()) ) {
				PlayerInventory()->Pull(num);
				block_manager.DeleteBlock(food);
				emit Updated();
			}
		} else {
			emit Notify(tr("You cannot eat."));
		}
	}
	world->Unlock();
}

void Player::Build(
		const short x_target,
		const short y_target,
		const short z_target,
		const ushort slot)
{
	world->WriteLock();
	Block * const block=ValidBlock(slot);
	if ( block && (AIR!=world->Sub(x, y, z-1) || 0==player->Weight()) ) {
		player->GetDeferredAction()->
			SetBuild(x_target, y_target, z_target, block, slot);
	}
	world->Unlock();
}

void Player::Craft(const ushort num) {
	world->WriteLock();
	Inventory * const inv=PlayerInventory();
	if ( inv ) {
		const bool craft=inv->MiniCraft(num);
		if ( craft ) {
			emit Updated();
		}
	} else {
		Notify("Cannot craft.");
	}
	world->Unlock();
}

void Player::TakeOff(const ushort num) {
	world->WriteLock();
	if ( ValidBlock(num) ) {
		for (ushort i=PlayerInventory()->Start();
				i<PlayerInventory()->Size(); ++i)
		{
			InnerMove(num, i, PlayerInventory()->Number(num));
		}
	}
	world->Unlock();
}

void Player::ProcessCommand(QString & command) {
	//don't forget lock and unlock world mutex where needed:
	//world->WriteLock();
	//or
	//world->ReadLock();
	//world->Unlock();
	QTextStream comm_stream(&command);
	QString request;
	comm_stream >> request;
	if ( "give"==request || "get"==request ) {
		world->WriteLock();
		Inventory * const inv=PlayerInventory();
		if ( !creativeMode ) {
			emit Notify(tr("You are not in Creative Mode."));
		} else if ( inv ) {
			int kind, sub, num;
			comm_stream >> kind >> sub >> num;
			if ( num <= 0 ) {
				num=1;
			}
			while ( num && inv->HasRoom() ) {
				for (ushort i=9; i && num; --i) {
					inv->Get(block_manager.
						NewBlock(kind, sub));
					--num;
				}
			}
			if ( num > 0 ) {
				emit Notify(tr("No place for %1 things.").
					arg(num));
			}
			emit Updated();
		} else {
			emit Notify(tr("No room."));
		}
		world->Unlock();
	} else if ( "move"==request ) {
		int dir;
		comm_stream >> dir;
		Move(dir);
	} else if ( "what"==request ) {
		ushort x_what, y_what, z_what;
		comm_stream >> x_what >> y_what >> z_what;
		world->ReadLock();
		if ( creativeMode ) {
			Examine(x_what, y_what, z_what);
		} else {
			if (
					qAbs(x-x_what) > 1 ||
					qAbs(y-y_what) > 1 ||
					qAbs(z-z_what) > 1)
			{
				emit Notify(tr("Too far."));
			} else {
				Examine(x_what, y_what, z_what);
			}
		}
		world->Unlock();
	} else if ( "moo"==request ) {
		emit Notify("^__^");
		emit Notify("(oo)\\_______");
		emit Notify("(__)\\       )\\/\\");
		emit Notify("    ||----w |");
		emit Notify("    ||     ||");
	} else {
		emit Notify(tr("Don't know such command: \"%1\".").
			arg(command));
	}
} //Player::ProcessCommand

void Player::Get(Block * const block) {
	Inventory * const inv=PlayerInventory();
	if ( inv ) {
		inv->Get(block);
	}
}

bool Player::Visible(const ushort x_to, const ushort y_to, const ushort z_to)
const {
	return world->Visible(x, y, z, x_to, y_to, z_to);
}

int  Player::GetDir() const { return dir; }
void Player::SetDir(const int direction) {
	usingType=USAGE_TYPE_NO;
	if ( player ) {
		player->SetDir(direction);
	}
	dir=direction;
	emit Updated();
}

void Player::Damage(
		const short x_target,
		const short y_target,
		const short z_target)
const {
	player->GetDeferredAction()->SetDamage(x_target, y_target, z_target);
}

int Player::DamageKind() const {
	return creativeMode ?
		TIME :
		player ? player->DamageKind() : NO_HARM;
}

ushort Player::DamageLevel() const {
	return creativeMode ?
		MAX_DURABILITY :
		player ? player->DamageLevel() : 0;
}

void Player::CheckOverstep(const int dir) {
	UpdateXYZ();
	if (
			x <  (world->NumShreds()/2-1)*SHRED_WIDTH ||
			y <  (world->NumShreds()/2-1)*SHRED_WIDTH ||
			x >= (world->NumShreds()/2+2)*SHRED_WIDTH ||
			y >= (world->NumShreds()/2+2)*SHRED_WIDTH )
	{
		emit OverstepBorder(dir);
		if ( GetCreativeMode() ) {
			//coordinates of normal (non-creative) player are
			//reloaded (shifted corresponding to world reload)
			//automatically since such player is registered in
			//his shred.
			//This helps creative player shift his coordinates.
			switch ( dir ) {
				case NORTH: player->ReloadToNorth(); break;
				case SOUTH: player->ReloadToSouth(); break;
				case EAST:  player->ReloadToEast();  break;
				case WEST:  player->ReloadToWest();  break;
			}
		}
		UpdateXYZ();
	}
	emit Moved(GlobalX(), GlobalY(), z);
	//for curses screen to update itself in creative mode
	//IDEA: make signal to tell srceen player coordinates in creative mode
	emit Updated();
}

void Player::BlockDestroy() {
	if ( cleaned ) {
		return;
	}
	emit Notify("You died.");
	player=0;
	usingType=USAGE_TYPE_NO;
	usingSelfType=USAGE_TYPE_NO;

	emit Destroyed();
	world->ReloadAllShreds(
		homeLati,
		homeLongi,
		homeX,
		homeY,
		homeZ,
		world->NumShreds());
}

void Player::WorldSizeReloadStart() {
	if ( player ) {
		disconnect(player, SIGNAL(Destroyed()), 0, 0);
	}
	homeX-=world->NumShreds()/2*SHRED_WIDTH;
	homeY-=world->NumShreds()/2*SHRED_WIDTH;
}

void Player::WorldSizeReloadFinish() {
	homeX+=world->NumShreds()/2*SHRED_WIDTH;
	homeY+=world->NumShreds()/2*SHRED_WIDTH;
}

void Player::SetPlayer(
		const ushort player_x,
		const ushort player_y,
		const ushort player_z)
{
	x=player_x;
	y=player_y;
	z=player_z;
	if ( GetCreativeMode() ) {
		( player=block_manager.NewBlock(CREATOR, DIFFERENT)->
			ActiveBlock() )->SetXYZ(x, y, z);
	} else {
		Block * const target_block=world->GetBlock(x, y, z);
		if ( DWARF!=target_block->Kind() ) {
			world->Build( (player=block_manager.
					NewBlock(DWARF, H_MEAT)->
						ActiveBlock()),
				x, y, z,
				GetDir(),
				0,
				true /*force build*/ );
		} else {
			player=target_block->ActiveBlock();
		}
	}
	player->SetDeferredAction(new DeferredAction(player, GetWorld()));
	SetDir(player->GetDir());

	connect(player, SIGNAL(Destroyed()),
		this, SLOT(BlockDestroy()),
		Qt::DirectConnection);
	connect(player, SIGNAL(Moved(int)),
		this, SLOT(CheckOverstep(int)),
		Qt::DirectConnection);
	connect(player, SIGNAL(Updated()),
		this, SIGNAL(Updated()),
		Qt::DirectConnection);
	connect(player, SIGNAL(ReceivedText(const QString &)),
		this, SIGNAL(Notify(const QString &)),
		Qt::DirectConnection);
} //Player::SetPlayer

void Player::SetNumShreds(ushort num) const {
	const ushort num_shreds=world->NumShreds();
	if ( num < 5 ) {
		emit Notify(tr("Shreds number too small: %1x%1.").arg(num));
	} else if ( 1 != num%2 ) {
		emit Notify(tr("Invalid shreds number: %1x%1.").arg(num));
	} else {
		const short shift=num/2 - num_shreds/2;
		world->ReloadAllShreds(
			//put loaded zone center to where player is
			world->Latitude()  + x/SHRED_WIDTH - num_shreds/2,
			world->Longitude() + y/SHRED_WIDTH - num_shreds/2,
			//new x and y correspond to player stanging
			//in loaded zone center
			x - (x/SHRED_WIDTH)*SHRED_WIDTH +
				(num_shreds/2+shift)*SHRED_WIDTH,
			y - (y/SHRED_WIDTH)*SHRED_WIDTH +
				(num_shreds/2+shift)*SHRED_WIDTH,
			z,
			num);
	}
	emit Notify(tr("Shreds number is %1x%1 now.").arg(num_shreds));
}

Player::Player(World * const w) :
		dir(NORTH),
		world(w),
		usingType(USAGE_TYPE_NO),
		usingSelfType(USAGE_TYPE_NO),
		cleaned(false)
{
	QSettings sett(QDir::currentPath()+'/'+
			world->WorldName()+"/settings.ini",
		QSettings::IniFormat);
	sett.beginGroup("player");
	homeLongi=sett.value("home_longitude",
		qlonglong(world->GetSpawnLongi())).toLongLong();
	homeLati =sett.value("home_latitude",
		qlonglong(world->GetSpawnLati())).toLongLong();
	homeX=sett.value("home_x", 0).toInt();
	homeY=sett.value("home_y", 0).toInt();
	homeZ=sett.value("home_z", HEIGHT/2).toInt();
	x    =sett.value("current_x", 0).toInt();
	y    =sett.value("current_y", 0).toInt();
	z    =sett.value("current_z", HEIGHT/2+1).toInt();
	creativeMode=sett.value("creative_mode", false).toBool();

	const ushort plus=world->NumShreds()/2*SHRED_WIDTH;
	homeX+=plus;
	homeY+=plus;
	x+=plus;
	y+=plus;
	SetPlayer(x, y, z);

	connect(world, SIGNAL(NeedPlayer(
			const ushort,
			const ushort,
			const ushort)),
		this, SLOT(SetPlayer(
			const ushort,
			const ushort,
			const ushort)),
		Qt::DirectConnection);
	connect(this, SIGNAL(OverstepBorder(int)),
		world, SLOT(ReloadShreds(int)),
		Qt::DirectConnection);
	connect(world, SIGNAL(StartReloadAll()),
		this, SLOT(WorldSizeReloadStart()),
		Qt::DirectConnection);
	connect(world, SIGNAL(FinishReloadAll()),
		this, SLOT(WorldSizeReloadFinish()),
		Qt::DirectConnection);
} //Player::Player

void Player::CleanAll() {
	world->WriteLock();
	if ( cleaned ) {
		world->Unlock();
		return;
	}
	cleaned=true;

	if ( creativeMode ) {
		block_manager.DeleteBlock(player);
	}

	QSettings sett(QDir::currentPath()+'/'+
			world->WorldName()+"/settings.ini",
		QSettings::IniFormat);
	sett.beginGroup("player");
	sett.setValue("home_longitude", qlonglong(homeLongi));
	sett.setValue("home_latitude", qlonglong(homeLati));
	const ushort min=world->NumShreds()/2*SHRED_WIDTH;
	sett.setValue("home_x", homeX-min);
	sett.setValue("home_y", homeY-min);
	sett.setValue("home_z", homeZ);
	sett.setValue("current_x", x-min);
	sett.setValue("current_y", y-min);
	sett.setValue("current_z", z);
	sett.setValue("creative_mode", creativeMode);

	world->Unlock();
}

Player::~Player() { CleanAll(); }
