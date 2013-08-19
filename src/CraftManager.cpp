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

#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include "CraftManager.h"

CraftManager craft_manager;

bool CraftManager::MiniCraft(craft_item & item, craft_item & result) const {
	craft_recipe recipe;
	recipe.append(&item);
	return Craft(recipe, result);
}

bool CraftManager::Craft(const craft_recipe & recipe, craft_item & result)
const {
	const ushort size=recipe.size();
	for (ushort i=0; i<recipes.size(); ++i) {
		if ( recipes.at(i)->size()!=size+1 ) {
			continue;
		}
		ushort j=0;
		for ( ; j<size && recipes.at(i)->at(j)->num==recipe.at(j)->num
			&& recipes.at(i)->at(j)->kind==recipe.at(j)->kind
			&& recipes.at(i)->at(j)->sub==recipe.at(j)->sub; ++j);
		if ( j==size ) {
			result.num =recipes.at(i)->at(j)->num;
			result.kind=recipes.at(i)->at(j)->kind;
			result.sub =recipes.at(i)->at(j)->sub;
			return true;
		}
	}
	return false;
}

CraftManager::CraftManager() {
	QFile file("texts/recipes.txt");
	if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		fputs("No recipes file found.\n", stderr);
		return;
	}
	while ( !file.atEnd() ) {
		const QByteArray rec_arr=file.readLine();
		if ( rec_arr.isEmpty() ) {
			fputs("recipes read error.\n", stderr);
			break;
		}
		QTextStream in(rec_arr, QIODevice::ReadOnly | QIODevice::Text);
		craft_recipe * const recipe=new craft_recipe;
		for (;;) {
			craft_item * const item=new craft_item;
			item->num=0;
			in >> item->num >> item->kind >> item->sub;
			if ( item->num ) {
				recipe->append(item);
			} else {
				delete item;
				break;
			}
		}
		recipes.append(recipe);
	}
}

CraftManager::~CraftManager() {
	for (ushort j=0; j<recipes.size(); ++j) {
		for (ushort i=0; i<recipes.at(j)->size(); ++i) {
			delete recipes.at(j)->at(i);
		}
		delete recipes.at(j);
	}
}
