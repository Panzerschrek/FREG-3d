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

#ifndef SHREDMANAGER_H
#define SHREDMANAGER_H


#define REGION_SIZE 8
#define MAX_SHRED_COMPRESSED_DATA_SIZE 16384//16 kb

#include "world.h"

struct Region
{
    int x, y;
    QByteArray compressed_region_data;

};

class ShredManager
{
    public:
    ShredManager( World* world );

    void WriteShredData( QByteArray* uncompressed_data, int longitude, int latitude );
    void GetShredData( QByteArray* uncompressed_data, int longitude, int latitude );

    private:
    World* world;
    int region_x0, region_y0, region_x1, region_y1;
    int num_regions;

};
#endif//SHREDMANAGER_H
