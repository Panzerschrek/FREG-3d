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

#include "VirtScreen.h"
#include "Player.h"
#include "world.h"

void VirtScreen::ConnectWorld() {
	connect(w, SIGNAL(Updated(
			const ushort,
			const ushort,
			const ushort)),
		this, SLOT(Update(
			const ushort,
			const ushort,
			const ushort)),
		Qt::DirectConnection);
	connect(w, SIGNAL(UpdatedAround(
			const ushort,
			const ushort,
			const ushort,
			const ushort)),
		this, SLOT(UpdateAround(
			const ushort,
			const ushort,
			const ushort,
			const ushort)),
		Qt::DirectConnection);
}

void VirtScreen::UpdatesEnd() {}

void VirtScreen::DeathScreen() {}

VirtScreen::VirtScreen(World * const world_, Player * const player_) :
		w(world_),
		player(player_)
{
	connect(w, SIGNAL(Notify(const QString &)),
		this, SLOT(Notify(const QString &)));
	connect(player, SIGNAL(Notify(const QString &)),
		this, SLOT(Notify(const QString &)));

	connect(w, SIGNAL(GetString(QString &)),
		this, SLOT(PassString(QString &)),
		Qt::DirectConnection);
	connect(player, SIGNAL(GetString(QString &)),
		this, SLOT(PassString(QString &)),
		Qt::DirectConnection);

	connect(player, SIGNAL(Updated()),
		this, SLOT(UpdatePlayer()),
		Qt::DirectConnection);
	connect(w, SIGNAL(ReConnect()),
		this, SLOT(ConnectWorld()),
		Qt::DirectConnection);
	connect(w, SIGNAL(UpdatedAll()),
		this, SLOT(UpdateAll()),
		Qt::DirectConnection);
	connect(w, SIGNAL(Moved(const int)),
		this, SLOT(Move(const int)),
		Qt::DirectConnection);
	ConnectWorld();
	connect(w, SIGNAL(UpdatesEnded()),
		this, SLOT(UpdatesEnd()),
		Qt::DirectConnection);
}

void VirtScreen::CleanAll() {}

VirtScreen::~VirtScreen() { CleanAll(); }
