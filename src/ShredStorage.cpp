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

#include <QByteArray>
#include <QFile>
#include "ShredStorage.h"
#include "world.h"
#include "Shred.h"

bool LongLat::operator==(const LongLat & coords) const {
	return ( longitude==coords.longitude &&
	          latitude==coords.latitude );
}

LongLat::LongLat(const long longi, const long lati) :
		longitude(longi),
		latitude(lati)
{}

uint qHash(const LongLat & coords) {
	return ((coords.longitude & 0xffff) << 16) +
	        (coords.latitude  & 0xffff);
}

ShredStorage::ShredStorage(const ushort size_,
		const long longi_center, const long lati_center)
	:
		size(size_),
		preloadThread(0)
{
	storage.reserve(size*size);
	for (long i=longi_center-size/2; i<=longi_center+size/2; ++i)
	for (long j= lati_center-size/2; j<= lati_center+size/2; ++j) {
		AddShredData(i, j);
	}
}

ShredStorage::~ShredStorage() {
	if ( preloadThread ) {
		preloadThread->wait();
		delete preloadThread;
	}
	QHash<LongLat, QByteArray *>::const_iterator i=storage.constBegin();
	for ( ; i!=storage.constEnd(); ++i ) {
		if ( i.value() ) {
			WriteToFileShredData(
				i.key().longitude, i.key().latitude);
		}
	}
}

void ShredStorage::Shift(const int direction,
		const long longitude_center, const long latitude_center)
{
	if ( preloadThread ) {
		preloadThread->wait();
		delete preloadThread;
	}
	preloadThread = new PreloadThread(this, direction,
		longitude_center, latitude_center, size);
	preloadThread->start();
}

QByteArray * ShredStorage::GetShredData(const long longi, const long lati)
const {
	return storage.value(LongLat(longi, lati));
}

void ShredStorage::SetShredData(QByteArray * const data,
		const long longi, const long lati)
{
	const LongLat coords(longi, lati);
	delete storage.value(coords);
	storage.insert(coords, data);
}

void ShredStorage::AddShredData(const long longitude, const long latitude) {
	QFile file(Shred::FileName(world->WorldName(), longitude, latitude));
	storage.insert(LongLat(longitude, latitude),
		( file.open(QIODevice::ReadOnly) ?
			new QByteArray(qUncompress(file.readAll())) : 0 ));
}

void ShredStorage::WriteToFileShredData(const long longi, const long lati) {
	const QByteArray * const data=storage.value(LongLat(longi, lati));
	if ( data ) {
		QFile file(Shred::FileName(world->WorldName(), longi, lati));
		if ( file.open(QIODevice::WriteOnly) ) {
			file.write(qCompress(*data));
		}
		delete data;
	}
}

void ShredStorage::Remove(const long longi, const long lati) {
	storage.remove(LongLat(longi, lati));
}

PreloadThread::PreloadThread(ShredStorage * const stor, const int dir,
		const long longi_c, const long lati_c, const ushort sz)
	:
		storage(stor),
		direction(dir),
		longi_center(longi_c),
		lati_center(lati_c),
		size(sz)
{}

void PreloadThread::run() {
	switch (direction) {
	case NORTH:
		for (long i=lati_center-size/2; i<=lati_center+size/2; ++i) {
			storage->WriteToFileShredData(longi_center+size/2, i);
			storage->Remove(longi_center+size/2, i);
			storage->AddShredData(longi_center-size/2, i);
		}
	break;
	case SOUTH:
		for (long i=lati_center-size/2; i<=lati_center+size/2; ++i) {
			storage->WriteToFileShredData(longi_center-size/2, i);
			storage->Remove(longi_center-size/2, i);
			storage->AddShredData(longi_center+size/2, i);
		}
	break;
	case EAST:
		for (long i=longi_center-size/2; i<=longi_center+size/2; ++i) {
			storage->WriteToFileShredData(i, lati_center-size/2);
			storage->Remove(i, lati_center-size/2);
			storage->AddShredData(i, lati_center+size/2);
		}
	break;
	case WEST:
		for (long i=longi_center-size/2; i<=longi_center+size/2; ++i) {
			storage->WriteToFileShredData(i, lati_center+size/2);
			storage->Remove(i, lati_center+size/2);
			storage->AddShredData(i, lati_center-size/2);
		}
	break;
	default: fprintf(stderr,
		"PreloadThread::run: unknown direction: %d\n", direction);
	}
}
