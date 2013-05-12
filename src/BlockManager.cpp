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
#include <memory>
#include "header.h"
#include "blocks.h"
#include "BlockManager.h"

BlockManager block_manager;

BlockManager::BlockManager()
{
	for(ushort sub=0; sub<=AIR; ++sub) {
		normals[sub]=new Block(sub);
	}
}
BlockManager::~BlockManager() {
	for(ushort sub=0; sub<=AIR; ++sub) {
		delete normals[sub];
	}
}

Block * BlockManager::NormalBlock(const int sub) {
	return normals[sub];
}

Block * BlockManager::NewBlock(const int kind, int sub) {
	if ( sub > AIR ) {
		fprintf(stderr,
			"BlockManager::NewBlock: \
				Don't know such substance: %d.\n",
			sub);
		sub=STONE;
	}
	switch ( kind ) {
		case BLOCK:  return New<Block >(sub);
		case GRASS:  return New<Grass >(sub);
		case PICK:   return New<Pick  >(sub);
		case PLATE:  return New<Plate >(sub);
		case ACTIVE: return New<Active>(sub);
		case LADDER: return New<Ladder>(sub);
		case WEAPON: return New<Weapon>(sub);
		case BUSH:   return New<Bush  >(sub);
		case CHEST:  return New<Chest >(sub);
		case PILE:   return New<Pile  >(sub);
		case DWARF:  return New<Dwarf >(sub);
		case RABBIT: return New<Rabbit>(sub);
		case DOOR:   return New<Door  >(sub);
		case LIQUID: return New<Liquid>(sub);
		case CLOCK:  return New<Clock >(sub);
		case WORKBENCH: return New<Workbench>(sub);
		default:
			fprintf(stderr,
				"BlockManager::NewBlock: unlisted kind: %d\n",
				kind);
			return New<Block>(sub);
	}
}

Block * BlockManager::BlockFromFile(QDataStream & str) {
	quint16 kind, sub;
	bool normal;
	str >> kind >> sub >> normal;
	if ( normal ) {
		return NormalBlock(sub);
	}

	//if some kind will not be listed here,
	//blocks of this kind just will not load,
	//unless kind is inherited from Inventory class or one
	//of its derivatives - in this case this may cause something bad.
	switch ( kind ) {
		case BLOCK:  return New<Block >(str, sub);
		case PICK:   return New<Pick  >(str, sub);
		case PLATE:  return New<Plate >(str, sub);
		case LADDER: return New<Ladder>(str, sub);
		case WEAPON: return New<Weapon>(str, sub);
		case BUSH:   return New<Bush  >(str, sub);
		case CHEST:  return New<Chest >(str, sub);
		case RABBIT: return New<Rabbit>(str, sub);
		case DWARF:  return New<Dwarf >(str, sub);
		case PILE:   return New<Pile  >(str, sub);
		case GRASS:  return New<Grass >(str, sub);
		case ACTIVE: return New<Active>(str, sub);
		case LIQUID: return New<Liquid>(str, sub);
		case LOCKED_DOOR:
		case DOOR:   return New<Door  >(str, sub);
		case CLOCK:  return New<Clock >(str, sub);
		case WORKBENCH: return New<Workbench>(str, sub);
		default:
			fprintf(stderr,
				"BlockManager::BlockFromFile: \
				unlisted kind: %d.\n",
				kind);
			return New<Block>(str, sub);
	}
}

void BlockManager::DeleteBlock(Block * const block) {
	if ( block && block!=NormalBlock(block->Sub()) ) {
		delete block;
	}
}

template <typename Thing>
Thing * BlockManager::New(const int sub) {
	return new Thing(sub);
}

template <typename Thing>
Thing * BlockManager::New(QDataStream & str, const int sub) {
	return new Thing(str, sub);
}
