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

#include "world.h"
#include "Active.h"
#include "Inventory.h"
#include "DeferredAction.h"

void DeferredAction::GhostMove() const {
	attachedBlock->Move(( HERE==num ) ?
		attachedBlock->GetDir() : num);
}

void DeferredAction::Move() const {
	GetWorld()->Move(attachedBlock->X(), attachedBlock->Y(),
		attachedBlock->Z(), ( HERE==num ) ?
			attachedBlock->GetDir() : num);
}

void DeferredAction::Jump() const {
	GetWorld()->Jump(
		attachedBlock->X(), attachedBlock->Y(), attachedBlock->Z(),
		attachedBlock->GetDir());
}

void DeferredAction::Build() {
	if ( DOWN==attachedBlock->GetDir() &&
			AIR!=GetWorld()->Sub(xTarg, yTarg, zTarg) )
	{
		if ( world->Move(attachedBlock->X(), attachedBlock->Y(),
				attachedBlock->Z(), UP) )
		{
			++zTarg;
		} else {
			return;
		}
	}
	const int kind = material->Kind();
	const int sub  = material->Sub();
	if ( !world->Build(material, xTarg, yTarg, zTarg,
			World::TurnRight(attachedBlock->GetDir()),
			attachedBlock) )
	{ // build not successful
		return;
	} // else:
	Inventory * const inv=attachedBlock->HasInventory();
	if ( !inv ) {
		return;
	} // else:
	inv->Pull(srcSlot);
	// put more material in building inventory slot:
	if ( inv->Number(srcSlot) ) {
		return;
	} // else:
	for (ushort i=srcSlot+1; i<inv->Size() &&
		inv->Number(srcSlot)<MAX_STACK_SIZE; ++i)
	{
		const Block * const block_i=inv->ShowBlock(i);
		if ( block_i &&
				kind==block_i->Kind() &&
				sub ==block_i->Sub() )
		{
			inv->MoveInside(i, srcSlot, inv->Number(i));
		}
	}
} // void DeferredAction::Build()

void DeferredAction::Damage() const {
	world->Damage(xTarg, yTarg, zTarg,
		attachedBlock->DamageLevel(),
		attachedBlock->DamageKind());
	world->DestroyAndReplace(xTarg, yTarg, zTarg);
}

void DeferredAction::Throw() const {
	world->Drop(
		attachedBlock,
		xTarg, yTarg, zTarg,
		srcSlot, destSlot, num);
	attachedBlock->EmitUpdated();
}

void DeferredAction::SetGhostMove(const ushort dir) {
	type=DEFERRED_GHOST_MOVE;
	num=dir;
}

void DeferredAction::SetMove(const ushort dir) {
	type=DEFERRED_MOVE;
	num=dir;
}
void DeferredAction::SetJump() {
	type=DEFERRED_JUMP;
}
void DeferredAction::SetBuild(const ushort x, const ushort y, const ushort z,
		Block * const mat, const ushort builder_slot)
{
	xTarg = x;
	yTarg = y;
	zTarg = z;
	material = mat;
	srcSlot = builder_slot;
	type = DEFERRED_BUILD;
}
void DeferredAction::SetDamage(const ushort x, const ushort y, const ushort z)
{
	xTarg = x;
	yTarg = y;
	zTarg = z;
	type = DEFERRED_DAMAGE;
}
void DeferredAction::SetThrow(const ushort x, const ushort y, const ushort z,
		const ushort src, const ushort dest, const ushort n)
{
	xTarg = x;
	yTarg = y;
	zTarg = z;
	srcSlot = src;
	destSlot = dest;
	num = n;
	type = DEFERRED_THROW;
}

void DeferredAction::MakeAction() {
	switch ( type ) {
	case DEFERRED_MOVE:   Move();   break;
	case DEFERRED_JUMP:   Jump();   break;
	case DEFERRED_BUILD:  Build();  break;
	case DEFERRED_DAMAGE: Damage(); break;
	case DEFERRED_THROW:  Throw();  break;
	case DEFERRED_GHOST_MOVE: GhostMove(); break;
	}
	type = DEFERRED_NOTHING;
}

World * DeferredAction::GetWorld() const { return world; }

DeferredAction::DeferredAction(Active * const attached) :
		type(DEFERRED_NOTHING),
		attachedBlock(attached),
		xTarg(), yTarg(), zTarg(),
		material(),
		srcSlot(), destSlot(),
		num()
{}
