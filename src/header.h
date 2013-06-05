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

#ifndef HEADER_H
#define HEADER_H

#include <cstdio>
#include <QtGlobal>

#ifdef Q_OS_WIN32
	#include <windows.h>
	inline void usleep(int n ) { Sleep(n/1000); }
#endif

const float FREG_VERSION=0.1;

const ushort SHRED_WIDTH=16;
const ushort HEIGHT=128;

const ushort SECONDS_IN_HOUR=60;
const ushort SECONDS_IN_DAY=24*SECONDS_IN_HOUR;
const ushort END_OF_NIGHT  = 6*SECONDS_IN_HOUR;
const ushort END_OF_MORNING=12*SECONDS_IN_HOUR;
const ushort END_OF_NOON   =18*SECONDS_IN_HOUR;
const ushort END_OF_EVENING= 0*SECONDS_IN_HOUR;
const ushort SECONDS_IN_NIGHT=END_OF_NIGHT;
const ushort SECONDS_IN_DAYLIGHT=SECONDS_IN_DAY-END_OF_NIGHT;

const uchar MAX_LIGHT_RADIUS=15;

const ushort MAX_DURABILITY=100;
const ushort MAX_BREATH=60;

enum dirs {
	UP, ///< 0
	DOWN, ///< 1
	NORTH, ///< 2
	SOUTH, ///< 3
	EAST, ///< 4
	WEST, ///< 5
	NORTH_EAST, ///< 6
	SOUTH_EAST, ///< 7
	SOUTH_WEST, ///< 8
	NORTH_WEST, ///< 9
	HERE ///< 10
}; //enum dirs

enum { NOT_MOVABLE, MOVABLE, ENVIRONMENT };

enum times_of_day { MORNING, NOON, EVENING, NIGHT };

enum damage_kinds {
	MINE,    ///< 0
	DIG,     ///< 1
	CUT,     ///< 2
	THRUST,  ///< 3
	CRUSH,   ///< 4
	HEAT,    ///< 5
	FREEZE,  ///< 6
	MELT,    ///< 7
	ELECTRO, ///< 8
	HUNGER,  ///< 9
	BREATH,  ///< 10
	EATEN,   ///< 11
	TIME,    ///< 12
	NO_HARM, ///< 13
	DAMAGE_FALL ///< 14
}; //enum damage_kinds

enum kinds {//kind of atom
	//do not change order, or rewrite craft recipes.
	//add new kinds to bottom.
	BLOCK, ///<0
	BELL, ///<1
	CHEST, ///<2
	PILE, ///<3
	DWARF, ///<4
	ANIMAL, ///<5
	PICK, ///<6
	TELEGRAPH, ///<7
	LIQUID, ///<8
	GRASS, ///<9
	BUSH, ///<10
	RABBIT, ///<11
	ACTIVE, ///<12
	CLOCK, ///<13
	PLATE, ///<14
	WORKBENCH, ///<15
	WEAPON, ///<16
	LADDER, ///<17
	DOOR, ///< 18
	LOCKED_DOOR ///< 19
}; //enum kinds
enum subs {//substance block is made from
	//do not change order, or rewrite craft recipes.
	//add new substances right before air.
	STONE,      ///<0
	MOSS_STONE, ///<1
	NULLSTONE,  ///<2
	SKY,        ///<3
	STAR,       ///<4
	SUN_MOON,   ///<5
	SOIL,       ///<6
	H_MEAT,     ///<7 (hominid meat)
	A_MEAT,     ///<8 (animal meat)
	GLASS,      ///<9
	WOOD,       ///<10
	DIFFERENT,  ///<11
	IRON,       ///<12
	WATER,      ///<13
	GREENERY,   ///<14
	SAND,       ///<15
	HAZELNUT,   ///<16
	ROSE,       ///<17
	CLAY,       ///<18
	AIR //keep it last in this list
}; //enum subs

enum usage_types { NO, OPEN, INNER_ACTION };

enum transparency {
	BLOCK_OPAQUE,
	BLOCK_TRANSPARENT,
	INVISIBLE,
	NONSTANDARD=6,
	UNDEF
}; //enum transparency


#endif
