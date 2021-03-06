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
#include <QDataStream>
#include "Shred.h"
#include "world.h"
#include "BlockManager.h"
#include "Xyz.h"
#include "Active.h"

// Qt version in Debian stable that time.
const quint8 DATASTREAM_VERSION = QDataStream::Qt_4_6;

const ushort SHRED_WIDTH_SHIFT = 4;

/// Get local coordinate.
ushort Shred::CoordInShred(const ushort x) { return x & 0xF; }
/// Get shred coordinate in loaded zone (from 0 to numShreds).
ushort Shred::CoordOfShred(const ushort x) { return x >> SHRED_WIDTH_SHIFT; }

long Shred::Longitude() const { return longitude; }
long Shred::Latitude()  const { return latitude; }
ushort Shred::ShredX() const { return shredX; }
ushort Shred::ShredY() const { return shredY; }
World * Shred::GetWorld() const { return world; }

bool Shred::LoadShred() {
	QByteArray * const data=GetWorld()->GetShredData(longitude, latitude);
	if ( !data ) {
		return false;
	} // else:
	QDataStream in(*data);
	quint8 version;
	in >> version;
	if ( Q_UNLIKELY(DATASTREAM_VERSION != version) ) {
		fprintf(stderr, "Wrong version: %d\nGenerating new shred.\n",
			DATASTREAM_VERSION);
		return false;
	} // else:
	in.setVersion(DATASTREAM_VERSION);
	for (ushort x=0; x<SHRED_WIDTH; ++x)
	for (ushort y=0; y<SHRED_WIDTH; ++y) {
		PutNormalBlock(NULLSTONE, x, y, 0);
		lightMap[x][y][0] = 0;
		for (ushort z=1; ; ++z) {
			quint8 kind, sub;
			const bool normal = block_manager.
				KindSubFromFile(in, kind, sub);
			if ( sub==SKY || sub==STAR ) {
				for ( ; z < HEIGHT-1; ++z) {
					PutNormalBlock(AIR, x, y, z);
					lightMap[x][y][z]=0;
				}
				PutNormalBlock(sub, x, y, HEIGHT-1);
				lightMap[x][y][HEIGHT-1] = 1;
				break;
			} else if ( normal ) {
				lightMap[x][y][z] = 0;
				PutNormalBlock(sub, x, y, z);
			} else {
				lightMap[x][y][z] = 0;
				SetBlock(block_manager.BlockFromFile(
					in, kind, sub), x, y, z);
			}
		}
	}
	return true;
} // bool Shred::LoadShred()

Shred::Shred(const ushort shred_x, const ushort shred_y,
		const long longi, const long lati, Shred * const mem)
	:
		longitude(longi), latitude(lati),
		shredX(shred_x), shredY(shred_y),
		memory(mem)
{
	if ( LoadShred() ) { // successfull loading
		return;
	} // else:
	// new shred generation:
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		PutNormalBlock(NULLSTONE, i, j, 0);
		lightMap[i][j][0]=0;
		for (ushort k=1; k<HEIGHT-1; ++k) {
			PutNormalBlock(AIR, i, j, k);
			lightMap[i][j][k]=0;
		}
		PutNormalBlock(( (qrand()%5) ? SKY : STAR ), i, j, HEIGHT-1);
		lightMap[i][j][HEIGHT-1]=1;
	}
	switch ( TypeOfShred(longi, lati) ) {
	default: fprintf(stderr, "Shred::Shred: unlisted type: %c, code %d\n",
		TypeOfShred(longi, lati), int(TypeOfShred(longi, lati)));
	// no break;
	case SHRED_PLAIN:     Plain(); break;
	case SHRED_TESTSHRED: TestShred(); break;
	case SHRED_PYRAMID:   Pyramid(); break;
	case SHRED_HILL:      Hill(); break;
	case SHRED_DESERT:    Desert(); break;
	case SHRED_WATER:     Water(); break;
	case SHRED_FOREST:    Forest(); break;
	case SHRED_MOUNTAIN:  Mountain(); break;
	case SHRED_EMPTY:     /* empty shred */ break;
	case SHRED_CHAOS:     ChaosShred(); break;
	case SHRED_NULLMOUNTAIN: NullMountain(); break;
	case SHRED_NORMAL_UNDERGROUND: NormalUnderground(); break;
	}
} // Shred::Shred(ushort shred_x, shred_y, long longi, lati, Shred * mem)

Shred::~Shred() {
	const long mapSize = GetWorld()->MapSize();
	if (
			(longitude < mapSize) && (longitude >= 0) &&
			(latitude  < mapSize) && (latitude  >= 0) )
	{
		QByteArray * const shred_data = new QByteArray();
		shred_data->reserve(70000);
		QDataStream outstr(shred_data, QIODevice::WriteOnly);
		outstr << DATASTREAM_VERSION;
		outstr.setVersion(DATASTREAM_VERSION);
		for (ushort x=0; x<SHRED_WIDTH; ++x)
		for (ushort y=0; y<SHRED_WIDTH; ++y) {
			ushort height = HEIGHT-2;
			for ( ; blocks[x][y][height]->Sub()==AIR; --height);
			for (ushort z=1; z <= height; ++z) {
				blocks[x][y][z]->SaveToFile(outstr);
				block_manager.DeleteBlock(blocks[x][y][z]);
			}
			blocks[x][y][HEIGHT-1]->SaveToFile(outstr);
		}
		GetWorld()->SetShredData(shred_data, longitude, latitude);
	} else {
		for (ushort x=0; x<SHRED_WIDTH; ++x)
		for (ushort y=0; y<SHRED_WIDTH; ++y) {
			ushort height = HEIGHT-2;
			for ( ; blocks[x][y][height]->Sub()==AIR; --height);
			for (ushort z=1; z<=height; ++z) {
				block_manager.DeleteBlock(blocks[x][y][z]);
			}
		}
	}
} // Shred::~Shred()

Shred * Shred::GetShredMemory() const { return memory; }

long Shred::GlobalX(const ushort x) const {
	return (Latitude()  - CoordOfShred(x))*SHRED_WIDTH + x;
}
long Shred::GlobalY(const ushort y) const {
	return (Longitude() - CoordOfShred(y))*SHRED_WIDTH + y;
}

void Shred::PhysEventsFrequent() {
	// falling
	for (QLinkedList<Active *>::iterator i = fallList.begin();
			i != fallList.end();)
	{
		if ( (*i)->GetShred() != this  ) {
			i = fallList.erase(i);
			continue;
		} // else:
		const ushort weight=(*i)->Weight();
		if ( weight ) {
			const ushort x=(*i)->X();
			const ushort y=(*i)->Y();
			const ushort z=(*i)->Z();
			if ( GetWorld()->GetBlock(x,y, z-1)->Kind()==LIQUID ) {
				(*i)->SetFalling(false);
				i = fallList.erase(i);
			} else if ( weight <= GetBlock(CoordInShred(x),
						CoordInShred(y), z-1)->Weight()
					|| !GetWorld()->Move(x, y, z, DOWN) )
			{
				(*i)->FallDamage();
				(*i)->SetFalling(false);
				i = fallList.erase(i);
				GetWorld()->DestroyAndReplace(x, y, z);
				GetWorld()->DestroyAndReplace(x, y, z-1);
			} else {
				++i;
			}
		} else {
			(*i)->SetFalling(false);
			i = fallList.erase(i);
		}
	}
	// frequent actions
	for (QLinkedList<Active *>::const_iterator j =
				activeListFrequent.constBegin();
			j != activeListFrequent.constEnd(); ++j)
	{
		if ( (*j)->GetShred() == this  ) {
			(*j)->ActFrequent();
		}
	}
	UnregisterExternalActives();
} // void Shred::PhysEventsFrequent()

void Shred::DeleteDestroyedActives() {
	for (QLinkedList<Active *>::iterator i = deleteList.begin();
			i != deleteList.end(); i = deleteList.erase(i))
	{
		Unregister(*i);
		delete *i;
	}
}

void Shred::PhysEventsRare() {
	for (QLinkedList<Active *>::const_iterator i =
				activeListRare.constBegin();
			i != activeListRare.constEnd(); ++i)
	{
		if ( (*i)->GetShred()==this  ) {
			(*i)->ActRare();
			GetWorld()->DestroyAndReplace((*i)->X(), (*i)->Y(),
				(*i)->Z());
		}
	}
	UnregisterExternalActives();
}

void Shred::UnregisterExternalActives() {
	for (QLinkedList<Active *>::iterator i = activeListAll.begin();
			i != activeListAll.end();)
	{
		if ( (*i)->GetShred() == this ) {
			++i;
		} else {
			Active * const erase = *i;
			--i;
			Unregister(erase);
		}
	}
}

int Shred::Sub(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Sub();
}

void Shred::Register(Active * const active) {
	activeListAll.append(active);
	switch ( active->ShouldAct() ) {
	case FREQUENT:          activeListFrequent.append(active); break;
	case FREQUENT_AND_RARE: activeListFrequent.append(active); // no break;
	case RARE:              activeListRare.append(active); break;
	}
	AddFalling(active);
	AddShining(active);
}
void Shred::Unregister(Active * const active) {
	activeListAll     .removeAll(active);
	activeListFrequent.removeAll(active);
	activeListRare    .removeAll(active);
	fallList          .removeAll(active);
	RemShining(active);
}

void Shred::AddFalling(Active * const active) {
	const Block * block;
	if ( !active->IsFalling() &&
			active->ShouldFall() &&
			ENVIRONMENT==( block=GetBlock(
				CoordInShred(active->X()),
				CoordInShred(active->Y()), active->Z()-1) )->
					Movable() &&
			!(*active == *block) )
	{
		active->SetFalling(true);
		fallList.append(active);
	}
}

void Shred::AddFalling(const ushort x, const ushort y, const ushort z) {
	Active * const active = blocks[x][y][z]->ActiveBlock();
	if ( active ) {
		AddFalling(active);
	}
}

void Shred::AddShining(Active * const active) {
	if ( active->LightRadius() ) {
		shiningList.append(active);
	}
}

void Shred::RemShining(Active * const active) {
	shiningList.removeAll(active);
}

void Shred::AddToDelete(Active * const active) { deleteList.append(active); }

QLinkedList<Active *>::const_iterator Shred::ShiningBegin() const {
	return shiningList.constBegin();
}
QLinkedList<Active *>::const_iterator Shred::ShiningEnd() const {
	return shiningList.constEnd();
}

void Shred::ReloadToNorth() {
	for (QLinkedList<Active*>::const_iterator i=activeListAll.constBegin();
			i != activeListAll.constEnd(); ++i)
	{
		(*i)->ReloadToNorth();
	}
	++shredY;
}
void Shred::ReloadToEast() {
	for (QLinkedList<Active*>::const_iterator i=activeListAll.constBegin();
			i != activeListAll.constEnd(); ++i)
	{
		(*i)->ReloadToEast();
	}
	--shredX;
}
void Shred::ReloadToSouth() {
	for (QLinkedList<Active*>::const_iterator i=activeListAll.constBegin();
			i != activeListAll.constEnd(); ++i)
	{
		(*i)->ReloadToSouth();
	}
	--shredY;
}
void Shred::ReloadToWest() {
	for (QLinkedList<Active*>::const_iterator i=activeListAll.constBegin();
			i != activeListAll.constEnd(); ++i)
	{
		(*i)->ReloadToWest();
	}
	++shredX;
}

Block * Shred::GetBlock(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z];
}

void Shred::SetBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	Active * const active = ( blocks[x][y][z]=block )->ActiveBlock();
	if ( active ) {
		active->SetXYZ( (ShredX() << SHRED_WIDTH_SHIFT) + x,
			(ShredY() << SHRED_WIDTH_SHIFT) + y, z );
		Register(active);
	}
}

void Shred::SetNewBlock(const int kind, const int sub,
		const ushort x, const ushort y, const ushort z, const int dir)
{
	Block * const to_delete = GetBlock(x, y, z);
	Active * const active = to_delete->ActiveBlock();
	if ( active ) {
		Unregister(active);
	}
	block_manager.DeleteBlock(to_delete);
	Block * const block = block_manager.NewBlock(kind, sub);
	block->SetDir(dir);
	SetBlock(block, x, y, z);
}

void Shred::PutBlock(Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	blocks[x][y][z] = block;
}

void Shred::PutNormalBlock(const int sub,
		const ushort x, const ushort y, const ushort z)
{
	blocks[x][y][z] = Normal(sub);
}

Block * Shred::Normal(const int sub) { return block_manager.NormalBlock(sub); }

QString Shred::FileName() const {
	return FileName(GetWorld()->WorldName(), longitude, latitude);
}

QString Shred::FileName(const QString world_name,
		const long longi, const long lati)
{
	return QString("%1/y%2x%3").arg(world_name).arg(longi).arg(lati);
}

char Shred::TypeOfShred(const long longi, const long lati) const {
	return GetWorld()->TypeOfShred(longi, lati);
}

// shred generators section
// these functions fill space between the lowest nullstone layer and sky.
// so use k from 1 to heigth-2.
void Shred::CoverWith(const int kind, const int sub) {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		ushort k = HEIGHT-2;
		for ( ; AIR==Sub(i, j, k); --k);
		SetNewBlock(kind, sub, i, j, ++k);
	}
}

void Shred::RandomDrop(const ushort num, const int kind, const int sub,
		const bool on_water)
{
	for (ushort i=0; i<num; ++i) {
		const int rand = qrand();
		const ushort x = CoordInShred(rand);
		const ushort y = CoordInShred(rand >> SHRED_WIDTH_SHIFT);
		ushort z = HEIGHT-2;
		for ( ; Sub(x, y, z)==AIR; --z);
		if( on_water || Sub(x, y, z)!=WATER ) {
			SetNewBlock(kind, sub, x, y, ++z);
		}
	}
}

void Shred::PlantGrass() {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		ushort k;
		for (k=HEIGHT-2; GetBlock(i, j, k)->Transparent(); --k);
		if ( SOIL==Sub(i, j, k++) && AIR==Sub(i, j, k) ) {
			SetNewBlock(GRASS, GREENERY, i, j, k);
		}
	}
}

void Shred::TestShred() {
	const ushort level=FlatUndeground()+1;
	short row=1, column=-1;
	// row 1
	SetNewBlock(CLOCK, IRON, column+=2, row, level);
	SetNewBlock(CHEST, WOOD, column+=2, row, level);
	SetNewBlock(ACTIVE, SAND, column+=2, row, level);
	PutNormalBlock(GLASS, column+=2, 1, level);
	SetNewBlock(PILE, DIFFERENT, column+=2, row, level);
	SetNewBlock(PLATE, STONE, column+=2, row, level);
	PutNormalBlock(NULLSTONE, column+=2, row, level);
	// row 2
	column=-1;
	row+=2;
	SetNewBlock(LADDER, NULLSTONE, column+=2, row, level);
	// tall ladder
	for (ushort i=level+1; i<=level+5 && i<HEIGHT-1; ++i) {
		SetNewBlock(LADDER, WOOD, column, row, i);
	}
	SetNewBlock(DWARF, H_MEAT, column+=2, row, level);
	SetNewBlock(LIQUID, WATER, column+=2, row, level - 3);
	SetNewBlock(LIQUID, WATER, column, row, level - 2);
	PutNormalBlock(AIR, column, row, level - 1);
	SetNewBlock(BUSH, WOOD, column+=2, row, level);
	SetNewBlock(RABBIT, A_MEAT, column+=2, row, level - 2);
	PutNormalBlock(AIR, column, row, level - 1);
	SetNewBlock(WORKBENCH, IRON, column+=2, row, level);
	SetNewBlock(DOOR, GLASS, column+=2, row, level);
	blocks[column][row][level]->SetDir(NORTH);
	// row 3
	column=-1;
	row+=2;
	SetNewBlock(WEAPON, IRON, column+=2, row, level);
	SetNewBlock(BLOCK, SAND, column+=2, row, level);
	SetNewBlock(BLOCK, WATER, column+=2, row, level);
	SetNewBlock(ACTIVE, WATER, column+=2, row, level);
	SetNewBlock(DOOR, STONE, column+=2, row, level);
	blocks[9][5][level]->SetDir(NORTH);
	SetNewBlock(BLOCK, CLAY, column+=2, row, level);
	SetNewBlock(LIQUID, STONE, column+=2, row, level-1);
	// row 4
	column=-1;
	row+=2;
	SetNewBlock(TEXT, PAPER, column+=2, row, level);
	GetBlock(column, row, level)->Inscribe(".hidden");
	SetNewBlock(BELL, IRON, column+=2, row, level);
	// suicide booth
	/*for (ushort i=1; i<4; ++i)
	for (ushort j=7; j<10; ++j)
	for (ushort k=level; k<level+5; ++k) {
		if ( k<HEIGHT-1 ) {
			PutNormalBlock(GLASS, i, j, k);
		}
	}
	SetNewBlock(RABBIT, A_MEAT, 2, 8, level);*/
} // void Shred::TestShred()

void Shred::NullMountain() {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		ushort k;
		for (k=1; k<HEIGHT/2; ++k) {
			PutNormalBlock(NULLSTONE, i, j, k);
		}
		for ( ; k<HEIGHT-1; ++k) {
			if ( i==4 || i==5 || j==4 || j==5 ) {
				PutNormalBlock(NULLSTONE, i, j, k);
			}
		}
	}
}

void Shred::Pyramid() { // pyramid by Panzerschrek
	const ushort level=qMin(FlatUndeground(), ushort(HEIGHT-1-16));
	for (ushort z=level+1, dz=0; dz<8; z+=2, ++dz) { // pyramid
		for (ushort x=dz, y=dz; x<(16 - dz); ++x, ++y) {
			blocks[x][dz][z] =
				blocks[x][15-dz][z  ] =
				blocks[x][   dz][z+1] =
				blocks[x][15-dz][z+1] =
			blocks[dz][y][z] =
				blocks[15-dz][y][z  ] =
				blocks[   dz][y][z+1] =
				blocks[15-dz][y][z+1] = Normal(STONE);
		}
	}
	blocks[SHRED_WIDTH/2][0][level+1]=Normal(AIR); // entrance
	for (ushort z=HEIGHT/2-60, dz=0; dz<8; ++dz, ++z) { // room inside
		for (ushort x=1; x<SHRED_WIDTH-1; ++x)
		for (ushort y=1; y<SHRED_WIDTH-1; ++y) {
			blocks[x][y][z]=Normal(AIR);
		}
	}
	for (ushort z=HEIGHT/2-52; z<=level; ++z) { // horizontal tunnel
		blocks[SHRED_WIDTH/2][SHRED_WIDTH/2][z]=Normal(AIR);
	}
}

void Shred::ChaosShred() {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j)
	for (ushort k=1; k<HEIGHT/2; ++k) {
		quint8 kind = qrand() % LAST_KIND;
		quint8 sub  = qrand() % LAST_SUB;
		if ( kind==TELEGRAPH || kind==ANIMAL ) {
			kind = BLOCK;
		}
		if ( sub==AIR || sub==STAR || sub==SUN_MOON || sub==SKY ) {
			sub = STONE;
		}
		SetNewBlock(RABBIT, sub, i, j, k);
	}
}

void Shred::NormalCube(const ushort x_start, const ushort y_start,
		const ushort z_start,
		const ushort x_size, const ushort y_size, const ushort z_size,
		const int sub)
{
	for (ushort x=x_start; x < x_start+x_size; ++x)
	for (ushort y=y_start; y < y_start+y_size; ++y)
	for (ushort z=z_start; z < z_start+z_size; ++z) {
		if ( InBounds(x, y, z) ) {
			PutNormalBlock(sub, x, y, z);
		}
	}
}

bool Shred::Tree(const ushort x, const ushort y, const ushort z,
		const ushort height)
{
	if (
			SHRED_WIDTH<=x+2 ||
			SHRED_WIDTH<=y+2 ||
			HEIGHT-1<=z+height || height<2 )
	{
		return false;
	} // else:
	// check for room
	for (ushort i=x; i<=x+2; ++i)
	for (ushort j=y; j<=y+2; ++j)
	for (ushort k=z; k<z+height; ++k) {
		const quint8 sub = Sub(i, j, k);
		if ( AIR!=sub && WATER!=sub ) {
			return false;
		}
	}
	for (ushort k=z; k < z+height-1; ++k) { // trunk
		PutNormalBlock(WOOD, x+1, y+1, k);
	}
	if ( ENVIRONMENT==GetBlock(x+1, y+1, z-1)->Movable() ) {
		block_manager.DeleteBlock(blocks[x+1][y+1][z-1]);
		PutNormalBlock(WOOD, x+1, y+1, z-1);
	}
	// branches
	const int r = qrand();
	if ( r & 0x1 ) SetNewBlock(BLOCK, WOOD, x,   y+1, z+height/2, WEST);
	if ( r & 0x2 ) SetNewBlock(BLOCK, WOOD, x+2, y+1, z+height/2, EAST);
	if ( r & 0x4 ) SetNewBlock(BLOCK, WOOD, x+1, y,   z+height/2, NORTH);
	if ( r & 0x8 ) SetNewBlock(BLOCK, WOOD, x+1, y+2, z+height/2, SOUTH);
	// leaves
	for (ushort i=x; i<=x+2; ++i)
	for (ushort j=y; j<=y+2; ++j)
	for (ushort k=z+height/2; k<z+height; ++k) {
		if ( AIR == Sub(i, j, k) ) {
			PutNormalBlock(GREENERY, i, j, k);
		}
	}
	return true;
} // bool Shred::Tree(ushort x, ushort y, ushort z, ushort height)

bool Shred::InBounds(const ushort x, const ushort y, const ushort z) const {
	return ( x<SHRED_WIDTH && y<SHRED_WIDTH && z && z<HEIGHT-1 );
}
