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

#ifndef ANIMAL_H
#define ANIMAL_H

#include "Active.h"

class Animal : public Active {
	Q_OBJECT

	quint8 breath;
	quint16 satiation;

	virtual quint16 NutritionalValue(int sub) const=0;

	public:
	void DoRareAction();
	int  ShouldAct() const;
	QString FullName() const=0;
	Animal * IsAnimal();

	ushort Breath() const;
	ushort Satiation() const;
	bool Eat(int sub);

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Animal(int sub, quint16 id);
	Animal(QDataStream & str, int sub, quint16 id);
}; // class Animal

#endif
