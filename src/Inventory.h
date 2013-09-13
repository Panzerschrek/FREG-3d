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

#ifndef INVENTORY_H
#define INVENTORY_H

#include <QStack>

const ushort INV_SIZE = 26;
const ushort MAX_STACK_SIZE = 9;

class Inventory {
	const ushort size;
	QStack<Block *> * inventory;

	protected:
	virtual void SaveAttributes(QDataStream & out) const;

	public:
	virtual quint8 Kind() const=0;
	virtual int Sub() const=0;
	/// Returns true on success.
	virtual bool Drop(ushort src, ushort dest, ushort num, Inventory * to);
	/// Returns true on success.
	virtual bool GetAll(Inventory * from);
	virtual bool Access() const;
	/// Returns true on success.
	virtual bool Get(Block * block, ushort start=0);
	virtual void Pull(ushort num);
	virtual void MoveInside(ushort num_from, ushort num_to, ushort num);
	virtual void ReceiveSignal(const QString &)=0;
	virtual ushort Start() const;
	virtual ushort Weight() const;
	virtual QString FullName() const=0;
	virtual Inventory * HasInventory();

	/// Returns true if block found its place.
	bool GetExact(Block * block, ushort num);
	/// Returns true on success (something has been crafted).
	bool MiniCraft(ushort num);
	/// Returns true on success.
	bool InscribeInv(ushort num, const QString & str);
	int  GetInvSub(ushort i) const;
	int  GetInvKind(ushort i) const;
	ushort Size() const;
	ushort GetInvWeight(ushort i) const;
	quint8 Number(ushort i) const;
	Block * ShowBlock(ushort num) const;
	QString GetInvNote(ushort num) const;
	QString InvFullName(ushort num) const;
	QString NumStr(ushort num) const;

	bool IsEmpty() const;
	bool HasRoom() const;

	void BeforePush(Block * who);

	// it is not recommended to make inventory size more than 26,
	// because it will not be convenient to deal with inventory
	// in console version.
	Inventory(ushort sz=INV_SIZE);
	Inventory(QDataStream & str, ushort size=INV_SIZE);
	~Inventory();
}; // class Inventory

#endif
