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

#ifndef CRAFTMANAGER_H
#define CRAFTMANAGER_H

#include <QList>

typedef struct {
	ushort num;
	int kind;
	int sub;
} craft_item;
typedef QList<craft_item *> craft_recipe;

class CraftManager {
	public:

	bool MiniCraft(craft_item & item, craft_item & result) const;
	bool Craft(const craft_recipe & recipe, craft_item & result) const;
	
	CraftManager();
	~CraftManager();

	private:
	QList<craft_recipe *> recipes;
}; //class CraftManager

extern CraftManager craft_manager;

#endif
