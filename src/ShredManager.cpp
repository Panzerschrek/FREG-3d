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

#ifndef SHREDMANAGER_CPP
#define SHREDMANAGER_CPP

#include "ShredManager.h"


ShredManager::ShredManager( World* world )
{
    this->world= world;


    if( world->Latitude() < 0 )
        region_x0= ( world->Latitude() + 1 - REGION_SIZE ) / REGION_SIZE;
    else
        region_x0= world->Latitude() / REGION_SIZE;

    if( world->Longitude() < 0 )
        region_y0= ( world->Longitude() + 1 - REGION_SIZE ) / REGION_SIZE;
    else
        region_y0= world->Longitude() / REGION_SIZE;


     if( world->Latitude() + world->NumShreds() < 0 )
        region_x1= ( world->Latitude() + world->NumShreds() + 1 - REGION_SIZE ) / REGION_SIZE;
    else
        region_x1= ( world->Latitude() + world->NumShreds() ) / REGION_SIZE;

    if( world->Longitude() + world->NumShreds() < 0 )
        region_y1= ( world->Longitude() + world->NumShreds() + 1 - REGION_SIZE ) / REGION_SIZE;
    else
        region_y1= ( world->Longitude() + world->NumShreds() ) / REGION_SIZE;

    num_regions= world->NumShreds() / REGION_SIZE + 2;
    num_regions*= num_regions;

    regions_list= new QByteArray*[ num_regions ];
    regions_coords= new unsigned int [ num_
    for( iny i= 0; i< num_regions; i++ )
        regions_list[i]= new QByteArray();

}

#endif//SHREDMANAGER_CPP
