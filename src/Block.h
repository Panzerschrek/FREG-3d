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

#ifndef BLOCK_H
#define BLOCK_H

#include "header.h"
#include <QString>

const QString SOUND_STRINGS[] = {
	"Ding!",
	"Ouch!"
};

enum before_push_action {
	NO_ACTION,
	MOVE_UP,
	JUMP,
	DAMAGE,
	DESTROY,
	MOVE_SELF
}; // enum before_push_action

enum WEARABLE {
	WEARABLE_NOWHERE,
	WEARABLE_HEAD,
	WEARABLE_ARM,
	WEARABLE_BODY,
	WEARABLE_LEGS
}; // enum WEARABLE

class Inventory;
class Active;
class Animal;

class Block { // blocks without special physics and attributes
	static const ushort MAX_NOTE_LENGTH = 144;
	quint8 Transparency(quint8 transp, int sub);

	const quint8 transparent;
	const quint8 sub;
	const quint16 id;
	quint8 direction;

	protected:
	QString * note;
	qint16 durability;

	public:
	virtual QString FullName() const;
	virtual quint8 Kind() const;
	virtual bool Catchable() const;
	virtual int  Movable() const;
	/// Returns true on success.
	virtual bool Inscribe(const QString & str);
	virtual int  BeforePush(int dir, Block * who);
	virtual bool Move(int direction);
	virtual int  Damage(ushort dmg, int dmg_kind);
	virtual usage_types Use(Block * who=0);
	/// Usually returns new block of the same kind and sub (except glass).
	/** When reimplemented in derivatives, inside it you can create a pile,
	 *  put several blocks in it, and return pile.
	 */
	virtual Block * DropAfterDamage() const;

	virtual Inventory * HasInventory();
	virtual Animal * IsAnimal();
	virtual Active * ActiveBlock();

	virtual int Wearable() const;
	virtual int DamageKind() const;
	virtual ushort DamageLevel() const;

	virtual uchar LightRadius() const;
	virtual int Temperature() const;
	virtual ushort Weight() const;
	/// Receive text signal.
	virtual void ReceiveSignal(const QString &);

	protected:
	virtual void SaveAttributes(QDataStream &) const;

	public:
	/// Determines kind and sub, unique for every kind-sub pair.
	quint16 GetId() const;
	void Restore();
	void SetDir(int dir);

	int GetDir() const;
	int Sub() const;
	short Durability() const;
	QString GetNote() const;
	int Transparent() const;

	bool operator==(const Block &) const;

	void SaveToFile(QDataStream & out) const;

	Block(int sub, quint16 id, quint8 transp=UNDEF);
	Block(QDataStream &, int sub, quint16 id, quint8 transp=UNDEF);
	virtual ~Block();

	protected:
	static quint16 IdFromKindSub(quint16 id, quint8 sub);
}; // class Block

#endif
