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
#include <QString>
#include <qmath.h>
#include "worldmap.h"
#include "header.h"

#ifdef QT_OS_WIN23
	const int END_OF_LINE_SHIFT=2;
#else
	const int END_OF_LINE_SHIFT=1;
#endif

WorldMap::WorldMap(const QString * const world_name) {
	map = new QFile(*world_name+"/map.txt");
	if ( map->open(QIODevice::ReadOnly | QIODevice::Text) ) {
		mapSize=int(qSqrt(1+4*map->size())-1)/2;
	} else {
		const ushort max_command_length=50;
		char command[max_command_length];
		const ushort map_size=75;
		#ifdef Q_OS_LINUX
			snprintf(command,
				max_command_length,
				"./mapgen -s %hu -r %d -f %s",
				map_size,
				qrand(),
				qPrintable(*world_name+"/map.txt"));
		#endif
		#ifdef Q_OS_WIN32
			#ifdef Q_CC_MSVC
			sprintf_s(command,
				max_command_length,
				"mapgen.exe -s %hu -r %d -f %s",
				map_size,
				qrand(),
				qPrintable(*world_name+"/map.txt"));
			#else
			snprintf(command,
				max_command_length,
				"mapgen.exe -s %hu -r %d -f %s",
				map_size,
				qrand(),
				qPrintable(*world_name+"/map.txt"));
			#endif
		#endif
		system(command);
		if ( map->open(QIODevice::ReadOnly | QIODevice::Text) ) {
			mapSize=map_size;
		} else {
			mapSize=1;
		}
	}
}

char WorldMap::TypeOfShred(const long longi, const long lati) {
	if (
			longi >= mapSize || longi < 0 ||
			lati  >= mapSize || lati  < 0 )
	{
		return OUT_BORDER_SHRED;
	} else if ( !map->seek((mapSize+1)*longi+lati) ) {
		return DEFAULT_SHRED;
	}
	char c;
	map->getChar(&c);
	return c;
}

long WorldMap::MapSize() const { return mapSize; }

WorldMap::~WorldMap() { delete map; }
