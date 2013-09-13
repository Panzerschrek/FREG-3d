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
 * block that will be changed (damaged, inscribed, etc). */

class BlockManager {
	public:
	BlockManager();
	~BlockManager();

	/// Use this to receive a pointer to normal block.
	Block * NormalBlock(int sub) const;
	/// Use this to receive a pointer to new not-normal block.
	Block * NewBlock(int kind, int sub) const;
	/// Use this to load block from file.
	Block * BlockFromFile(QDataStream &, quint8 kind, quint8 sub) const;
	Block * BlockFromFile(QDataStream &) const;
	/// Returns true if block is normal.
	bool KindSubFromFile(QDataStream &, quint8 & kind, quint8 & sub) const;
	/// Use this to safely delete block.
	void DeleteBlock(Block * block) const;

	static QString KindToString(quint8 kind);
	static QString SubToString(quint8 sub);
	static quint8 StringToKind(const QString &);
	static quint8 StringToSub(const QString &);
	static quint16 MakeId(quint8 kind, quint8 sub);

	private:
	Block * normals[LAST_SUB];
	static const QString kinds[LAST_KIND];
	static const QString subs[LAST_SUB];

	template <typename Thing>
	Thing * New(int sub, quint16 id) const;
	template <typename Thing>
	Thing * New(QDataStream & str, int sub, quint16 id) const;
}; // class BlockManager

extern BlockManager block_manager;

#endif
