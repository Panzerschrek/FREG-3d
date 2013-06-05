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

#include <QDataStream>
#include <QTextStream>
#include <QString>
#include "blocks.h"
#include "world.h"
#include "Shred.h"
#include "CraftManager.h"
#include "BlockManager.h"

//Block::
	QString & Block::FullName(QString & str) const {
		switch ( sub ) {
			case STAR: case SUN_MOON: case SKY:
			case AIR:        return str="Air";
			case WATER:      return str="Ice";
			case STONE:      return str="Stone";
			case MOSS_STONE: return str="Moss stone";
			case NULLSTONE:  return str="Nullstone";
			case GLASS:      return str="Glass";
			case SOIL:       return str="Soil";
			case HAZELNUT:   return str="Hazelnut";
			case WOOD:       return str="Wood";
			case GREENERY:   return str="Leaves";
			case ROSE:       return str="Rose";
			case A_MEAT:     return str="Animal meat";
			case H_MEAT:     return str="Not animal meat";
			case IRON:       return str="Iron block";
			case SAND:       return str="Sandstone";
			case CLAY:       return str="Clay brick";
			default:
				fprintf(stderr,
					"Block::FullName: unlisted sub: %d.\n",
					sub);
				return str="Unknown block";
		}
	}

	void Block::SetTransparency(const quint8 transp) {
		if ( UNDEF==transp ) {
			switch ( sub ) {
				case AIR: transparent=INVISIBLE; break;
				case WATER: case GREENERY:
				case GLASS:
					  transparent=BLOCK_TRANSPARENT;
				break;
				default: transparent=BLOCK_OPAQUE;
			}
		} else {
			transparent=transp;
		}
	}

	int Block::Damage(const ushort dmg, const int dmg_kind) {
		switch ( Sub() ) {
			case DIFFERENT:
				if ( TIME==dmg_kind ) {
					return durability=0;
				}
				//no break, only time damages DIFFERENT
			case NULLSTONE:
			case STAR:
			case AIR:
			case SKY:
			case SUN_MOON:
			case WATER:
				return ( HEAT==dmg_kind || TIME==dmg_kind ) ?
					durability-=dmg : durability;
			case MOSS_STONE:
			case STONE:
				return ( MINE==dmg_kind ) ?
					durability-=2*dmg :
					durability-=dmg;
			case GREENERY:
			case GLASS: return durability=0;
			case ROSE:
			case HAZELNUT:
			case WOOD:
				return (CUT==dmg_kind) ?
					durability-=2*dmg :
					durability-=dmg;
			case SAND:
			case SOIL:
				return (DIG==dmg_kind) ?
					durability-=2*dmg :
					durability-=dmg;
			case A_MEAT:
			case H_MEAT:
				return (THRUST==dmg_kind) ?
					durability-=2*dmg :
					durability-=dmg;
			default: return durability-=dmg;
		}
	}

	Block * Block::DropAfterDamage() const {
		return ( BLOCK==Kind() && GLASS!=sub ) ?
			block_manager.NormalBlock(sub) : 0;
	}

	int  Block::Kind() const { return BLOCK; }

	bool Block::Catchable() const { return false; }

	int  Block::Movable() const {
		return ( AIR==Sub() ) ? ENVIRONMENT : NOT_MOVABLE;
	}

	int  Block::BeforePush(const int, Block * const) { return NO_ACTION; }

	int  Block::Move(const int) { return 0; }

	bool Block::Armour() const { return false; }

	int  Block::Wearable() const { return WEARABLE_NOWHERE; }

	int  Block::DamageKind() const { return CRUSH; }

	void Block::Restore() { durability=MAX_DURABILITY; }

	void Block::SetDir(const int dir) { direction=dir; }

	int  Block::GetDir() const { return direction; }

	int  Block::Sub() const { return sub; }

	int  Block::Transparent() const { return transparent; }

	short Block::Durability() const { return durability; }

	uchar Block::LightRadius() const { return 0; }

	QString & Block::GetNote(QString & str) const { return str=*note; }

	ushort Block::DamageLevel() const { return 1; }

	usage_types Block::Use() { return NO; }

	Inventory * Block::HasInventory() { return 0; }

	Animal * Block::IsAnimal() { return 0; }

	Active * Block::ActiveBlock() { return 0; }

	int Block::Temperature() const {
		switch (sub) {
			case WATER: return -100;
			default: return 0;
		}
	}

	void Block::ReceiveSignal(const QString &) {}

	void Block::Inscribe(const QString & str) { *note=str; }

	void Block::SaveAttributes(QDataStream &) const {}

	bool Block::operator==(const Block & block) const {
		return ( block.Kind()==Kind() &&
				block.Sub()==Sub() &&
				block.GetDir()==GetDir() &&
				block.Durability()==Durability() &&
				*block.note==*note );
	}

	ushort Block::Weight() const {
		switch ( Sub() ) {
			case NULLSTONE: return WEIGHT_NULLSTONE;
			case SAND:      return WEIGHT_SAND;
			case SOIL:      return WEIGHT_SAND+WEIGHT_WATER;
			case GLASS:     return WEIGHT_GLASS;
			case WOOD:      return WEIGHT_WATER-1;
			case IRON:      return WEIGHT_IRON;
			case GREENERY:  return WEIGHT_GREENERY;
			case ROSE:
			case HAZELNUT:  return WEIGHT_MINIMAL;
			case MOSS_STONE:
			case STONE:     return WEIGHT_STONE;
			case A_MEAT:
			case H_MEAT:    return WEIGHT_WATER-10;
			case SKY:
			case STAR:
			case SUN_MOON:
			case AIR:       return WEIGHT_AIR;
			default:        return WEIGHT_WATER;
		}
	}

	void Block::SaveToFile(QDataStream & out) const {
		const bool normal=(this==block_manager.NormalBlock(Sub()));
		out << (quint16)Kind() << sub << normal;
		if ( normal ) {
			return;
		}
		out << direction << durability << *note;
		SaveAttributes(out);
	}

	Block::Block(const int sb, const quint8 transp) :
			sub(sb),
			direction(UP),
			note(new QString("")),
			durability(MAX_DURABILITY)
	{
		SetTransparency(transp);
	}

	Block::Block(QDataStream & str, const int sub_, const quint8 transp) :
			sub(sub_),
			note(new QString)
	{
		SetTransparency(transp);
		str >> direction >> durability >> *note;
	}

	Block::~Block() { delete note; }

//Plate::
	QString & Plate::FullName(QString & str) const {
		switch ( Sub() ) {
			case WOOD: return str="Wooden board";
			case IRON: return str="Iron plate";
			case STONE: return str="Stone slab";
			default:
				fprintf(stderr,
					"Plate::FullName: unlisted sub: %d",
					Sub());
				return str="Strange plate";
		}
	}

	int Plate::Kind() const { return PLATE; }

	Block * Plate::DropAfterDamage() const {
		return block_manager.NewBlock(PLATE, Sub());
	}

	int Plate::BeforePush(const int, Block * const) { return JUMP; }

	ushort Plate::Weight() const { return Block::Weight()/4; }

	Plate::Plate(const int sub)
			:
			Block(sub, NONSTANDARD)
	{}

	Plate::Plate(QDataStream & str, const int sub) :
			Block(str, sub, NONSTANDARD)
	{}

//Ladder::
	Block * Ladder::DropAfterDamage() const {
		return block_manager.NewBlock(LADDER, Sub());
	}

	QString & Ladder::FullName(QString & str) const {
		return str="Ladder";
	}

	int Ladder::Kind() const { return LADDER; }

	int Ladder::BeforePush(const int, Block * const) { return MOVE_UP; }

	ushort Ladder::Weight() const { return Block::Weight()*3; }

	bool Ladder::Catchable() const { return true; }

	Ladder::Ladder(const int sub) :
			Block(sub, NONSTANDARD)
	{}

	Ladder::Ladder(QDataStream & str, const int sub) :
			Block(str, sub, NONSTANDARD)
	{}

//Weapon::
	QString & Weapon::FullName(QString & str) const {
		switch ( Sub() ) {
			case STONE: return str="Pebble";
			case IRON:  return str="Spike";
			case WOOD:  return str="Stick";
			default:
				fprintf(stderr,
					"Weapon::FullName: unlisted sub: %d\n",
					Sub());
				return str="Some weapon";
		}
	}

	int Weapon::Kind() const { return WEAPON; }

	ushort Weapon::DamageLevel() const {
		switch ( Sub() ) {
			case WOOD: return 4;
			case IRON: return 6;
			case STONE: return 5;
			default:
				fprintf(stderr,
					"Weapon::DamageLevel: \
					unlisted sub: %d\n",
					Sub());
				return 1;
		}
	}

	int  Weapon::DamageKind() const {
		return ( IRON==Sub() ) ? THRUST : CRUSH;
	}

	int Weapon::BeforePush(const int, Block * const) {
		return ( IRON==Sub() ) ? DAMAGE : NO_ACTION;
	}

	ushort Weapon::Weight() const { return Block::Weight()/4; }

	int Weapon::Wearable() const { return WEARABLE_ARM; }

	Weapon::Weapon(const int sub) :
			Block(sub, NONSTANDARD)
	{}

	Weapon::Weapon(QDataStream & str, const int sub) :
			Block(str, sub, NONSTANDARD)
	{}

//Pick::
	int Pick::Kind() const { return PICK; }

	int Pick::DamageKind() const { return MINE; }

	ushort Pick::DamageLevel() const {
		switch ( Sub() ) {
			case IRON: return 10;
			default:
				fprintf(stderr,
					"Pick::DamageLevel: \
					unlisted sub: %d\n",
					Sub());
				return 1;
		}
	}

	QString & Pick::FullName(QString & str) const {
		switch ( sub ) {
			case IRON: return str="Iron pick";
			default:
				fprintf(stderr,
					"Pick::FullName(QString&): \
					Pick has unknown substance: %d\n",
					sub);
				return str="Strange pick";
		}
	}

	Pick::Pick(const int sub) :
			Weapon(sub)
	{}

	Pick::Pick(QDataStream & str, const int sub) :
			Weapon(str, sub)
	{}

//Active::
	QString & Active::FullName(QString & str) const {
		switch ( Sub() ) {
			case SAND:  return str="Sand";
			case WATER: return str="Snow";
			default:
				fprintf(stderr,
					"Active::FullName: Unlisted sub: %d\n",
					sub);
				return str="Unkown active block";
		}
	}

	int Active::Kind() const { return ACTIVE; }

	Active * Active::ActiveBlock() { return this; }

	void Active::SetFalling(const bool set) { falling=set; }
	bool Active::IsFalling() const { return falling; }
	bool Active::FallDamage() {
		if ( fall_height > safe_fall_height ) {
			const ushort dmg=(fall_height -
				safe_fall_height)*10;
			fall_height=0;
			GetWorld()->
				Damage(x_self, y_self, z_self-1, dmg,
					DAMAGE_FALL);
			return GetWorld()->
				Damage(x_self, y_self,z_self, dmg,
					DAMAGE_FALL);
		} else {
			fall_height=0;
			return false;
		}
	}

	ushort Active::X() const { return x_self; }
	ushort Active::Y() const { return y_self; }
	ushort Active::Z() const { return z_self; }

	int Active::Move(const int dir) {
		switch ( dir ) {
			case NORTH: --y_self; break;
			case SOUTH: ++y_self; break;
			case EAST:  ++x_self; break;
			case WEST:  --x_self; break;
			case UP:    ++z_self; break;
			case DOWN:  --z_self; break;
			default:
				fprintf(stderr,
					"Active::Move: unlisted dir: %d\n",
					dir);
				return 0;
		}
		if ( DOWN==dir ) {
			++fall_height;
		} else if ( GetWorld()->
				GetShred(x_self, y_self)!=whereShred )
		{
			whereShred->RemActive(this);
			(whereShred=GetWorld()->GetShred(x_self, y_self))->
				AddActive(this);
		}
		emit Moved(dir);
		return 0;
	}

	void Active::ActFrequent() {}
	void Active::ActRare() {}
	int  Active::ShouldAct() const { return NEVER; }

	void Active::SendSignalAround(const QString & signal) const {
		World * const world=GetWorld();
		if ( InBounds(x_self-1, y_self) ) {
			world->GetBlock(x_self-1, y_self, z_self)->
				ReceiveSignal(signal);
		}
		if ( InBounds(x_self+1, y_self) ) {
			world->GetBlock(x_self+1, y_self, z_self)->
				ReceiveSignal(signal);
		}
		if ( InBounds(x_self, y_self-1) ) {
			world->GetBlock(x_self, y_self-1, z_self)->
				ReceiveSignal(signal);
		}
		if ( InBounds(x_self, y_self+1) ) {
			world->GetBlock(x_self, y_self+1, z_self)->
				ReceiveSignal(signal);
		}
		world->GetBlock(x_self, y_self, z_self-1)->
			ReceiveSignal(signal);
		world->GetBlock(x_self, y_self, z_self+1)->
			ReceiveSignal(signal);
	}

	World * Active::GetWorld() const { return whereShred->GetWorld(); }

	bool Active::InBounds(
			const ushort x,
			const ushort y,
			const ushort z)
	const {
		return GetWorld()->InBounds(x, y, z);
	}

	int Active::Movable() const { return MOVABLE; }

	bool Active::ShouldFall() const { return true; }

	int Active::Damage(const ushort dmg, const int dmg_kind) {
		const int last_dur=durability;
		Block::Damage(dmg, dmg_kind);
		if ( last_dur != durability ) {
			switch ( dmg_kind ) {
				case HUNGER:
					ReceiveSignal(QObject::tr(
						"You faint from hunger!"));
					break;
				case HEAT:
					ReceiveSignal(QObject::tr(
						"You burn!"));
					break;
				case BREATH:
					ReceiveSignal(QObject::tr(
						"You choke withot air!"));
					break;
				case DAMAGE_FALL:
					ReceiveSignal(
						QObject::tr(
						"You fall, damage %1.").
						arg(last_dur-durability));
					break;
				default:
					ReceiveSignal(
						QObject::tr(
						"Received %1 damage!").
						arg(last_dur-durability));
			}
			emit Updated();
		}
		return durability;
	}

	void Active::ReceiveSignal(const QString & str) {
		emit ReceivedText(str);
	}

	void Active::ReloadToNorth() { y_self+=SHRED_WIDTH; }
	void Active::ReloadToSouth() { y_self-=SHRED_WIDTH; }
	void Active::ReloadToWest()  { x_self+=SHRED_WIDTH; }
	void Active::ReloadToEast()  { x_self-=SHRED_WIDTH; }

	void Active::SaveAttributes(QDataStream & out) const {
		out << fall_height;
	}

	void Active::Register(
			Shred * const sh,
			const ushort x,
			const ushort y,
			const ushort z)
	{
		if ( !whereShred ) {
			whereShred=sh;
			x_self=x;
			y_self=y;
			z_self=z;
			whereShred->AddActive(this);
			if ( ENVIRONMENT==sh->Movable(
					x%SHRED_WIDTH,
					y%SHRED_WIDTH, z-1) &&
					!(*this==*sh->GetBlock(
						x%SHRED_WIDTH,
						y%SHRED_WIDTH, z-1)) )
			{
				whereShred->AddFalling(this);
			}
		}
	}

	void Active::Unregister() {
		if ( whereShred ) {
			whereShred->RemActive(this);
			whereShred->RemFalling(this);
			whereShred=0;
		}
	}

	Active::Active(const int sub, const quint8 transp) :
			Block(sub, transp),
			fall_height(0),
			falling(false),
			x_self(),
			y_self(),
			z_self(),
			whereShred(0)
	{}

	Active::Active(QDataStream & str, const int sub, const quint8 transp) :
			Block(str, sub, transp),
			falling(false),
			x_self(),
			y_self(),
			z_self(),
			whereShred(0)
	{
		str >> fall_height;
	}

	Active::~Active() {
		Unregister();
		emit Destroyed();
	}

//Animal::
	void Animal::ActRare() {
		World * const world=GetWorld();
		if (
				AIR!=world->Sub(x_self, y_self, z_self+1) &&
				AIR!=world->Sub(x_self, y_self, z_self-1) &&
			InBounds(x_self+1, y_self) &&
				AIR!=world->Sub(x_self+1, y_self, z_self) &&
			InBounds(x_self-1, y_self) &&
				AIR!=world->Sub(x_self-1, y_self, z_self) &&
			InBounds(x_self, y_self+1) &&
				AIR!=world->Sub(x_self, y_self+1, z_self) &&
			InBounds(x_self, y_self-1) &&
				AIR!=world->Sub(x_self, y_self-1, z_self) )
		{
			if ( breath <= 0 ) {
				if ( world->Damage(x_self, y_self, z_self,
						10, BREATH) )
				{
					return;
				}
			} else
				--breath;
		} else if ( breath < MAX_BREATH ) {
			++breath;
		}
		if ( satiation <= 0 ) {
			if ( world->Damage(x_self, y_self, z_self,
					5, HUNGER) )
			{
				return;
			}
		} else {
			--satiation;
		}
		if ( durability < MAX_DURABILITY ) {
			++durability;
		}
		emit Updated();
	}
	int Animal::ShouldAct() const { return RARE; }

	ushort Animal::Breath() const { return breath; }

	ushort Animal::Satiation() const { return satiation; }

	void Animal::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		out << breath << satiation;
	}

	Animal * Animal::IsAnimal() { return this; }

	Animal::Animal(const int sub) :
			Active(sub, NONSTANDARD),
			breath(MAX_BREATH),
			satiation(SECONDS_IN_DAY)
	{}

	Animal::Animal(QDataStream & str, const int sub) :
			Active(str, sub, NONSTANDARD)
	{
		str >> breath >> satiation;
	}

//Inventory::
	bool Inventory::Access() const { return true; }

	ushort Inventory::Start() const { return 0; }

	Inventory * Inventory::HasInventory() { return this; }

	ushort Inventory::Size() const { return size; }

	bool Inventory::Drop(const ushort src, ushort dest,
			const ushort num,
			Inventory * const inv_to)
	{
		if ( dest<inv_to->Start() ) {
			dest=inv_to->Start();
		}
		bool ok_flag=false;
		for (ushort i=0; i<num; ++i) {
			if ( inv_to &&
					src<Size() &&
					dest<inv_to->Size() &&
					!inventory[src].isEmpty() &&
					inv_to->Get(inventory[src].top(), dest) )
			{
				ok_flag=true;
			}
			Pull(src);
		}
		return ok_flag;
	}

	bool Inventory::GetAll(Inventory * const from) {
		if ( !from || !from->Access() ) {
			return false;
		}
		for (ushort i=0; i<from->Size(); ++i) {
			from->Drop(i, 0, from->Number(i), this);
		}
		return true;
	}

	void Inventory::Pull(const ushort num) {
		if ( !inventory[num].isEmpty() )
			inventory[num].pop();
	}

	void Inventory::SaveAttributes(QDataStream & out) const {
		for (ushort i=0; i<Size(); ++i) {
			out << Number(i);
			for (ushort j=0; j<Number(i); ++j)
				inventory[i].top()->SaveToFile(out);
		}
	}

	bool Inventory::Get(Block * const block, ushort start) {
		if ( start<Start() ) {
			start=Start();
		}
		if ( !block ) {
			return true;
		}
		for (ushort i=start; i<Size(); ++i) {
			if ( GetExact(block, i) ) {
				return true;
			}
		}
		return false;
	}

	bool Inventory::GetExact(Block * const block, const ushort num) {
		if ( !block ) {
			return true;
		}
		if ( inventory[num].isEmpty() ||
				( *block==*inventory[num].top() &&
				Number(num)<max_stack_size ) )
		{
			inventory[num].push(block);
			return true;
		}
		return false;
	}

	void Inventory::MoveInside(const ushort num_from, const ushort num_to,
			const ushort num)
	{
		for (ushort i=0; i<num; ++i) {
			if ( GetExact(ShowBlock(num_from), num_to) ) {
				Pull(num_from);
			}
		}
	}

	int Inventory::InscribeInv(const ushort num, const QString & str) {
		const int number=Number(num);
		if ( !number ) {
			return 0;
		}
		const int sub=inventory[num].top()->Sub();
		if ( inventory[num].top()==block_manager.NormalBlock(sub) ) {
			for (ushort i=0; i<number; ++i)
				inventory[num].replace(i,
					block_manager.NormalBlock(sub));
		}
		for (ushort i=0; i<number; ++i)
			inventory[num].at(i)->Inscribe(str);
		return 0;
	}

	QString & Inventory::InvFullName(QString & str, const ushort i) const {
		return str=( inventory[i].isEmpty() ) ? "" :
			inventory[i].top()->FullName(str);
	}

	QString & Inventory::NumStr(QString & str, const ushort i) const {
		return str=( 1<Number(i) ) ?
			QString(" (%1x)").arg(Number(i)) :
			"";
	}

	ushort Inventory::GetInvWeight(const ushort i) const {
		return ( inventory[i].isEmpty() ) ? 0 :
			inventory[i].top()->Weight()*Number(i);
	}

	int Inventory::GetInvSub(const ushort i) const {
		return ( inventory[i].isEmpty() ) ? AIR :
			inventory[i].top()->Sub();
	}

	int Inventory::GetInvKind(const ushort i) const {
		return ( inventory[i].isEmpty() ) ? BLOCK :
			inventory[i].top()->Kind();
	}

	QString & Inventory::GetInvNote(QString & str, const ushort num)
	const {
		return str=inventory[num].top()->GetNote(str);
	}

	ushort Inventory::Weight() const {
		ushort sum=0;
		for (ushort i=0; i<Size(); ++i) {
			sum+=GetInvWeight(i);
		}
		return sum;
	}

	Block * Inventory::ShowBlock(const ushort num) const {
		return ( num>Size() || inventory[num].isEmpty() ) ?
			0 : inventory[num].top();
	}

	bool Inventory::IsEmpty() const {
		for (ushort i=Start(); i<Size(); ++i) {
			if ( !inventory[i].isEmpty() ) {
				return false;
			}
		}
		return true;
	}
	bool Inventory::HasRoom() const {
		for (ushort i=Start(); i<Size(); ++i) {
			if ( inventory[i].isEmpty() ) {
				return true;
			}
		}
		return false;
	}

	void Inventory::BeforePush(Block * const who) {
		Inventory * const inv = who->HasInventory();
		if ( inv ) {
			inv->GetAll(this);
		}
	}

	quint8 Inventory::Number(const ushort i) const {
		return inventory[i].size();
	}

	int Inventory::MiniCraft(const ushort num) {
		const ushort size=inventory[num].size();
		if ( !size )
			return 1; //empty
		craft_item item={
			size,
			GetInvKind(num),
			GetInvSub(num)
		};
		craft_item result;

		if ( craft_manager.MiniCraft(item, result) ) {
			while ( !inventory[num].isEmpty() ) {
				Block * const to_drop=ShowBlock(num);
				Pull(num);
				block_manager.DeleteBlock(to_drop);
			}
			for (ushort i=0; i<result.num; ++i)
				Get(block_manager.
					NewBlock(result.kind, result.sub));
			return 0; //success
		}
		return 2; //no such recipe
	}

	Inventory::Inventory(const ushort sz) :
			size(sz)
	{
		inventory=new QStack<Block *>[Size()];
	}

	Inventory::Inventory(QDataStream & str, const ushort sz) :
			size(sz)
	{
		inventory=new QStack<Block *>[Size()];
		for (ushort i=0; i<Size(); ++i) {
			quint8 num;
			str >> num;
			while ( num-- )
				inventory[i].push(block_manager.
					BlockFromFile(str));
		}
	}

	Inventory::~Inventory() {
		for (ushort i=0; i<Size(); ++i)
			while ( !inventory[i].isEmpty() ) {
				Block * const block=inventory[i].pop();
				block_manager.DeleteBlock(block);
			}
		delete [] inventory;
	}

//Dwarf::
	ushort Dwarf::Weight() const {
		World * const world=GetWorld();
		return (
				(InBounds(x_self+1, y_self) &&
					world->GetBlock(
						x_self+1,
						y_self,
						z_self)->Catchable()) ||
				(InBounds(x_self-1, y_self) &&
					world->GetBlock(
						x_self-1,
						y_self,
						z_self)->Catchable()) ||
				(InBounds(x_self, y_self+1) &&
					world->GetBlock(
						x_self,
						y_self+1,
						z_self)->Catchable()) ||
				(InBounds(x_self, y_self-1) &&
					world->GetBlock(
						x_self,
						y_self-1,
						z_self)->Catchable()) ) ?
			0 : Inventory::Weight()+Block::Weight();
	}

	Block * Dwarf::DropAfterDamage() const {
		return block_manager.NormalBlock(H_MEAT);
	}

	int Dwarf::Kind() const { return DWARF; }

	int Dwarf::Sub() const { return Block::Sub(); }

	QString & Dwarf::FullName(QString & str) const {
		return str="Dwarf " + *note;
	}

	ushort Dwarf::Start() const { return onLegs+1; }

	int Dwarf::DamageKind() const {
		if ( Number(inRight) ) {
			return ShowBlock(inRight)->DamageKind();
		} else if ( Number(inLeft) ) {
			return ShowBlock(inLeft)->DamageKind();
		} else {
			return CRUSH;
		}
	}

	ushort Dwarf::DamageLevel() const {
		ushort level=1;
		if ( Number(inRight) ) {
			level+=ShowBlock(inRight)->DamageLevel();
		}
		if ( Number(inLeft) ) {
			level+=ShowBlock(inLeft)->DamageLevel();
		}
		return level;
	}

	int Dwarf::Eat(Block * const to_eat) {
		if ( !to_eat )
			return 1;

		switch ( to_eat->Sub() ) {
			case HAZELNUT: satiation+=SECONDS_IN_HOUR/2; break;
			case H_MEAT:   satiation+=SECONDS_IN_HOUR*2.5; break;
			case A_MEAT:   satiation+=SECONDS_IN_HOUR*2; break;
			default: return 2; //not ate
		}

		if ( SECONDS_IN_DAY < satiation )
			satiation=1.1*SECONDS_IN_DAY;

		return 0; //ate
	}

	Inventory * Dwarf::HasInventory() { return Inventory::HasInventory(); }

	bool Dwarf::Access() const { return false; }

	void Dwarf::MoveInside(const ushort num_from, const ushort num_to,
			const ushort num)

	{
		Block * const block=ShowBlock(num_from);
		if ( block && (num_to > onLegs ||
				inRight==num_to || inLeft==num_to ||
				( onHead==num_to &&
					WEARABLE_HEAD==block->Wearable() ) ||
				( onBody==num_to &&
					WEARABLE_BODY==block->Wearable() ) ||
				( onLegs==num_to &&
					WEARABLE_LEGS==block->Wearable() )) )
		{
			for (ushort i=0; i<num; ++i) {
				Inventory::MoveInside(num_from, num_to, 1);
			}
		}
	}

	void Dwarf::SaveAttributes(QDataStream & out) const {
		Animal::SaveAttributes(out);
		Inventory::SaveAttributes(out);
	}

	uchar Dwarf::LightRadius() const { return 3; }

	void Dwarf::Inscribe(const QString &) {
		SendSignalAround(tr("Don't touch me!"));
	}

	Dwarf::Dwarf(const int sub) :
			Animal(sub),
			Inventory()
	{
		*note="Urist";
	}

	Dwarf::Dwarf(QDataStream & str, const int sub) :
			Animal(str, sub),
			Inventory(str)
	{}

//Chest::
	Block * Chest::DropAfterDamage() const {
		return block_manager.NewBlock(Kind(), Sub());
	}

	int Chest::Kind() const { return CHEST; }

	int Chest::Sub() const { return Block::Sub(); }

	QString & Chest::FullName(QString & str) const {
		switch (sub) {
			case WOOD:  return str=QObject::tr("Wooden chest");
			case STONE: return str=QObject::tr("Stone chest");
			default:
				fprintf(stderr,
					"Chest::FullName: unlisted sub: %d\n",
					sub);
				return str=QObject::tr("Chest");
		}
	}

	Inventory * Chest::HasInventory() {
		return Inventory::HasInventory();
	}

	usage_types Chest::Use() { return OPEN; }

	int Chest::BeforePush(const int, Block * const who) {
		Inventory::BeforePush(who);
		return NO_ACTION;
	}

	ushort Chest::Weight() const {
		return Block::Weight()*4+Inventory::Weight();
	}

	void Chest::SaveAttributes(QDataStream & out) const {
		Inventory::SaveAttributes(out);
	}

	Chest::Chest(const int s, const ushort size) :
			Block(s),
			Inventory(size)
	{}

	Chest::Chest(QDataStream & str, const int sub, const ushort size) :
			Block(str, sub),
			Inventory(str, size)
	{}

//Pile::
	int Pile::BeforePush(const int, Block * const who) {
		Inventory::BeforePush(who);
		return NO_ACTION;
	}

	void Pile::ActRare() {
		if ( ifToDestroy ) {
			GetWorld()->Damage(x_self, y_self, z_self, 0, TIME);
		}
	}
	int Pile::ShouldAct() const { return RARE; }

	int Pile::Kind() const { return PILE; }

	int Pile::Sub() const { return Block::Sub(); }

	QString & Pile::FullName(QString & str) const {
		switch ( Sub() ) {
			case DIFFERENT: return str=tr("Pile");
			default:
				fprintf(stderr,
					"Pile::FullName: unlisted sub: %d\n",
					Sub());
				return str=tr("Unknown pile");
		}
	}

	Inventory * Pile::HasInventory() { return Inventory::HasInventory(); }

	usage_types Pile::Use() { return OPEN; }

	ushort Pile::Weight() const { return Inventory::Weight(); }

	bool Pile::Drop(const ushort src, const ushort dest, const ushort num,
			Inventory * const inv)
	{
		const bool ret=Inventory::Drop(src, dest, num, inv);
		ifToDestroy=IsEmpty();
		return ret;
	}

	void Pile::Pull(const ushort num) {
		Inventory::Pull(num);
		ifToDestroy=IsEmpty();
	}

	void Pile::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		Inventory::SaveAttributes(out);
		out << ifToDestroy;
	}

	Pile::Pile(const int sub) :
			Active(sub, NONSTANDARD),
			Inventory(inv_size),
			ifToDestroy(false)
	{}

	Pile::Pile(QDataStream & str, const int sub) :
			Active(str, sub, NONSTANDARD),
			Inventory(str, inv_size)
	{
		str >> ifToDestroy;
	}

//Liquid::
	bool Liquid::CheckWater() const {
		World * const world=GetWorld();
		return (
				WATER==world->Sub(x_self, y_self, z_self-1) ||
				WATER==world->Sub(x_self, y_self, z_self+1) ||
				(InBounds(x_self-1, y_self) &&
					WATER==world->Sub(
						x_self-1, y_self, z_self)) ||
				(InBounds(x_self+1, y_self) &&
					WATER==world->Sub(
						x_self+1, y_self, z_self)) ||
				(InBounds(x_self, y_self-1) &&
					WATER==world->Sub(
						x_self, y_self-1, z_self)) ||
				(InBounds(x_self, y_self+1) &&
					WATER==world->Sub(
						x_self, y_self+1, z_self)));
	}

	void Liquid::ActRare() {
		World * const world=GetWorld();
		//IDEA: turn off water drying up in ocean
		if ( WATER==Sub() && !CheckWater() &&
				world->Damage(x_self, y_self, z_self,
					1, HEAT) )
		{
			return;
		}
		switch ( qrand()%20 ) {
			case 0:
				world->Move(x_self, y_self, z_self, NORTH);
			break;
			case 1:
				world->Move(x_self, y_self, z_self, EAST);
			break;
			case 2:
				world->Move(x_self, y_self, z_self, SOUTH);
			break;
			case 3:
				world->Move(x_self, y_self, z_self, WEST);
			break;
			default: return;
		}
	}
	int Liquid::ShouldAct() const  { return RARE; }

	int Liquid::Movable() const { return ENVIRONMENT; }

	int Liquid::Kind() const { return LIQUID; }

	QString & Liquid::FullName(QString & str) const {
		switch (sub) {
			case WATER: return str="Water";
			case STONE: return str="Lava";
			default:
				fprintf(stderr,
					"Liquid::FullName(QString&): \
					Liquid has unknown substance: %d\n",
					sub);
				return str="Unknown liquid";
		}
	}

	int Liquid::Temperature() const { return ( WATER==sub ) ? 0 : 1000; }

	Liquid::Liquid(const int sub) :
			Active(sub)
	{}

	Liquid::Liquid(QDataStream & str, const int sub) :
			Active(str, sub)
	{}

//Grass::
	void Grass::ActRare() {
		short i=x_self, j=y_self;
		//increase this if grass grows too fast
		switch ( qrand()%(SECONDS_IN_HOUR*2) ) {
			case 0: ++i; break;
			case 1: --i; break;
			case 2: ++j; break;
			case 3: --j; break;
			default: return;
		}

		World * const world=GetWorld();
		if ( InBounds(i, j) && world->Enlightened(i, j, z_self) ) {
			if ( AIR==world->Sub(i, j, z_self) &&
					SOIL==world->Sub(i, j, z_self-1) )
				world->Build(block_manager.NewBlock(GRASS,
						Sub()), i, j, z_self);
			else if ( SOIL==world->Sub(i, j, z_self) &&
					AIR==world->Sub(i, j, z_self+1) )
				world->Build(block_manager.NewBlock(GRASS,
						Sub()), i, j, z_self+1);
		}
	}
	int Grass::ShouldAct() const  { return RARE; }

	QString & Grass::FullName(QString & str) const {
		switch ( sub ) {
			case GREENERY: return str="Grass";
			default:
				fprintf(stderr,
					"Grass::FullName(QString&): \
					unlisted sub: %d\n",
					sub);
				return str="Unknown plant";
		}
	}

	int Grass::Kind() const { return GRASS; }

	bool Grass::ShouldFall() const { return false; }

	int Grass::BeforePush(const int, Block * const) { return DESTROY; }

	Grass::Grass(const int sub) :
			Active(sub)
	{}

	Grass::Grass(QDataStream & str, const int sub) :
			Active(str, sub)
	{}

//Bush::
	QString & Bush::FullName(QString & str) const { return str="Bush"; }

	int Bush::Kind() const { return BUSH; }

	int Bush::Sub() const { return Block::Sub(); }

	usage_types Bush::Use() { return OPEN; }

	Inventory * Bush::HasInventory() { return Inventory::HasInventory(); }

	int Bush::Movable() const { return NOT_MOVABLE; }

	ushort Bush::Weight() const {
		return Inventory::Weight()+Block::Weight();
	}

	void Bush::ActRare() {
		if ( 0==qrand()%(SECONDS_IN_HOUR*4) ) {
			Get(block_manager.NormalBlock(HAZELNUT));
		}
	}
	int Bush::ShouldAct() const  { return RARE; }

	int Bush::BeforePush(const int, Block * const who) {
		Inventory::BeforePush(who);
		return NO_ACTION;
	}

	bool Bush::ShouldFall() const { return false; }

	Block * Bush::DropAfterDamage() const {
		return block_manager.NormalBlock(WOOD);
	}

	void Bush::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		Inventory::SaveAttributes(out);
	}

	Bush::Bush(const int sub) :
			Active(sub),
			Inventory(bush_size)
	{}

	Bush::Bush(QDataStream & str, const int sub) :
			Active(str, sub),
			Inventory(str, bush_size)
	{}

//Rabbit::
	short Rabbit::Attractive(const int sub) const {
		switch ( sub ) {
			case H_MEAT: return -8;
			case A_MEAT: return -1;
			case GREENERY: return 1;
			case SAND: return -1;
			default: return 0;
		}
	}

	void Rabbit::ActFrequent() {
		World * const world=GetWorld();
		//eat sometimes
		if ( SECONDS_IN_DAY/2 > satiation ) {
			for (ushort x=x_self-1; x<=x_self+1; ++x)
			for (ushort y=y_self-1; y<=y_self+1; ++y)
			for (ushort z=z_self-1; z<=z_self+1; ++z) {
				if ( InBounds(x, y) &&
						GREENERY==world->Sub(x, y, z) )
				{
					world->Eat(x_self, y_self, z_self,
						x, y, z);
					return;
				}
			}
		}
		//analyse world around
		short for_north=0, for_west=0;
		for (ushort x=x_self-4; x<=x_self+4; ++x)
		for (ushort y=y_self-4; y<=y_self+4; ++y)
		for (ushort z=z_self-1; z<=z_self+3; ++z) {
			if ( InBounds(x, y, z) ) {
				const short attractive=
					Attractive(world->Sub(x, y, z));
				if ( attractive &&
						world->DirectlyVisible(
							x_self, y_self,	z_self,
							x, y, z) )
				{
					if ( y!=y_self ) {
						for_north+=attractive/
							(y_self-y);
					}
					if ( x!=x_self ) {
						for_west +=attractive/
							(x_self-x);
					}
				}
			}
		}
		//make direction and move there
		const ushort calmness=5;
		if ( qAbs(for_north)>calmness || qAbs(for_west)>calmness ) {
			SetDir( ( qAbs(for_north)>qAbs(for_west) ) ?
				( ( for_north>0 ) ? NORTH : SOUTH ) :
				( ( for_west >0 ) ? WEST  : EAST  ) );
			if ( qrand()%2 ) {
				world->Move(x_self, y_self, z_self, direction);
			} else {
				world->Jump(x_self, y_self, z_self);
			}
		} else {
			switch ( qrand()%60 ) {
				case 0: SetDir(NORTH); break;
				case 1: SetDir(SOUTH); break;
				case 2: SetDir(EAST);  break;
				case 3: SetDir(WEST);  break;
				default: return;
			}
			world->Move(x_self, y_self, z_self, GetDir());
		}
	}
	int Rabbit::ShouldAct() const { return FREQUENT_AND_RARE; }

	Block * Rabbit::DropAfterDamage() const {
		return block_manager.NormalBlock(A_MEAT);
	}

	QString & Rabbit::FullName(QString & str) const {
		return str="Rabbit";
	}

	int Rabbit::Kind() const { return RABBIT; }

	int Rabbit::Eat(Block * const to_eat) {
		if ( NULL==to_eat )
			return 2;

		if ( GREENERY==to_eat->Sub() ) {
			satiation+=SECONDS_IN_HOUR*4;
			return 1;
		}
		return 0;
	}

	Rabbit::Rabbit(const int sub) :
			Animal(sub)
	{}

	Rabbit::Rabbit(QDataStream & str, const int sub) :
			Animal(str, sub)
	{}

//Workbench::
	void Workbench::Craft() {
		while ( Number(0) ) { //remove previous product
			Block * const to_push=ShowBlock(0);
			Pull(0);
			block_manager.DeleteBlock(to_push);
		}
		craft_recipe recipe;
		for (ushort i=Start(); i<Size(); ++i)
			if ( Number(i) ) {
				craft_item * item=new craft_item;
				item->num=Number(i);
				item->kind=GetInvKind(i);
				item->sub=GetInvSub(i);
				recipe.append(item);
			}
		craft_item result;
		if ( craft_manager.Craft(recipe, result) ) {
			for (ushort i=0; i<result.num; ++i) {
				GetExact(block_manager.
					NewBlock(result.kind, result.sub), 0);
			}
		}
		for (ushort i=0; i<recipe.size(); ++i) {
			delete recipe.at(i);
		}
	}

	bool Workbench::Drop(const ushort src, const ushort dest,
			const ushort num,
			Inventory * const inv_to)
	{
		if ( !inv_to ||
				src>=Size() || dest>=inv_to->Size() ||
				!Number(src) )
		{
			return false;
		}
		if ( src==0 ) {
			while ( Number(0) ) {
				if ( !inv_to->Get(ShowBlock(0)) ) {
					return false;
				}
				Pull(0);
			}
			for (ushort i=Start(); i<Size(); ++i) {
				while ( Number(i) ) {
					Block * const to_pull=ShowBlock(i);
					Pull(i);
					block_manager.DeleteBlock(to_pull);
				}
			}
			return true;
		} else {
			bool ok_flag=false;
			for (ushort i=0; i<num; ++i) {
				if ( inv_to->Get(ShowBlock(src), dest) ) {
					ok_flag=true;
				}
				Pull(src);
				Craft();
			}
			return ok_flag;
		}
	}

	QString & Workbench::FullName(QString & str) const {
		switch ( Sub() ) {
			case WOOD: return str="Workbench";
			case IRON: return str="Iron anvil";
			default:
				fprintf(stderr,
					"Bench::FullName: unlisted sub: %d\n",
					Sub());
				return str="Strange workbench";
		}
	}

	int Workbench::Kind() const { return WORKBENCH; }

	ushort Workbench::Start() const { return 1; }

	bool Workbench::Get(Block * const block, const ushort start) {
		if ( Inventory::Get(block, start) ) {
			Craft();
			return true;
		} else {
			return false;
		}
	}

	bool Workbench::GetAll(Inventory * const from) {
		if ( Inventory::GetAll(from) ) {
			Craft();
			return true;
		} else {
			return false;
		}
	}

	Workbench::Workbench(const int sub) :
			Chest(sub, workbench_size)
	{}

	Workbench::Workbench(QDataStream & str, const int sub) :
			Chest(str, sub, workbench_size)
	{}

//Door::
	Block * Door::DropAfterDamage() const {
		return block_manager.NewBlock(DOOR, Sub());
	}

	int Door::BeforePush(const int dir, Block * const) {
		if ( locked || shifted || dir==World::Anti(GetDir()) )
			return NO_ACTION;
		movable=MOVABLE;
		if ( GetWorld()->Move(x_self, y_self, z_self, GetDir()) )
			shifted=true;
		movable=NOT_MOVABLE;
		return MOVE_SELF;
	}

	void Door::ActFrequent() {
		if ( shifted ) {
			World * const world=GetWorld();
			ushort x, y, z;
			world->Focus(x_self, y_self, z_self, x, y, z,
				World::Anti(GetDir()));
			if ( AIR==world->Sub(x, y, z) ) {
				movable=MOVABLE;
				world->Move(x_self, y_self, z_self,
					World::Anti(GetDir()));
				shifted=false;
				movable=NOT_MOVABLE;
			}
		}
	}
	int Door::ShouldAct() const  { return FREQUENT; }

	int Door::Kind() const { return locked ? LOCKED_DOOR : DOOR; }

	QString & Door::FullName(QString & str) const {
		QString sub_string;
		switch ( Sub() ) {
			case WOOD:  sub_string=" of wood";   break;
			case STONE: sub_string=" of stone";  break;
			case GLASS: sub_string=" of glass";  break;
			case IRON:  sub_string=" of iron";   break;
			default:
				sub_string=" of something";
				fprintf(stderr,
					"Door::FullName: unlisted sub: %d\n",
					Sub());
		}
		return str=QString((locked ? "Locked door" : "Door")) +
			sub_string;
	}

	int Door::Movable() const { return movable; }

	bool Door::ShouldFall() const { return false; }

	usage_types Door::Use() {
		locked=locked ? false : true;
		return NO;
	}

	void Door::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		out << shifted << locked;
	}

	Door::Door(const int sub) :
			Active(sub, ( STONE==sub ) ?
				BLOCK_OPAQUE :
				NONSTANDARD),
			shifted(false),
			locked(false),
			movable(NOT_MOVABLE)
	{}

	Door::Door(QDataStream & str, const int sub) :
			Active(str, sub, NONSTANDARD),
			movable(NOT_MOVABLE)
	{
		str >> shifted >> locked;
	}

//Clock::
	Block * Clock::DropAfterDamage() const {
		return block_manager.NewBlock(CLOCK, Sub());
	}

	usage_types Clock::Use() {
		World * const world=GetWorld();
		SendSignalAround(QString("Time is %1%2%3.").
			arg(world->TimeOfDay()/60).
			arg((world->TimeOfDay()%60 < 10) ? ":0" : ":").
			arg(world->TimeOfDay()%60));
		return NO;
	}

	int Clock::Kind() const { return CLOCK; }

	QString & Clock::FullName(QString & str) const {
		switch ( sub ) {
			case IRON: return str="Iron clock";
			default:
				fprintf(stderr,
					"Clock::FullName: unlisted sub: %d\n",
					sub);
				return str="Strange clock";
		}
	}

	int Clock::BeforePush(const int, Block * const) {
		Use();
		return NO_ACTION;
	}

	ushort Clock::Weight() const { return Block::Weight()/10; }

	bool Clock::ShouldFall() const { return false; }

	int Clock::Movable() const { return NOT_MOVABLE; }

	void Clock::ActRare() {
		if ( alarmTime==GetWorld()->TimeOfDay() ) {
			Use();
		} else if ( timerTime > 0 ) {
			--timerTime;
			note->setNum(timerTime);
		} else if ( timerTime==0 ) {
			Use();
			*note=tr("Timer fired.");
			timerTime=-1;
		}
	}
	int Clock::ShouldAct() const  { return RARE; }

	void Clock::Inscribe(const QString & str) {
		Block::Inscribe(str);
		char c;
		*txtStream >> c;
		if ( 'a'==c ) {
			ushort alarm_hour;
			*txtStream >> alarm_hour;
			*txtStream >> alarmTime;
			alarmTime+=alarm_hour*60;
			timerTime=-1;
		} else if ( 't'==c ) {
			*txtStream >> timerTime;
			alarmTime=-1;
		} else {
			alarmTime=timerTime=-1;
		}
		txtStream->seek(0);
	}

	Clock::Clock(const int sub) :
			Active(sub, NONSTANDARD),
			txtStream(new QTextStream(note)),
			alarmTime(-1),
			timerTime(-1)
	{}

	Clock::Clock (QDataStream & str, const int sub) :
			Active(str, sub, NONSTANDARD),
			txtStream(new QTextStream(note))
	{
		Inscribe(*note);
	}
	Clock::~Clock() { delete txtStream; }
