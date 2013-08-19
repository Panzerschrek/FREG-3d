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

#ifndef DEFERRED_ACTION_H
#define DEFERRED_ACTION_H

enum deferred_actions {
	DEFERRED_NOTHING,
	DEFERRED_GHOST_MOVE,
	DEFERRED_MOVE,
	DEFERRED_JUMP,
	DEFERRED_BUILD,
	DEFERRED_DAMAGE,
	DEFERRED_THROW
}; //enum deferred_actions

class Block;
class Active;
class World;

class DeferredAction {
	private:
	int type;
	Active * const attachedBlock;
	ushort xTarg, yTarg, zTarg;
	Block * material;
	ushort srcSlot, destSlot;
	ushort num;
	World * const world;

	void GhostMove() const;
	void Move() const;
	void Jump() const;
	void Build();
	void Damage() const;
	void Throw() const;

	void UnsetDeferredAction();

	public:
	void SetGhostMove(ushort dir=HERE);
	void SetMove(ushort dir=HERE);
	void SetJump();
	void SetBuild(ushort x_targ, ushort y_targ, ushort z_targ,
			Block * material,
			ushort builder_slot);
	void SetDamage(ushort x_targ, ushort y_targ, ushort z_targ);
	void SetThrow(ushort x_targ, ushort y_targ, ushort z_targ,
			ushort src_slot, ushort dest_slot, ushort num);

	World * GetWorld() const;
	int  GetActionType() const;
	void MakeAction();

	DeferredAction(Active * attached, World *);
	~DeferredAction();
}; //class DeferredAction

#endif
