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

#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

class Block;
class QDataStream;

/** \class BlockManager BlockManager.h
 * \brief This class is used for creating and deleting blocks,
 * also for loading them from file.
 *
 * Memory management, if any, should be implemented in this class.
 * At the current moment no special memory management is used.
 *
 * Normal blocks: blocks that are not special, e.g. usual stone, air, soil
 * are actually one block (for each substance).
 * One can receive a pointer to such block with
 * Block * NormalBlock(int sub).
 * Normal blocks are not needed to be deleted.
 * Use Block * NewBlock(int kind, int sub) to receive a pointer to
 * block that will be changed (damaged, inscribed, etc).
 */

class BlockManager {
	public:
	BlockManager();
	~BlockManager();

	///Use this to receive a pointer to normal block.
	Block * NormalBlock(int sub);
	///Use this to receive a pointer to new not-normal block.
	Block * NewBlock(int kind, int sub=STONE);
	///Use this to load block from file.
	Block * BlockFromFile(QDataStream &);
	///Use this to safely delete block.
	void DeleteBlock(Block * block);

	private:
	Block * normals[AIR+1];

	template <typename Thing>
	Thing * New(int sub);
	template <typename Thing>
	Thing * New(QDataStream & str, int sub);
}; //class BlockManager

extern BlockManager block_manager;

#endif
