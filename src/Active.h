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

#ifndef ACTIVE_H
#define ACTIVE_H

#include <QObject>
#include "Block.h"

enum ACTIVE_FREQUENCY {
	NEVER,
	FREQUENT,
	FREQUENT_AND_RARE,
	RARE
}; // enum ACTIVE_FREQUENCY

class Shred;
class World;
class DeferredAction;

class Active : public QObject, public Block {
	Q_OBJECT

	public:
	Active(int sub, quint16 id, quint8 transp=UNDEF);
	Active(QDataStream & str, int sub, quint16 id, quint8 transp=UNDEF);
	~Active();

	Shred * GetShred() const;
	World * GetWorld() const;
	QString FullName() const;
	quint8 Kind() const;

	Active * ActiveBlock();
	/// Returns true if shred border is overstepped.
	bool Move(int direction);
	void SetFalling(bool set);
	bool IsFalling() const;
	void FallDamage();

	ushort X() const;
	ushort Y() const;
	ushort Z() const;

	void ActFrequent();
	void ActRare();
	virtual int ShouldAct() const;
	virtual bool ShouldFall() const;

	void SetDeferredAction(DeferredAction *);
	DeferredAction * GetDeferredAction() const;

	int Movable() const;
	int Damage(ushort dmg, int dmg_kind);
	void ReceiveSignal(const QString &);

	void ReloadToNorth();
	void ReloadToSouth();
	void ReloadToWest();
	void ReloadToEast();

	void EmitUpdated();

	void SetXYZ(ushort x, ushort y, ushort z);
	void SetToDelete();

	signals:
	void Moved(int);
	void Destroyed();
	void Updated();
	void ReceivedText(const QString &);

	protected:
	void SendSignalAround(const QString &) const;
	void SaveAttributes(QDataStream & out) const;

	virtual void DoFrequentAction();
	virtual void DoRareAction();

	private:
	void UpdateShred();
	bool IsToDelete() const;

	quint8 fall_height;
	bool falling;
	bool frozen; // don't do actions when frozen
	DeferredAction * deferredAction;
	/// Coordinates in loaded world zone.
	ushort x_self, y_self, z_self;
}; // class Active

#endif
