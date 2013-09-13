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

#include <QDataStream>
#include <QTextStream>
#include <QString>
#include <QFile>
#include "blocks.h"
#include "world.h"
#include "Shred.h"
#include "CraftManager.h"
#include "BlockManager.h"

// Block::
	QString Block::FullName() const {
		switch ( Sub() ) {
		case STAR: case SUN_MOON: case SKY:
		case AIR:        return QObject::tr("Air");
		case WATER:      return QObject::tr("Ice");
		case STONE:      return QObject::tr("Stone");
		case MOSS_STONE: return QObject::tr("Moss stone");
		case NULLSTONE:  return QObject::tr("Nullstone");
		case GLASS:      return QObject::tr("Glass");
		case SOIL:       return QObject::tr("Soil");
		case HAZELNUT:   return QObject::tr("Hazelnut");
		case WOOD:       return QObject::tr("Wood");
		case GREENERY:   return QObject::tr("Leaves");
		case ROSE:       return QObject::tr("Rose");
		case A_MEAT:     return QObject::tr("Animal meat");
		case H_MEAT:     return QObject::tr("Not animal meat");
		case IRON:       return QObject::tr("Iron block");
		case SAND:       return QObject::tr("Sandstone");
		case CLAY:       return QObject::tr("Clay brick");
		default:
			fprintf(stderr, "Block::FullName: unlisted sub: %d.\n",
				Sub());
			return "Unknown block";
		}
	}

	quint8 Block::Transparency(const quint8 transp, const int sub) {
		if ( UNDEF==transp ) {
			switch ( sub ) {
			case AIR:   return INVISIBLE;
			case WATER:
			case GREENERY:
			case GLASS: return BLOCK_TRANSPARENT;
			default:    return BLOCK_OPAQUE;
			}
		} else {
			return transp;
		}
	}

	int Block::Damage(const ushort dmg, const int dmg_kind) {
		ushort mult=1;
		switch ( Sub() ) {
		case DIFFERENT:
			if ( TIME==dmg_kind ) {
				return durability=0;
			}
			// no break
		case NULLSTONE:
		case STAR:
		case AIR:
		case SKY:
		case SUN_MOON: mult=0; break;
		case WATER: mult=( HEAT==dmg_kind || TIME==dmg_kind ); break;
		case MOSS_STONE:
		case STONE:
			switch ( dmg_kind ) {
			case CRUSH: mult=0; break;
			case MINE:  mult=2; break;
			}
			break;
		case GREENERY:
		case GLASS: return durability=0;
		case ROSE:
		case HAZELNUT:
		case WOOD: mult=1+(CUT==dmg_kind); break;
		case SAND:
		case SOIL: mult=1+(DIG==dmg_kind); break;
		case A_MEAT:
		case H_MEAT: mult=1+(THRUST==dmg_kind); break;
		}
		return durability-=mult*dmg;
	}

	Block * Block::DropAfterDamage() const {
		return GLASS==Sub() ?
			0 : BLOCK==Kind() ?
				block_manager.NormalBlock(Sub()) :
				block_manager.NewBlock(Kind(), Sub());
	}

	int  Block::Movable() const {
		return ( AIR==Sub() ) ? ENVIRONMENT : NOT_MOVABLE;
	}

	quint8 Block::Kind() const { return BLOCK; }
	quint16 Block::GetId() const { return id; }
	bool Block::Catchable() const { return false; }
	int  Block::BeforePush(const int, Block * const) { return NO_ACTION; }
	bool Block::Move(const int) { return false; }
	usage_types Block::Use(Block *) { return USAGE_TYPE_NO; }
	int  Block::Wearable() const { return WEARABLE_NOWHERE; }
	int  Block::DamageKind() const { return CRUSH; }
	ushort Block::DamageLevel() const { return 1; }
	uchar Block::LightRadius() const { return 0; }
	void Block::ReceiveSignal(const QString &) {}

	bool Block::Inscribe(const QString & str) {
		if ( note ) {
			*note=str.left(MAX_NOTE_LENGTH);
		} else {
			note=new QString(str.left(MAX_NOTE_LENGTH));
		}
		if ( ""==*note ) {
			delete note;
			note=0;
		}
		return true;
	}

	Inventory * Block::HasInventory() { return 0; }
	Animal * Block::IsAnimal() { return 0; }
	Active * Block::ActiveBlock() { return 0; }

	void Block::Restore() { durability=MAX_DURABILITY; }
	int  Block::GetDir() const { return direction; }
	int  Block::Sub() const { return sub; }
	int  Block::Transparent() const { return transparent; }
	short Block::Durability() const { return durability; }
	QString Block::GetNote() const { return note ? *note : ""; }

	int Block::Temperature() const {
		switch (sub) {
		case WATER: return -100;
		default:    return 0;
		}
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
		case PAPER:
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

	void Block::SetDir(const int dir) {
		if ( BLOCK!=Kind() || WOOD==Sub() ) {
			direction=dir;
		}
	}

	bool Block::operator==(const Block & block) const {
		return ( block.Kind()==Kind() &&
			block.Sub()==Sub() &&
			block.GetDir()==GetDir() &&
			block.Durability()==Durability() &&
			( (!note && !block.note) ||
				(note && block.note && *block.note==*note) ) );
	}

	void Block::SaveAttributes(QDataStream &) const {}
	void Block::SaveToFile(QDataStream & out) const {
		if ( this==block_manager.NormalBlock(sub) ) {
			out << quint8( 0x80 | sub );
		} else {
			quint16 data=direction;
			out << sub << Kind() << ( ( ( ( data
				<<= 7 ) |= durability )
				<<= 1 ) |= !!note );
			if ( Q_UNLIKELY(note) ) {
				out << *note;
			}
			SaveAttributes(out);
		}
	}

	Block::Block(const int subst, const quint16 i,
			const quint8 transp)
		:
			transparent(Transparency(transp, subst)),
			sub(subst),
			id(i),
			direction(UP),
			note(0),
			durability(MAX_DURABILITY)
	{}
	Block::Block(QDataStream & str, const int subst, const quint16 i,
			const quint8 transp)
		:
			transparent(Transparency(transp, subst)),
			sub(subst),
			id(i),
			note(0)
	{
		quint16 data;
		str >> data;
		if ( Q_UNLIKELY(data & 1) ) {
			str >> *(note = new QString);
		}
		durability = ( data >>=1 ) & 0x7F;
		direction = (data >>= 7);
	}
	Block::~Block() { delete note; }
// Plate::
	QString Plate::FullName() const {
		switch ( Sub() ) {
		case WOOD:  return QObject::tr("Wooden board");
		case IRON:  return QObject::tr("Iron plate");
		case STONE: return QObject::tr("Stone slab");
		default:
			fprintf(stderr, "Plate::FullName: unlisted sub: %d",
				Sub());
			return "Strange plate";
		}
	}

	quint8 Plate::Kind() const { return PLATE; }
	int Plate::BeforePush(const int, Block * const) { return JUMP; }
	ushort Plate::Weight() const { return Block::Weight()/4; }

	Plate::Plate(const int sub, const quint16 id) :
			Block(sub, id, NONSTANDARD)
	{}
	Plate::Plate(QDataStream & str, const int sub, const quint16 id) :
			Block(str, sub, id, NONSTANDARD)
	{}
// Ladder::
	QString Ladder::FullName() const {
		switch ( Sub() ) {
		case WOOD:  return QObject::tr("Ladder");
		case STONE: return QObject::tr("Rock with ledges");
		default:
			fprintf(stderr, "Ladder::FullName: unlisted sub: %d\n",
				Sub());
			return "Strange ladder";
	}
	}

	quint8 Ladder::Kind() const { return LADDER; }
	int  Ladder::BeforePush(const int, Block * const) { return MOVE_UP; }
	bool Ladder::Catchable() const { return true; }
	ushort Ladder::Weight() const { return Block::Weight()*3; }

	Block * Ladder::DropAfterDamage() const {
		return ( STONE==Sub() ) ?
			block_manager.NormalBlock(STONE) :
			block_manager.NewBlock(LADDER, Sub());
	}

	Ladder::Ladder(const int sub, const quint16 id) :
			Block(sub, id, NONSTANDARD)
	{}
	Ladder::Ladder(QDataStream & str, const int sub, const quint16 id) :
			Block(str, sub, id, NONSTANDARD)
	{}
// Weapon::
	QString Weapon::FullName() const {
		switch ( Sub() ) {
		case STONE: return QObject::tr("Pebble");
		case IRON:  return QObject::tr("Spike");
		case WOOD:  return QObject::tr("Stick");
		default:
			fprintf(stderr, "Weapon::FullName: unlisted sub: %d\n",
				Sub());
			return "Some weapon";
		}
	}

	quint8 Weapon::Kind() const { return WEAPON; }
	ushort Weapon::Weight() const { return Block::Weight()/4; }
	int    Weapon::Wearable() const { return WEARABLE_ARM; }

	ushort Weapon::DamageLevel() const {
		switch ( Sub() ) {
		case WOOD: return 4;
		case IRON: return 6;
		case STONE: return 5;
		default:
			fprintf(stderr, "Weapon::DamageLevel: sub (?): %d\n.",
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

	Weapon::Weapon(const int sub, const quint16 id) :
			Block(sub, id, NONSTANDARD)
	{}
	Weapon::Weapon(QDataStream & str, const int sub, const quint16 id) :
			Block(str, sub, id, NONSTANDARD)
	{}
// Pick::
	quint8 Pick::Kind() const { return PICK; }
	int Pick::DamageKind() const { return MINE; }

	ushort Pick::DamageLevel() const {
		switch ( Sub() ) {
		case IRON: return 10;
		default:
			fprintf(stderr, "Pick::DamageLevel: sub (?): %d\n.",
				Sub());
			return 1;
		}
	}

	QString Pick::FullName() const {
		switch ( Sub() ) {
		case IRON: return QObject::tr("Iron pick");
		default:
			fprintf(stderr, "Pick::FullName: unknown sub: %d\n",
				Sub());
			return "Strange pick";
		}
	}

	Pick::Pick(const int sub, const quint16 id) :
			Weapon(sub, id)
	{}
	Pick::Pick(QDataStream & str, const int sub, const quint16 id) :
			Weapon(str, sub, id)
	{}
// Animal::
	void Animal::DoRareAction() {
		World * const world = GetWorld();
		if (
				AIR != world->Sub(X(), Y(), Z()+1) &&
				AIR != world->Sub(X(), Y(), Z()-1) &&
					world->InBounds(X()+1, Y()) &&
				AIR != world->Sub(X()+1, Y(), Z()) &&
					world->InBounds(X()-1, Y()) &&
				AIR != world->Sub(X()-1, Y(), Z()) &&
					world->InBounds(X(), Y()+1) &&
				AIR != world->Sub(X(), Y()+1, Z()) &&
					world->InBounds(X(), Y()-1) &&
				AIR != world->Sub(X(), Y()-1, Z()) )
		{
			if ( breath <= 0 ) {
				world->Damage(X(), Y(), Z(), 10, BREATH);
			} else {
				--breath;
			}
		} else if ( breath < MAX_BREATH ) {
			++breath;
		}
		if ( satiation <= 0 ) {
			world->Damage(X(), Y(), Z(), 5, HUNGER);
		} else {
			--satiation;
		}
		if ( durability < MAX_DURABILITY ) {
			++durability;
		}
		emit Updated();
	}
	int Animal::ShouldAct() const { return FREQUENT_AND_RARE; }

	ushort Animal::Breath() const { return breath; }
	ushort Animal::Satiation() const { return satiation; }
	Animal * Animal::IsAnimal() { return this; }

	bool Animal::Eat(const int sub) {
		const int value = NutritionalValue(sub);
		if ( value ) {
			satiation += value;
			ReceiveSignal(tr("Yum!"));
			if ( SECONDS_IN_DAY < satiation ) {
				satiation=1.1*SECONDS_IN_DAY;
				ReceiveSignal(tr("You have gorged yourself!"));
			}
			return true;
		} else {
			ReceiveSignal(tr("You cannot eat this."));
			return false;
		}
	}

	void Animal::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		out << breath << satiation;
	}

	Animal::Animal(const int sub, const quint16 id) :
			Active(sub, id, NONSTANDARD),
			breath(MAX_BREATH),
			satiation(SECONDS_IN_DAY)
	{}
	Animal::Animal(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id, NONSTANDARD)
	{
		str >> breath >> satiation;
	}
// Inventory::
	bool   Inventory::Access() const { return true; }
	ushort Inventory::Start() const { return 0; }
	ushort Inventory::Size() const { return size; }
	Inventory * Inventory::HasInventory() { return this; }

	bool Inventory::Drop(const ushort src, ushort dest,
			const ushort num,
			Inventory * const inv_to)
	{
		if ( dest<inv_to->Start() ) {
			dest=inv_to->Start();
		}
		bool ok_flag=false;
		for (ushort i=0; i<num; ++i) {
			if ( src<Size() &&
					dest<inv_to->Size() &&
					!inventory[src].isEmpty() &&
					inv_to->Get(inventory[src].top(),
						dest) )
			{
				ok_flag=true;
			}
			Pull(src);
		}
		return ok_flag;
	}

	bool Inventory::GetAll(Inventory * const from) {
		bool flag=false;
		for (ushort i=0; i<from->Size(); ++i) {
			if ( from->Drop(i, 0, from->Number(i), this) ) {
				flag=true;
			}
		}
		return flag;
	}

	void Inventory::Pull(const ushort num) {
		if ( !inventory[num].isEmpty() ) {
			inventory[num].pop();
		}
	}

	void Inventory::SaveAttributes(QDataStream & out) const {
		for (ushort i=0; i<Size(); ++i) {
			out << Number(i);
			for (ushort j=0; j<Number(i); ++j) {
				inventory[i].top()->SaveToFile(out);
			}
		}
	}

	bool Inventory::Get(Block * const block, ushort start) {
		if ( start<Start() ) {
			start=Start();
		}
		if ( block ) {
			for (ushort i=start; i<Size(); ++i) {
				if ( GetExact(block, i) ) {
					return true;
				}
			}
			return false;
		} else {
			return true;
		}
	}

	bool Inventory::GetExact(Block * const block, const ushort num) {
		if ( block ) {
			if ( inventory[num].isEmpty() ||
					( *block==*inventory[num].top() &&
					Number(num)<MAX_STACK_SIZE ) )
			{
				inventory[num].push(block);
				return true;
			}
			return false;
		} else {
			return true;
		}
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

	bool Inventory::InscribeInv(const ushort num, const QString & str) {
		const int number=Number(num);
		if ( !number ) {
			ReceiveSignal(QObject::tr("Nothing here."));
			return false;
		}
		const int sub=inventory[num].top()->Sub();
		if ( inventory[num].top()==block_manager.NormalBlock(sub) ) {
			for (ushort i=0; i<number; ++i) {
				inventory[num].replace(i,
					block_manager.NormalBlock(sub));
			}
		}
		for (ushort i=0; i<number; ++i) {
			if ( !inventory[num].at(i)->Inscribe(str) ) {
				ReceiveSignal(QObject::tr(
					"Cannot inscribe this."));
				return false;
			}
		}
		ReceiveSignal(QObject::tr("Inscribed."));
		return true;
	}

	QString Inventory::InvFullName(const ushort num) const {
		return inventory[num].isEmpty() ?
			"" : inventory[num].top()->FullName();
	}

	QString Inventory::NumStr(const ushort num) const {
		return QString(" (%1x)").arg(Number(num));
	}

	ushort Inventory::GetInvWeight(const ushort i) const {
		return inventory[i].isEmpty() ?
			0 : inventory[i].top()->Weight()*Number(i);
	}

	int Inventory::GetInvSub(const ushort i) const {
		return inventory[i].isEmpty() ?
			AIR : inventory[i].top()->Sub();
	}

	int Inventory::GetInvKind(const ushort i) const {
		return inventory[i].isEmpty() ?
			BLOCK : int(inventory[i].top()->Kind());
	}

	QString Inventory::GetInvNote(const ushort num) const {
		return inventory[num].top()->GetNote();
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

	bool Inventory::MiniCraft(const ushort num) {
		const ushort size=inventory[num].size();
		if ( !size ) {
			ReceiveSignal(QObject::tr("Nothing here"));
			return false;
		}
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
			for (ushort i=0; i<result.num; ++i) {
				Get(block_manager.
					NewBlock(result.kind, result.sub));
			}
			ReceiveSignal(QObject::tr("Craft successful."));
			return true;
		}
		ReceiveSignal(QObject::tr(
			"You don't know how to craft this."));
		return false; // no such recipe
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
			while ( num-- ) {
				inventory[i].push(block_manager.
					BlockFromFile(str));
			}
		}
	}
	Inventory::~Inventory() {
		for (ushort i=0; i<Size(); ++i) {
			while ( !inventory[i].isEmpty() ) {
				Block * const block=inventory[i].pop();
				block_manager.DeleteBlock(block);
			}
		}
		delete [] inventory;
	}
// Chest::
	quint8 Chest::Kind() const { return CHEST; }
	int Chest::Sub() const { return Block::Sub(); }
	Inventory * Chest::HasInventory() { return Inventory::HasInventory(); }
	usage_types Chest::Use(Block *) { return USAGE_TYPE_OPEN; }

	void Chest::ReceiveSignal(const QString & str) {
		Block::ReceiveSignal(str);
	}

	QString Chest::FullName() const {
		switch ( Sub() ) {
		case WOOD:  return QObject::tr("Wooden chest");
		case STONE: return QObject::tr("Stone chest");
		default:
			fprintf(stderr, "Chest::FullName: unlisted sub: %d\n",
				Sub());
			return QObject::tr("Chest");
		}
	}

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

	Chest::Chest(const int sub, const quint16 id, const ushort size) :
			Block(sub, id),
			Inventory(size)
	{}
	Chest::Chest(QDataStream & str, const int sub, const quint16 id,
			const ushort size)
		:
			Block(str, sub, id),
			Inventory(str, size)
	{}
// Pile::
	int Pile::BeforePush(const int, Block * const who) {
		Inventory::BeforePush(who);
		return NO_ACTION;
	}

	void Pile::DoRareAction() {
		if ( IsEmpty() ) {
			Damage(Durability(), TIME);
		}
	}

	int Pile::ShouldAct() const { return RARE; }
	quint8 Pile::Kind() const { return PILE; }
	int Pile::Sub() const { return Block::Sub(); }
	Inventory * Pile::HasInventory() { return Inventory::HasInventory(); }
	usage_types Pile::Use(Block *) { return USAGE_TYPE_OPEN; }
	ushort Pile::Weight() const { return Inventory::Weight(); }
	Block * Pile::DropAfterDamage() const { return 0; }

	void Pile::ReceiveSignal(const QString & str) {
		Active::ReceiveSignal(str);
	}

	QString Pile::FullName() const {
		switch ( Sub() ) {
		case DIFFERENT: return tr("Pile");
		default:
			fprintf(stderr, "Pile::FullName: unlisted sub: %d\n",
				Sub());
			return tr("Unknown pile");
		}
	}

	void Pile::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		Inventory::SaveAttributes(out);
	}

	Pile::Pile(const int sub, const quint16 id) :
			Active(sub, id, NONSTANDARD),
			Inventory(INV_SIZE)
	{}
	Pile::Pile(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id, NONSTANDARD),
			Inventory(str, INV_SIZE)
	{}
// Liquid::
	bool Liquid::CheckWater() const {
		World * const world=GetWorld();
		return (
			WATER==world->Sub(X(), Y(), Z()-1) ||
			WATER==world->Sub(X(), Y(), Z()+1) ||
			(world->InBounds(X()-1, Y()) &&
				WATER==world->Sub(X()-1, Y(), Z())) ||
			(world->InBounds(X()+1, Y()) &&
				WATER==world->Sub(X()+1, Y(), Z())) ||
			(world->InBounds(X(), Y()-1) &&
				WATER==world->Sub(X(), Y()-1, Z())) ||
			(world->InBounds(X(), Y()+1) &&
				WATER==world->Sub(X(), Y()+1, Z())) );
	}

	void Liquid::DoRareAction() {
		World * const world=GetWorld();
		// IDEA: turn off water drying up in ocean
		if ( WATER==Sub() && !CheckWater() ) {
			world->Damage(X(), Y(), Z(), 1, HEAT);
		}
		switch ( qrand()%20 ) {
		case 0: world->Move(X(), Y(), Z(), NORTH); break;
		case 1: world->Move(X(), Y(), Z(), EAST);  break;
		case 2:	world->Move(X(), Y(), Z(), SOUTH); break;
		case 3: world->Move(X(), Y(), Z(), WEST);  break;
		}
	}

	int Liquid::ShouldAct() const  { return RARE; }
	int Liquid::Movable() const { return ENVIRONMENT; }
	quint8 Liquid::Kind() const { return LIQUID; }
	int Liquid::Temperature() const { return ( WATER==Sub() ) ? 0 : 1000; }
	uchar Liquid::LightRadius() const { return ( WATER==Sub() ) ? 0 : 3; }
	Block * Liquid::DropAfterDamage() const { return 0; }

	QString Liquid::FullName() const {
		switch ( Sub() ) {
		case WATER: return tr("Water");
		case STONE: return tr("Lava");
		default:
			fprintf(stderr, "Liquid::FullName(): sub (?): %d\n",
				Sub());
			return "Unknown liquid";
		}
	}

	Liquid::Liquid(const int sub, const quint16 id) :
			Active(sub, id)
	{}
	Liquid::Liquid(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id)
	{}
// Grass::
	void Grass::DoRareAction() {
		World * const world = GetWorld();
		if ( SOIL != world->Sub(X(), Y(), Z()-1) ) {
			world->Damage(X(), Y(), Z(), durability, TIME);
		}
		short i=X(), j=Y();
		// increase this if grass grows too fast
		switch ( qrand() % (SECONDS_IN_HOUR*2) ) {
		case 0: ++i; break;
		case 1: --i; break;
		case 2: ++j; break;
		case 3: --j; break;
		default: return;
		}
		if ( world->InBounds(i, j) && world->Enlightened(i, j, Z()) ) {
			if ( AIR==world->Sub(i, j, Z()) &&
					SOIL==world->Sub(i, j, Z()-1) )
			{
				world->Build(block_manager.NewBlock(GRASS,
						Sub()), i, j, Z());
			} else if ( SOIL==world->Sub(i, j, Z()) &&
					AIR==world->Sub(i, j, Z()+1) )
			{
				world->Build(block_manager.NewBlock(GRASS,
						Sub()), i, j, Z()+1);
			}
		}
	}

	QString Grass::FullName() const {
		switch ( Sub() ) {
		case GREENERY: return tr("Grass");
		default:
			fprintf(stderr, "Grass::FullName(): sub (?): %d\n",
				Sub());
			return "Unknown plant";
		}
	}

	int  Grass::ShouldAct() const  { return RARE; }
	int  Grass::BeforePush(const int, Block * const) { return DESTROY; }
	bool Grass::ShouldFall() const { return false; }
	quint8 Grass::Kind() const { return GRASS; }
	Block * Grass::DropAfterDamage() const { return 0; }

	Grass::Grass(const int sub, const quint16 id) :
			Active(sub, id)
	{}
	Grass::Grass(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id)
	{}
// Bush::
	QString Bush::FullName() const { return tr("Bush"); }
	quint8 Bush::Kind() const { return BUSH; }
	int  Bush::Sub() const { return Block::Sub(); }
	int  Bush::Movable() const { return NOT_MOVABLE; }
	bool Bush::ShouldFall() const { return false; }
	int  Bush::ShouldAct() const  { return RARE; }
	usage_types Bush::Use(Block *) { return USAGE_TYPE_OPEN; }
	Inventory * Bush::HasInventory() { return Inventory::HasInventory(); }

	void Bush::ReceiveSignal(const QString & str) {
		Active::ReceiveSignal(str);
	}

	ushort Bush::Weight() const {
		return Inventory::Weight()+Block::Weight();
	}

	void Bush::DoRareAction() {
		if ( 0==qrand()%(SECONDS_IN_HOUR*4) ) {
			Get(block_manager.NormalBlock(HAZELNUT));
		}
	}

	int Bush::BeforePush(const int, Block * const who) {
		Inventory::BeforePush(who);
		return NO_ACTION;
	}

	Block * Bush::DropAfterDamage() const {
		Block * const pile=block_manager.NewBlock(PILE, DIFFERENT);
		pile->HasInventory()->Get(block_manager.NormalBlock(WOOD));
		pile->HasInventory()->Get(block_manager.NormalBlock(HAZELNUT));
		return pile;
	}

	void Bush::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		Inventory::SaveAttributes(out);
	}

	Bush::Bush(const int sub, const quint16 id) :
			Active(sub, id),
			Inventory(BUSH_SIZE)
	{}
	Bush::Bush(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id),
			Inventory(str, BUSH_SIZE)
	{}
// Rabbit::
	short Rabbit::Attractive(const int sub) const {
		switch ( sub ) {
		case H_MEAT:   return -8;
		case A_MEAT:   return -1;
		case GREENERY: return  1;
		case SAND:     return -1;
		default:       return  0;
		}
	}

	void Rabbit::DoFrequentAction() {
		World * const world = GetWorld();
		// analyse world around
		short for_north = 0, for_west = 0;
		for (ushort x=X()-4; x<=X()+4; ++x)
		for (ushort y=Y()-4; y<=Y()+4; ++y)
		for (ushort z=Z()-1; z<=Z()+3; ++z) {
			short attractive;
			if ( world->InBounds(x, y, z) &&
					(attractive = Attractive(
						world->Sub(x, y, z))) &&
					world->DirectlyVisible(
						X(), Y(), Z(), x, y, z) )
			{
				if ( y!=Y() ) for_north += attractive/(Y()-y);
				if ( x!=X() ) for_west  += attractive/(X()-x);
			}
		}
		// make direction and move there
		static const ushort calmness = 5;
		if ( qAbs(for_north)>calmness || qAbs(for_west)>calmness ) {
			SetDir( ( qAbs(for_north)>qAbs(for_west) ) ?
				( ( for_north>0 ) ? NORTH : SOUTH ) :
				( ( for_west >0 ) ? WEST  : EAST  ) );
			if ( qrand()%2 ) {
				world->Move(X(), Y(), Z(), GetDir());
			} else {
				world->Jump(X(), Y(), Z(), GetDir());
			}
		}
	} // void Rabbit::DoFrequentAction()

	void Rabbit::DoRareAction() {
		Animal::DoRareAction();
		// eat sometimes
		World * const world = GetWorld();
		if ( SECONDS_IN_DAY/2 > Satiation() ) {
			for (ushort x=X()-1; x<=X()+1; ++x)
			for (ushort y=Y()-1; y<=Y()+1; ++y)
			for (ushort z=Z();   z<=Z()+1; ++z) {
				if ( world->InBounds(x, y) &&
						GREENERY==world->Sub(x, y, z) )
				{
					world->Damage(x, y, z, MAX_DURABILITY,
						EATEN);
					world->DestroyAndReplace(x, y, z);
					Eat(GREENERY);
					return;
				}
			}
		}
		// random movement
		switch ( qrand()%60 ) {
		case 0: SetDir(NORTH); break;
		case 1: SetDir(SOUTH); break;
		case 2: SetDir(EAST);  break;
		case 3: SetDir(WEST);  break;
		default: return;
		}
		world->Move(X(), Y(), Z(), GetDir());
	}

	Block * Rabbit::DropAfterDamage() const {
		return block_manager.NormalBlock(A_MEAT);
	}

	QString Rabbit::FullName() const { return tr("Herbivore"); }
	int Rabbit::ShouldAct() const { return FREQUENT_AND_RARE; }
	quint8 Rabbit::Kind() const { return RABBIT; }

	quint16 Rabbit::NutritionalValue(const int sub) const {
		return ( GREENERY == sub ) ? SECONDS_IN_HOUR*4 : 0;
	}

	Rabbit::Rabbit(const int sub, const quint16 id) :
			Animal(sub, id)
	{}
	Rabbit::Rabbit(QDataStream & str, const int sub, const quint16 id) :
			Animal(str, sub, id)
	{}
// Workbench::
	void Workbench::Craft() {
		while ( Number(0) ) { // remove previous product
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

	QString Workbench::FullName() const {
		switch ( Sub() ) {
		case WOOD: return QObject::tr("Workbench");
		case IRON: return QObject::tr("Iron anvil");
		default:
			fprintf(stderr, "Workbench::FullName: sub (?): %d\n",
				Sub());
			return "Strange workbench";
		}
	}

	quint8 Workbench::Kind() const { return WORKBENCH; }
	ushort Workbench::Start() const { return 1; }

	void Workbench::ReceiveSignal(const QString & str) {
		Block::ReceiveSignal(str);
	}

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

	Workbench::Workbench(const int sub, const quint16 id) :
			Chest(sub, id, WORKBENCH_SIZE)
	{}
	Workbench::Workbench(QDataStream & str, const int sub,
			const quint16 id)
		:
			Chest(str, sub, id, WORKBENCH_SIZE)
	{}
// Door::
	int Door::BeforePush(const int dir, Block * const) {
		if ( locked || shifted || dir==World::Anti(GetDir()) ) {
			return NO_ACTION;
		}
		movable=MOVABLE;
		if ( GetWorld()->Move(X(), Y(), Z(), GetDir()) ) {
			shifted=true;
		}
		movable=NOT_MOVABLE;
		return MOVE_SELF;
	}

	void Door::DoFrequentAction() {
		if ( shifted ) {
			World * const world=GetWorld();
			ushort x, y, z;
			world->Focus(X(), Y(), Z(), x, y, z,
				World::Anti(GetDir()));
			if ( AIR==world->Sub(x, y, z) ) {
				movable=MOVABLE;
				world->Move(X(), Y(), Z(),
					World::Anti(GetDir()));
				shifted=false;
				movable=NOT_MOVABLE;
			}
		}
	}

	int  Door::ShouldAct() const  { return FREQUENT; }
	quint8 Door::Kind() const { return locked ? LOCKED_DOOR : DOOR; }
	int  Door::Movable() const { return movable; }
	bool Door::ShouldFall() const { return false; }

	QString Door::FullName() const {
		QString sub_string;
		switch ( Sub() ) {
		case WOOD:  sub_string=tr(" of wood");  break;
		case STONE: sub_string=tr(" of stone"); break;
		case GLASS: sub_string=tr(" of glass"); break;
		case IRON:  sub_string=tr(" of iron");  break;
		default:
			sub_string=tr(" of something");
			fprintf(stderr, "Door::FullName: unlisted sub: %d\n",
				Sub());
		}
		return tr(locked ? "Locked door" : "Door") + sub_string;
	}

	usage_types Door::Use(Block *) {
		locked=!locked;
		return USAGE_TYPE_NO;
	}

	void Door::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		out << shifted << locked;
	}

	Door::Door(const int sub, const quint16 id) :
			Active(sub, id, ( STONE==sub ) ?
				BLOCK_OPAQUE : NONSTANDARD),
			shifted(false),
			locked(false),
			movable(NOT_MOVABLE)
	{}
	Door::Door(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id, ( STONE==sub ) ?
				BLOCK_OPAQUE : NONSTANDARD),
			movable(NOT_MOVABLE)
	{
		str >> shifted >> locked;
	}
// Clock::
	usage_types Clock::Use(Block * const who) {
		World * world=GetWorld();
		if ( world ) {
			SendSignalAround(world->TimeOfDayStr());
		} else if ( who ) {
			const Active * const active=who->ActiveBlock();
			if ( active && (world=active->GetWorld()) ) {
				who->ReceiveSignal(world->TimeOfDayStr());
			}
		}
		return USAGE_TYPE_NO;
	}

	QString Clock::FullName() const {
		switch ( Sub() ) {
		case IRON: return tr("Iron clock");
		default:
			fprintf(stderr, "Clock::FullName: unlisted sub: %d\n",
				Sub());
			return "Strange clock";
		}
	}

	int Clock::BeforePush(const int, Block * const) {
		Use();
		return NO_ACTION;
	}

	quint8 Clock::Kind() const { return CLOCK; }
	ushort Clock::Weight() const { return Block::Weight()/10; }
	bool Clock::ShouldFall() const { return false; }
	int Clock::Movable() const { return NOT_MOVABLE; }
	int Clock::ShouldAct() const  { return RARE; }

	void Clock::DoRareAction() {
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

	bool Clock::Inscribe(const QString & str) {
		Block::Inscribe(str);
		char c;
		QTextStream txt_stream(note);
		txt_stream >> c;
		if ( 'a'==c ) {
			ushort alarm_hour;
			txt_stream >> alarm_hour;
			txt_stream >> alarmTime;
			alarmTime+=alarm_hour*60;
			timerTime=-1;
		} else if ( 't'==c ) {
			txt_stream >> timerTime;
			alarmTime=-1;
		} else {
			alarmTime=timerTime=-1;
		}
		return true;
	}

	Clock::Clock(const int sub, const quint16 id) :
			Active(sub, id, NONSTANDARD),
			alarmTime(-1),
			timerTime(-1)
	{}
	Clock::Clock (QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id, NONSTANDARD),
			alarmTime(-1),
			timerTime(-1)
	{
		if ( note ) {
			Inscribe(*note);
		}
	}
// Creator::
	quint8 Creator::Kind() const { return CREATOR; }
	int Creator::Sub() const { return Block::Sub(); }
	QString Creator::FullName() const { return tr("Creative block"); }
	int Creator::DamageKind() const { return TIME; }
	ushort Creator::DamageLevel() const { return MAX_DURABILITY; }
	Inventory * Creator::HasInventory() {
		return Inventory::HasInventory();
	}

	void Creator::ReceiveSignal(const QString & str) {
		Active::ReceiveSignal(str);
	}

	void Creator::SaveAttributes(QDataStream & out) const {
		Active::SaveAttributes(out);
		Inventory::SaveAttributes(out);
	}

	Creator::Creator(const int sub, const quint16 id) :
			Active(sub, id, NONSTANDARD),
			Inventory(INV_SIZE)
	{}
	Creator::Creator(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id, NONSTANDARD),
			Inventory(str, INV_SIZE)
	{}
// Text::
	quint8 Text::Kind() const { return TEXT; }
	QString Text::FullName() const { return QObject::tr("Paper page"); }

	usage_types Text::Use(Block * const who) {
		if ( note ) {
			return USAGE_TYPE_READ;
		} else {
			who->ReceiveSignal(QObject::tr(
				"Nothing is written on this page"));
			return USAGE_TYPE_NO;
		}
	}

	bool Text::Inscribe(const QString & str) {
		if ( '.'!=str.at(0) && !note ) {
			Block::Inscribe(str);
			return true;
		} else {
			return false;
		}
	}

	Text::Text(const int sub, const quint16 id) :
			Block(sub, id, NONSTANDARD)
	{}
	Text::Text(QDataStream & str, const int sub, const quint16 id) :
			Block(str, sub, id, NONSTANDARD)
	{}
// Map::
	quint8 Map::Kind() const { return MAP; }
	QString Map::FullName() const { return QObject::tr("Map"); }

	usage_types Map::Use(Block * const who) {
		if ( !note ) {
			if ( who ) {
				who->ReceiveSignal(QObject::tr(
					"Set title to this map first."));
			}
			return USAGE_TYPE_NO;
		} else if ( who && who->ActiveBlock() ) {
			const Active * const active=who->ActiveBlock();
			QFile map_file(active->GetWorld()->
				WorldName() + "/texts/" + *note + ".txt");
			const long  lati=active->GetShred()->Latitude();
			const long longi=active->GetShred()->Longitude();
			if ( !map_file.open(QIODevice::ReadWrite |
					QIODevice::Text) )
			{
				return USAGE_TYPE_READ;
			}
			World * const world=active->GetWorld();
			static const ushort FILE_SIZE_CHARS=31;
			if ( 0==map_file.size() ) { // new map
				map_file.putChar('+');
				for (ushort i=0; i<FILE_SIZE_CHARS-2; ++i) {
					map_file.putChar('-');
				}
				map_file.putChar('+');
				map_file.putChar('\n');
				for (ushort i=0; i<FILE_SIZE_CHARS-2; ++i) {
					map_file.putChar('|');
					for (ushort j=0; j<FILE_SIZE_CHARS-2;
							++j)
					{
						map_file.putChar(' ');
					}
					map_file.putChar('|');
					map_file.putChar('\n');
				}
				map_file.putChar('+');
				for (ushort i=0; i<FILE_SIZE_CHARS-2; ++i) {
					map_file.putChar('-');
				}
				map_file.putChar('+');

				map_file.seek(FILE_SIZE_CHARS/2);
				map_file.putChar('+');
				map_file.seek(FILE_SIZE_CHARS/2*
					(FILE_SIZE_CHARS+1));
				map_file.putChar('+');
				map_file.seek(FILE_SIZE_CHARS/2*
					(FILE_SIZE_CHARS+1)+FILE_SIZE_CHARS-1);
				map_file.putChar('+');
				map_file.seek((FILE_SIZE_CHARS-1)*
					(FILE_SIZE_CHARS+1)+FILE_SIZE_CHARS/2);
				map_file.putChar('+');

				longiStart=longi;
				latiStart=lati;
			}
			if (
					( abs( lati-latiStart ) >
						FILE_SIZE_CHARS/2 ) ||
					( abs(longi-longiStart) >
						FILE_SIZE_CHARS/2 ) )
			{
				return USAGE_TYPE_READ;
			}
			if ( savedChar ) {
				map_file.seek(savedShift);
				map_file.putChar(savedChar);
			}
			map_file.seek(savedShift=(FILE_SIZE_CHARS+1)*
				(longi-longiStart+FILE_SIZE_CHARS/2)+
				 lati-latiStart+FILE_SIZE_CHARS/2);
			map_file.putChar('@');
			savedChar=world->TypeOfShred(longi, lati);
			map_file.seek((FILE_SIZE_CHARS+1)*FILE_SIZE_CHARS-1);
			map_file.putChar('\n');
			map_file.putChar('@');
			map_file.putChar('=');
			map_file.putChar(savedChar);
			map_file.putChar('\n');
		}
		return USAGE_TYPE_READ;
	} // usage_types Map::Use(Block * who)

	void Map::SaveAttributes(QDataStream & out) const {
		out << longiStart << latiStart << savedShift << savedChar;
	}

	Map::Map(const int sub, const quint16 id) :
			Text(sub, id),
			longiStart(),
			latiStart(),
			savedShift(),
			savedChar(0)
	{}
	Map::Map(QDataStream & str, const int sub, const quint16 id) :
			Text(str, sub, id)
	{
		str >> longiStart >> latiStart >> savedShift >> savedChar;
	}
// Bell:
	quint8 Bell::Kind() const { return BELL; }
	QString Bell::FullName() const { return tr("Bell."); }

	usage_types Bell::Use(Block  * const) {
		SendSignalAround(SOUND_STRINGS[0]); // "Ding!"
		return USAGE_TYPE_NO;
	}

	void Bell::ReceiveSignal(const QString & str) {
		if ( SOUND_STRINGS[0]!=str ) {
			SendSignalAround(SOUND_STRINGS[0]);
		}
	}

	Bell::Bell(const int sub, const quint16 id) :
			Active(sub, id)
	{}
	Bell::Bell(QDataStream & str, const int sub, const quint16 id) :
			Active(str, sub, id)
	{}
