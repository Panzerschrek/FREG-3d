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

#ifndef SHRED_STORAGE_H
#define SHRED_STORAGE_H

#include <QHash>
class QByteArray;

struct LongLat {
	long longitude;
	long latitude;

	bool operator==(const LongLat &) const;
	LongLat(long longitude, long latitude);
};

class PreloadThread;

class ShredStorage {
	public:
	ShredStorage(ushort size,
			long longi_center, long lati_center);
	~ShredStorage();

	QByteArray * GetShredData(long longi, long lati) const;
	void SetShredData(QByteArray *, long longi, long lati);
	void Shift(int direction, long longitude, long latitude);

	void AddShredData(long longi, long lati);
	void WriteToFileShredData(long longi, long lati);

	void Remove(long longi, long lati);

	private:
	QHash<LongLat, QByteArray *> storage;
	const ushort size;
	PreloadThread * preloadThread;
}; // class ShredStorage

#include <QThread>

class PreloadThread : public QThread {
	Q_OBJECT

	public:
	PreloadThread(ShredStorage *, int direction,
			long longi_center, long lati_center,
			ushort size);

	protected:
	void run();

	private:
	ShredStorage * const storage;
	const int direction;
	const long longi_center;
	const long lati_center;
	const ushort size;
}; // class PreloadThread

#endif
