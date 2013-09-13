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

#include "Dwarf.h"
#include "world.h"
#include "BlockManager.h"
#include <QDataStream>

uchar Dwarf::GetActiveHand() const { return activeHand; }
void  Dwarf::SetActiveHand(const bool right) {
	activeHand=(right ? quint8(IN_RIGHT) : IN_LEFT);
}

ushort Dwarf::Weight() const {
	World * const world = GetWorld();
	return ( world && (
			(world->InBounds(X()+1, Y()) &&
				world->GetBlock(X()+1, Y(), Z())->
					Catchable()) ||
			(world->InBounds(X()-1, Y()) &&
				world->GetBlock(X()-1, Y(), Z())->
					Catchable()) ||
			(world->InBounds(X(), Y()+1) &&
				world->GetBlock(X(), Y()+1, Z())->
					Catchable()) ||
			(world->InBounds(X(), Y()-1) &&
				world->GetBlock(X(), Y()-1, Z())->
					Catchable()) ) ) ?
		0 : Inventory::Weight()+Block::Weight();
}

Block * Dwarf::DropAfterDamage() const {
	return block_manager.NormalBlock(H_MEAT);
}

quint8 Dwarf::Kind() const { return DWARF; }
int  Dwarf::Sub() const { return Block::Sub(); }
bool Dwarf::Access() const { return false; }
ushort Dwarf::Start() const { return ON_LEGS+1; }
QString Dwarf::FullName() const { return "Rational"; }
Inventory * Dwarf::HasInventory() { return Inventory::HasInventory(); }
uchar Dwarf::LightRadius() const { return lightRadius; }

void Dwarf::UpdateLightRadius() {
	Block * const in_left  = ShowBlock(IN_LEFT);
	Block * const in_right = ShowBlock(IN_RIGHT);
	const uchar  left_rad = in_left  ? in_left ->LightRadius() : 0;
	const uchar right_rad = in_right ? in_right->LightRadius() : 0;
	lightRadius = qMax(uchar(MIN_DWARF_LIGHT_RADIUS),
		qMax(left_rad, right_rad));
}

void Dwarf::ReceiveSignal(const QString & str) { Active::ReceiveSignal(str); }

int Dwarf::DamageKind() const {
	return ( Number(GetActiveHand()) ) ?
		ShowBlock(GetActiveHand())->DamageKind() : CRUSH;
}

ushort Dwarf::DamageLevel() const {
	ushort level = 1;
	if ( Number(IN_RIGHT) ) {
		level += ShowBlock(IN_RIGHT)->DamageLevel();
	}
	if ( Number(IN_LEFT) ) {
		level += ShowBlock(IN_LEFT)->DamageLevel();
	}
	return level;
}

bool Dwarf::Move(const int dir) {
	const bool overstepped = Active::Move(dir);
	if ( overstepped ) {
		for (ushort i=0; i<ON_LEGS; ++i) {
			Block * const block = ShowBlock(i);
			if ( block && block->Kind()==MAP ) {
				block->Use(this);
			}
		}
	}
	return overstepped;
}

quint16 Dwarf::NutritionalValue(const int sub) const {
	switch ( sub ) {
	case HAZELNUT: return SECONDS_IN_HOUR/2;
	case H_MEAT:   return SECONDS_IN_HOUR*2.5;
	case A_MEAT:   return SECONDS_IN_HOUR*2;
	}
	return 0;
}

void Dwarf::MoveInside(const ushort num_from, const ushort num_to,
		const ushort num)
{
	Block * const block = ShowBlock(num_from);
	if ( block && (num_to > ON_LEGS ||
			IN_RIGHT==num_to || IN_LEFT==num_to ||
			( ON_HEAD==num_to &&
				WEARABLE_HEAD==block->Wearable() ) ||
			( ON_BODY==num_to &&
				WEARABLE_BODY==block->Wearable() ) ||
			( ON_LEGS==num_to &&
				WEARABLE_LEGS==block->Wearable() )) )
	{
		Inventory::MoveInside(num_from, num_to, num);
	}
	UpdateLightRadius();
	GetWorld()->Shine(X(), Y(), Z(), lightRadius, true);
}

void Dwarf::SaveAttributes(QDataStream & out) const {
	Animal::SaveAttributes(out);
	Inventory::SaveAttributes(out);
	out << activeHand;
}

bool Dwarf::Inscribe(const QString &) {
	SendSignalAround(tr("Don't touch me!"));
	return false;
}

Dwarf::Dwarf(const int sub, const quint16 id) :
		Animal(sub, id),
		Inventory(),
		activeHand(IN_RIGHT),
		lightRadius(MIN_DWARF_LIGHT_RADIUS)
{
	note = new QString("Urist");
}
Dwarf::Dwarf(QDataStream & str, const int sub, const quint16 id) :
		Animal(str, sub, id),
		Inventory(str)
{
	str >> activeHand;
	UpdateLightRadius();
}
