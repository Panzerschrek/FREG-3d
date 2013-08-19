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

/** \file
 * \brief VirtScreen class declaration.
 */

#ifndef VIRTSCREEN_H
#define VIRTSCREEN_H

#include <QObject>

class QString;
class World;
class Player;

class VirtScreen : public QObject {
	/** \class VirtScreen VirtScreen.h
		* \brief This class provides base for all screens for freg.
		*
		* It provides interface for world-screen and player-screen
		* communications by its slots and signals.
	*/

	Q_OBJECT
	
	protected:
	///world to print
	World * const w;
	///player to print (HP, inventory, etc)
	Player * const player;

	private slots:
	///Prints world. Should not be called not within screen.
	virtual void Print()=0;

	public slots:
	///This is called for a notification to be displayed.
	virtual void Notify(const QString &) const=0;

	///This is called when program is stopped and from destructor.
	/**
	 * When implemented, this should contain check to prevent
	 * double cleaning.
	 */
	virtual void CleanAll();

	///This is called when string is needed to be received from input.
	/**
	 * It is connected to world in constructor.
	 */
	virtual QString PassString(QString &) const=0;

	///This is called when block at (x, y, z) should be updated in screen.
	/**
	 * When implemented, this should work fast.
	 * It is connected to world in constructor.
	 */
	virtual void Update(ushort x, ushort y, ushort z)=0;

	///This is called when all world should be updated in sceen.
	/**
	 * When implemented, this should work fast.
	 * It is connected to world in constructor.
	 */
	virtual void UpdateAll()=0;

	///Called when world loaded zone is moved to update world in screen.
	/**
	 * When implemented, this should work fast.
	 * It is connected to world in constructor.
	 */
	virtual void Move(int)=0;

	///Called when some player property needs to be updated in screen.
	/**
	 * When implemented, this should work fast.
	 * It is connected to world in constructor.
	 */
	virtual void UpdatePlayer()=0;
	
	///Called when area around xyz with range needs to be updated.
	/**
	 * When implemented, this should work fast.
	 * It is connected to world in constructor.
	 */
	virtual void UpdateAround(
			ushort x, ushort y, ushort z,
			ushort range)=0;

	///This is called to restore some connections.
	/**
	 * This restores connections to VirtScreen::Update and
	 * VirtScreen::UpdateAround which can be temporarily
	 * disconnected.
	 */
	void ConnectWorld();

	///This is called when current group of updates is ended.
	/**
	 * This is called from world when pack of world changing is ended.
	 * ( Can be used in screen optimization. )
	 */
	virtual void UpdatesEnd();

	void DeathScreen();

	signals:
	///This is emitted when input receives exit key.
	/**
	 * This is connected to application exit.
	 */
	void ExitReceived();

	public:
	///Constructor makes player and world connections.
	/**
	 * Constructor of non-virtual screen should contain this code
	 * to connect to player for sending input:
	 * connect(this, SIGNAL(InputReceived(int, int)),
	 * 	player, SLOT(Act(int, int)),
	 * 	Qt::DirectConnection);
	 */
	VirtScreen(World *, Player *);
	///Only calls VirtScreen::CleanAll
	virtual ~VirtScreen();
}; //class VirtScreen

#endif
