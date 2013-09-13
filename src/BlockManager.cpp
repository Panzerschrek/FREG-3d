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
#include "header.h"
#include "blocks.h"
#include "Dwarf.h"
#include "BlockManager.h"

const QString BlockManager::kinds[LAST_KIND]={
	"block",
	"bell",
	"chest",
	"pile",
	"intellectual",
	"animal",
	"pick",
	"telegraph",
	"liquid",
	"grass",
	"bush",
	"rabbit",
	"active",
	"clock",
	"plate",
	"workbench",
	"weapon",
	"ladder",
	"door",
	"locked door",
	"creator",
	"text",
	"map"
};

const QString BlockManager::subs[LAST_SUB]={
	"stone",
	"moss stone",
	"nullstone",
	"sky",
	"star",
	"sun_or_moon",
	"soil",
	"meat_of_intellectual",
	"animal_meat",
	"glass",
	"wood",
	"different",
	"iron",
	"water",
	"greenery",
	"sand",
	"hazelnut",
	"rose",
	"clay",
	"air",
	"paper"
};

BlockManager block_manager;

BlockManager::BlockManager() {
	for (ushort sub=0; sub<LAST_SUB; ++sub) {
		normals[sub]=new Block(sub, MakeId(BLOCK, sub));
	}
}
BlockManager::~BlockManager() {
	for(ushort sub=0; sub<LAST_SUB; ++sub) {
		delete normals[sub];
	}
}

Block * BlockManager::NormalBlock(const int sub) const { return normals[sub]; }

Block * BlockManager::NewBlock(const int kind, int sub) const {
	if ( sub >= LAST_SUB  ) {
		fprintf(stderr, "BlockManager::NewBlock: substance (?): %d.\n",
			sub);
		sub = STONE;
	}
	const quint16 id = MakeId(kind, sub);
	switch ( kind ) {
	default: fprintf(stderr, "BlockManager::NewBlock: unlisted kind: %d\n",
		kind);
	case BLOCK:  return New<Block >(sub, id);
	case BELL:   return New<Bell  >(sub, id);
	case GRASS:  return New<Grass >(sub, id);
	case PICK:   return New<Pick  >(sub, id);
	case PLATE:  return New<Plate >(sub, id);
	case ACTIVE: return New<Active>(sub, id);
	case LADDER: return New<Ladder>(sub, id);
	case WEAPON: return New<Weapon>(sub, id);
	case BUSH:   return New<Bush  >(sub, id);
	case CHEST:  return New<Chest >(sub, id);
	case PILE:   return New<Pile  >(sub, id);
	case DWARF:  return New<Dwarf >(sub, id);
	case RABBIT: return New<Rabbit>(sub, id);
	case LOCKED_DOOR:
	case DOOR:   return New<Door  >(sub, id);
	case LIQUID: return New<Liquid>(sub, id);
	case CLOCK:  return New<Clock >(sub, id);
	case TEXT:   return New<Text  >(sub, id);
	case MAP:    return New<Map   >(sub, id);
	case CREATOR: return New<Creator>(sub, id);
	case WORKBENCH: return New<Workbench>(sub, id);
	}
} // Block * BlockManager::NewBlock(int kind, int sub)

Block * BlockManager::BlockFromFile(QDataStream & str,
		const quint8 kind, const quint8 sub)
const {
	const quint16 id = MakeId(kind, sub);
	switch ( kind ) {
	default: fprintf(stderr,
		"BlockManager::BlockFromFile: kind (?): %d\n.", kind);
	case BLOCK:  return New<Block >(str, sub, id);
	case BELL:   return New<Bell  >(str, sub, id);
	case PICK:   return New<Pick  >(str, sub, id);
	case PLATE:  return New<Plate >(str, sub, id);
	case LADDER: return New<Ladder>(str, sub, id);
	case WEAPON: return New<Weapon>(str, sub, id);
	case BUSH:   return New<Bush  >(str, sub, id);
	case CHEST:  return New<Chest >(str, sub, id);
	case RABBIT: return New<Rabbit>(str, sub, id);
	case DWARF:  return New<Dwarf >(str, sub, id);
	case PILE:   return New<Pile  >(str, sub, id);
	case GRASS:  return New<Grass >(str, sub, id);
	case ACTIVE: return New<Active>(str, sub, id);
	case LIQUID: return New<Liquid>(str, sub, id);
	case TEXT:   return New<Text  >(str, sub, id);
	case MAP:    return New<Map   >(str, sub, id);
	case LOCKED_DOOR:
	case DOOR:   return New<Door  >(str, sub, id);
	case CLOCK:  return New<Clock >(str, sub, id);
	case CREATOR: return New<Creator>(str, sub, id);
	case WORKBENCH: return New<Workbench>(str, sub, id);
	}
}

Block * BlockManager::BlockFromFile(QDataStream & str) const {
	quint8 kind, sub;
	return KindSubFromFile(str, kind, sub) ?
		NormalBlock(sub) : BlockFromFile(str, kind, sub);
}

bool BlockManager::KindSubFromFile(QDataStream & str,
		quint8 & kind, quint8 & sub)
const {
	quint8 data;
	str >> data;
	sub = (data & 0x7F);
	if ( data & 0x80 ) { // normal bit
		return true;
	} else {
		str >> kind;
		return false;
	}
}

void BlockManager::DeleteBlock(Block * const block) const {
	if ( block != NormalBlock(block->Sub()) ) {
		delete block;
	}
}

QString BlockManager::KindToString(const quint8 kind) {
	return ( kind<LAST_KIND ) ?
		kinds[kind] : "unknown kind";
}

QString BlockManager::SubToString(const quint8 sub) {
	return ( sub<LAST_SUB ) ?
		subs[sub] : "unknown sub";
}

quint8 BlockManager::StringToKind(const QString & str) {
	int i = 0;
	for ( ; i<LAST_KIND && kinds[i]!=str; ++i);
	return i;
}

quint8 BlockManager::StringToSub(const QString & str) {
	int i = 0;
	for ( ; i<LAST_SUB && subs[i]!=str; ++i);
	return i;
}

template <typename Thing>
Thing * BlockManager::New(const int sub, const quint16 id) const {
	return new Thing(sub, id);
}

template <typename Thing>
Thing * BlockManager::New(QDataStream & str, const int sub, const quint16 id)
const {
	return new Thing(str, sub, id);
}

quint16 BlockManager::MakeId(const quint8 kind, const quint8 sub) {
	return (kind << 8) | sub;
}
