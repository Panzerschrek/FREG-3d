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

#include "screen.h" //NOX, if needed, is defined in screen.h

#ifdef NOX //no need for X server
	#include <QCoreApplication>
#else
	#include <QApplication>
#endif

#include <QString>
#include <QSettings>
#include <QDir>
#include <QTime>
#include "world.h"
#include "Player.h"

int main(int argc, char *argv[]) {
	puts(qPrintable(QObject::tr("Starting...")));
	QDir::current().mkdir("texts");
	freopen("texts/errors.txt", "wt", stderr);
	qsrand(QTime::currentTime().msec());
	#ifdef NOX
		QCoreApplication freg(argc, argv);
	#else
		QApplication freg(argc, argv);
	#endif
	QCoreApplication::setOrganizationName("freg-team");
	QCoreApplication::setApplicationName("freg");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	QSettings sett(QDir::currentPath()+"/freg.ini", QSettings::IniFormat);
	const QString worldName = sett.value("current_world", "mu").toString();
	sett.setValue("current_world", worldName);

	World world(worldName);
	Player player;
	const Screen screen(&world, &player);
	world.start();

	QObject::connect(&player, SIGNAL(Destroyed()),
		&screen, SLOT(DeathScreen()));

	QObject::connect(&freg, SIGNAL(aboutToQuit()),
		&screen, SLOT(CleanAll()));
	QObject::connect(&freg, SIGNAL(aboutToQuit()),
		&player, SLOT(CleanAll()));
	QObject::connect(&freg, SIGNAL(aboutToQuit()),
		&world, SLOT(CleanAll()));

	QObject::connect(&screen, SIGNAL(ExitReceived()), &freg, SLOT(quit()));
	QObject::connect(&world,  SIGNAL(ExitReceived()), &freg, SLOT(quit()));

	return freg.exec();
}
