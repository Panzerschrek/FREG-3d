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

#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QTextStream>
#include "Shred.h"
#include "world.h"
#include "BlockManager.h"

const ushort SEA_LEVEL    = 58;
const ushort SEABED_LEVEL = 48;
const ushort PLANE_LEVEL  = 64;
const ushort HILL_LEVEL   = 76;
const ushort MOUNTAIN_LEVEL=88;

const float SEABED_AMPLITUDE = 12.0f;
const float PLANE_AMPLITUDE  =  8.0f;
const float HILL_AMPLITUDE   = 29.0f;
const float MOUNTAIN_AMPLITUDE=50.0f;

/*landscape generation:
 * [ level - amplitude; level + amplitude ]
 * faster- [ level - amplitude/2; level + amplitude/2 ]
 * perlin [ -1; 1]
 * h= level + amplitude * perlin
*/

//Qt version in Debian stable that time.
const int datastream_version=QDataStream::Qt_4_6;

float Noise2(const int x, const int y) { //range - [-1;1]
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
		1073741823.5f - 1.0f;
}

float InterpolatedNoise(const short x, const short y) { //range - [-1;1]
	const short X = x >> 6;
	const short Y = y >> 6;

	const float noise[4]={
		Noise2(X, Y),
		Noise2(X + 1, Y),
		Noise2(X + 1, Y + 1),
		Noise2(X, Y + 1)
	};

	const float dx = (float (x) - float (X << 6))*0.015625f;
	const float dy = (float (y) - float (Y << 6))*0.015625f;

	const float interp_x[2]={
		dy * noise[3] + (1.0f - dy) * noise[0],
		dy * noise[2] + (1.0f - dy) * noise[1]
	};
	return interp_x[1] * dx + interp_x[0] * (1.0f - dx);
}

float FinalNoise(short x, short y) { //range [-1;1]  middle= 0
	float r;
	r = 0.5f * InterpolatedNoise(x, y);

	x <<= 1, y <<= 1;
	r += 0.25f * InterpolatedNoise(x, y);

	x <<= 1, y <<= 1;
	r += 0.125f * InterpolatedNoise(x, y);

	return r;
}

void Shred::ShredNominalAmplitudeAndLevel(
		const char shred_type,
		ushort * const l,
		float * const a)
const {
	switch (shred_type) {
		case 't':
		case 'p':
		case '%':
		case '#':
		case ';':
		default:
			*l = PLANE_LEVEL;
			*a = PLANE_AMPLITUDE;
		break;

		case '~':
			*l = SEABED_LEVEL;
			*a = SEABED_AMPLITUDE;
		break;

		case '^':
			*l = MOUNTAIN_LEVEL;
			*a = MOUNTAIN_AMPLITUDE;
		break;
		case '+':
			*l = HILL_LEVEL;
			*a = HILL_AMPLITUDE;
		break;
	}
}

void Shred::ShredLandAmplitudeAndLevel(
		const long longi, const long lati,
		ushort * const l,
		float * const a)
const {
	const char shred_types[8]={
		TypeOfShred(longi + 1, lati),
		TypeOfShred(longi - 1, lati),
		TypeOfShred(longi, lati + 1),
		TypeOfShred(longi, lati - 1),

		TypeOfShred(longi + 1, lati + 1),
		TypeOfShred(longi - 1, lati - 1),
		TypeOfShred(longi - 1, lati + 1),
		TypeOfShred(longi + 1, lati - 1)
	};
	float amplitude, level;
	float a2;
	ushort l2;

	const char this_shred_type = TypeOfShred(longi, lati);
	ShredNominalAmplitudeAndLevel(this_shred_type, &l2, &a2);
	const float this_shred_amplitude = amplitude = a2;
	const float this_shred_level = level = float (l2);
	for (int i = 0; i < 8; i++) {
		if (shred_types[i] != this_shred_type) {
			ShredNominalAmplitudeAndLevel(shred_types[i], &l2,&a2);
			amplitude += (a2 - this_shred_amplitude) * (1.0f/9.0f);
			level += (float (l2) - this_shred_level)*(1.0f / 9.0f);
		}
	}
	*a = amplitude;
	*l = ushort(level);
}

long Shred::Longitude() const { return longitude; }
long Shred::Latitude()  const { return latitude; }
ushort Shred::ShredX() const { return shredX; }
ushort Shred::ShredY() const { return shredY; }

World * Shred::GetWorld() const { return world; }

int Shred::LoadShred(QFile & file) {
	QByteArray read_data=file.readAll();
	QByteArray uncompressed=qUncompress(read_data);
	QDataStream in(uncompressed);
	quint8 version;
	in >> version;
	if ( datastream_version!=version ) {
		fprintf(stderr,
			"Wrong version: %d\nGenerating new shred.\n",
			datastream_version);
		return 1;
	}
	in.setVersion(datastream_version);
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		PutNormalBlock(NULLSTONE, i, j, 0);
		lightMap[i][j][0] = 0;
		for (ushort k=1; k<HEIGHT; ++k) {
			SetBlock(block_manager.BlockFromFile(in),
				 i, j, k);
			lightMap[i][j][k]=0;
		}
		lightMap[i][j][HEIGHT-1]=MAX_LIGHT_RADIUS;
	}
	return 0;
}

Shred::Shred(
		World * const world_,
		const ushort shred_x, const ushort shred_y,
		const long longi, const long lati)
	:
		world(world_),
		longitude(longi),
		latitude(lati),
		shredX(shred_x),
		shredY(shred_y)
{
	activeListFrequent.reserve(100);
	activeListRare.reserve(500);
	activeListAll.reserve(1000);
	fallList.reserve(100);
	QFile file(FileName());
	if ( file.open(QIODevice::ReadOnly) && !LoadShred(file) ) {
		return;
	}
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		PutNormalBlock(NULLSTONE, i, j, 0);
		for (ushort k=1; k<HEIGHT-1; ++k) {
			PutNormalBlock(AIR, i, j, k);
			lightMap[i][j][k]=0;
		}
		PutNormalBlock(( (qrand()%5) ? SKY : STAR ), i, j, HEIGHT-1);
		lightMap[i][j][HEIGHT-1]=MAX_LIGHT_RADIUS;
	}
	switch ( TypeOfShred(longi, lati) ) {
		case '#': NullMountain(); break;
		case '.': Plain();        break;
		case 't': TestShred();    break;
		case 'p': Pyramid();      break;
		case '+': Hill();         break;
		case '^': Mountain();     break;
		case ':': Desert();       break;
		case '%': Forest(longi, lati); break;
		case '~': Water( longi, lati); break;
		case '_': /* empty shred */    break;
		case '-': NormalUnderground(); break;
		default:
			Plain();
			fprintf(stderr,
				"Shred::Shred: unlisted type: %c, code %d\n",
				TypeOfShred(longi, lati),
				int(TypeOfShred(longi, lati)));
	}
}

Shred::~Shred() {
	const long mapSize=world->MapSize();
	if (
			(longitude < mapSize) && (longitude >= 0) &&
			(latitude  < mapSize) && (latitude  >= 0) )
	{
		QFile file(FileName());
		if ( !file.open(QIODevice::WriteOnly) ) {
			fputs("Shred::~Shred: Write Error\n", stderr);
			return;
		}
		QByteArray shred_data;
		shred_data.reserve(200000);
		QDataStream outstr(&shred_data, QIODevice::WriteOnly);
		outstr << (quint8)datastream_version;
		outstr.setVersion(datastream_version);
		for (ushort i=0; i<SHRED_WIDTH; ++i)
		for (ushort j=0; j<SHRED_WIDTH; ++j)
		for (ushort k=1; k<HEIGHT; ++k) {
			blocks[i][j][k]->SaveToFile(outstr);
			block_manager.DeleteBlock(blocks[i][j][k]);
		}
		file.write(qCompress(shred_data));
		return;
	}
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j)
	for (ushort k=1; k<HEIGHT-1; ++k) {
		block_manager.DeleteBlock(blocks[i][j][k]);
	}
}

void Shred::SetNewBlock(
		const int kind, const int sub,
		const ushort x, const ushort y, const ushort z)
{
	block_manager.DeleteBlock(blocks[x][y][z]);
	SetBlock( block_manager.NewBlock(kind, sub), x, y, z );
}

void Shred::RegisterBlock(
		Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	Active * const active=block->ActiveBlock();
	if ( active ) {
		active->Register(this,
			SHRED_WIDTH*shredX+x,
			SHRED_WIDTH*shredY+y, z);
	}
}

void Shred::PhysEventsFrequent() {
	for (int j=0; j<fallList.size(); ++j) {
		Active * const temp=fallList[j];
		const ushort weight=temp->Weight();
		if ( weight ) {
			const ushort x=temp->X();
			const ushort y=temp->Y();
			const ushort z=temp->Z();
			if ( weight > Weight(
					x%SHRED_WIDTH,
					y%SHRED_WIDTH, z-1) )
			{
				if ( !world->Move(x, y, z, DOWN) ) {
					RemFalling(temp);
					temp->FallDamage();
				}
			} else {
				RemFalling(temp);
				temp->FallDamage();
			}
		}
	}
	for (int j=0; j<activeListFrequent.size(); ++j) {
		activeListFrequent[j]->ActFrequent();
	}
}
void Shred::PhysEventsRare() {
	for (int j=0; j<activeListRare.size(); ++j) {
		activeListRare[j]->ActRare();
	}
}

int Shred::Sub(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Sub();
}
int Shred::Kind(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Kind();
}
int Shred::Durability(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Durability();
}
int Shred::Movable(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Movable();
}
int Shred::Transparent(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Transparent();
}
ushort Shred::Weight(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z]->Weight();
}

void Shred::AddActive(Active * const active) {
	activeListAll.append(active);
	switch ( active->ShouldAct() ) {
		case FREQUENT:
			activeListFrequent.append(active);
		break;
		case FREQUENT_AND_RARE:
			activeListFrequent.append(active);
		//no break;
		case RARE:
			activeListRare.append(active);
		break;
	}
}

void Shred::RemActive(Active * const active) {
	activeListAll.removeOne(active);
	activeListFrequent.removeOne(active);
	activeListRare.removeOne(active);
}

void Shred::AddFalling(Active * const active) {
	if ( !active->IsFalling() && active->ShouldFall() ) {
		active->SetFalling(true);
		fallList.append(active);
	}
}

void Shred::RemFalling(Active * const active) {
	fallList.removeOne(active);
	active->SetFalling(false);
}

void Shred::AddFalling(const ushort x, const ushort y, const ushort z) {
	Active * const active=blocks[x][y][z]->ActiveBlock();
	if ( active ) {
		AddFalling(active);
	}
}

void Shred::ReloadToNorth() {
	for (ushort i=0; i<activeListAll.size(); ++i) {
		activeListAll[i]->ReloadToNorth();
	}
	++shredY;
}
void Shred::ReloadToEast() {
	for (ushort i=0; i<activeListAll.size(); ++i) {
		activeListAll[i]->ReloadToEast();
	}
	--shredX;
}
void Shred::ReloadToSouth() {
	for (ushort i=0; i<activeListAll.size(); ++i) {
		activeListAll[i]->ReloadToSouth();
	}
	--shredY;
}
void Shred::ReloadToWest() {
	for (ushort i=0; i<activeListAll.size(); ++i) {
		activeListAll[i]->ReloadToWest();
	}
	++shredX;
}

Block *Shred::GetBlock(const ushort x, const ushort y, const ushort z) const {
	return blocks[x][y][z];
}

void Shred::SetBlock(
		Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	blocks[x][y][z]=block;
	RegisterBlock(block, x, y, z);
}

void Shred::PutBlock(
		Block * const block,
		const ushort x, const ushort y, const ushort z)
{
	blocks[x][y][z]=block;
}

void Shred::PutNormalBlock(
		const int sub,
		const ushort x, const ushort y, const ushort z,
		const int dir)
{
	blocks[x][y][z]=Normal(sub, dir);
}

Block * Shred::Normal(const int sub, const int dir) {
	return block_manager.NormalBlock(sub, dir);
}

QString Shred::FileName() const {
	return world->WorldName()+"/y"+
		QString::number(longitude)+"x"+
		QString::number(latitude);
}

char Shred::TypeOfShred(const long longi, const long lati) const {
	const long mapSize=world->MapSize();
	if (
			longi >= mapSize || longi < 0 ||
			lati  >= mapSize || lati  < 0 )
	{
		return OUT_BORDER_SHRED;
	}
	if ( !world->MapStream()->seek((mapSize+1)*longi+lati) ) {
		return DEFAULT_SHRED;
	}
	char c;
	*world->MapStream() >> c;
	return c;
}

void Shred::AddWater() {
	for (long longi=longitude-1; longi<=longitude+1; ++longi)
	for (long lati =latitude -1; lati <=latitude +1; ++lati ) {
		if ( '~'==TypeOfShred(longi, lati) ) {
			for (ushort i = 0; i < SHRED_WIDTH; i++)
			for (ushort j = 0; j < SHRED_WIDTH; j++) {
				for (ushort k = SEA_LEVEL; true; k--) {
					if ( Sub(i, j, k)!=AIR ) {
						break;
					}
					SetNewBlock(LIQUID, WATER, i, j, k);
				}
			}
			return;
		}
	}
}

ushort Shred::FlatUndeground(short depth) {
	if ( FLAT_GENERATION ) {
		NormalUnderground();
		return HEIGHT/2;
	}
	ushort level;
	float amplitude;
	ShredLandAmplitudeAndLevel(longitude, latitude, &level, &amplitude);

	if ( level >= HEIGHT-2 ) {
		level=HEIGHT-2;
	}
	for (ushort i = 0; i < SHRED_WIDTH; i++)
	for (ushort j = 0; j < SHRED_WIDTH; j++) {
		ushort k;
		for (k = 1; k <= level - depth - 6; k++) {
			PutNormalBlock(STONE, i, j, k);
		}
		for (; k <= level; ++k) {
			PutNormalBlock(SOIL, i, j, k);
		}
	}
	return level;
}

//shred generators section
//these functions fill space between the lowest nullstone layer and sky.
//so use k from 1 to heigth-2.
void Shred::NormalUnderground(const ushort depth, const int sub) {
	if ( FLAT_GENERATION ) {
		for (ushort x=0; x<SHRED_WIDTH; ++x)
		for (ushort y=0; y<SHRED_WIDTH; ++y) {
			ushort z=1;
			for ( ; z<HEIGHT/2-depth-6; ++z) {
				PutNormalBlock(STONE, x, y, z);
			}
			PutNormalBlock((qrand()%2 ? STONE : sub), x, y, z++);
			for ( ; z<=HEIGHT/2-depth; ++z) {
				PutNormalBlock(sub, x, y, z);
			}
		}
		return;
	}
	float amplitude[9];
	ushort level[9];
	float interp_level, interp_amplitude;
	float il[4], ia[4];
	float ik_x, ik_y;

	ShredLandAmplitudeAndLevel(longitude, latitude, &level[8],
		&amplitude[8]);
	ShredLandAmplitudeAndLevel(longitude + 1, latitude, &level[0],
		&amplitude[0]);
	ShredLandAmplitudeAndLevel(longitude - 1, latitude, &level[1],
		&amplitude[1]);
	ShredLandAmplitudeAndLevel(longitude, latitude + 1, &level[2],
		&amplitude[2]);
	ShredLandAmplitudeAndLevel(longitude, latitude - 1, &level[3],
		&amplitude[3]);
	ShredLandAmplitudeAndLevel(longitude + 1, latitude - 1, &level[4],
		&amplitude[4]);
	ShredLandAmplitudeAndLevel(longitude + 1, latitude + 1, &level[5],
		&amplitude[5]);
	ShredLandAmplitudeAndLevel(longitude - 1, latitude + 1, &level[6],
		&amplitude[6]);
	ShredLandAmplitudeAndLevel(longitude - 1, latitude - 1, &level[7],
		&amplitude[7]);
	/*
	   7 1 6
	   3 8 2
	   4 0 5
	 */
	ushort h;
	ushort k;
	short dirt_h;
	for (ushort i = 0; i < SHRED_WIDTH; ++i)
	for (ushort j = 0; j < SHRED_WIDTH; ++j) {
		/*
		 * 3+---+2
		 *  |...|
                 *  |...|
		 * 0+---+1
		 */
		//interpolate land amplitude and level
		if (j < SHRED_WIDTH / 2 && i < SHRED_WIDTH / 2) {
			il[0] = float (level[3]), ia[0] = amplitude[3];
			il[1] = float (level[8]), ia[1] = amplitude[8];
			il[2] = float (level[1]), ia[2] = amplitude[1];
			il[3] = float (level[7]), ia[3] = amplitude[7];
			ik_x = float (i + SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j + SHRED_WIDTH / 2) * 0.0625f;
		} else if (j < SHRED_WIDTH / 2 && i >= SHRED_WIDTH / 2) {
			il[0] = float (level[8]), ia[0] = amplitude[8];
			il[1] = float (level[2]), ia[1] = amplitude[2];
			il[2] = float (level[6]), ia[2] = amplitude[6];
			il[3] = float (level[1]), ia[3] = amplitude[1];
			ik_x = float (i - SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j + SHRED_WIDTH / 2) * 0.0625f;
		} else if (j >= SHRED_WIDTH / 2 && i < SHRED_WIDTH / 2) {
			il[0] = float (level[4]), ia[0] = amplitude[4];
			il[1] = float (level[0]), ia[1] = amplitude[0];
			il[2] = float (level[8]), ia[2] = amplitude[8];
			il[3] = float (level[3]), ia[3] = amplitude[3];
			ik_x = float (i + SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j - SHRED_WIDTH / 2) * 0.0625f;
		} else {
			il[0] = float (level[0]), ia[0] = amplitude[0];
			il[1] = float (level[5]), ia[1] = amplitude[5];
			il[2] = float (level[2]), ia[2] = amplitude[2];
			il[3] = float (level[8]), ia[3] = amplitude[8];
			ik_x = float (i - SHRED_WIDTH / 2) * 0.0625f;
			ik_y = float (j - SHRED_WIDTH / 2) * 0.0625f;
		}
		il[0] = il[0] * ik_y + (1.0f - ik_y) * il[3];
		il[1] = il[1] * ik_y + (1.0f - ik_y) * il[2];
		interp_level = il[0] * (1.0f - ik_x) + il[1] * ik_x;

		ia[0] = ia[0] * ik_y + (1.0f - ik_y) * ia[3];
		ia[1] = ia[1] * ik_y + (1.0f - ik_y) * ia[2];
		interp_amplitude = ia[0] * (1.0f - ik_x) + ia[1] * ik_x;

		h = short (interp_level +
			   FinalNoise(latitude * 16 + i,
				      longitude * 16 +
				      j) * interp_amplitude) - depth;
		if( h >= HEIGHT - 1 ) {
            		h= HEIGHT - 2;
		} else if ( h < 2 ) {
			h = 2;
		}
		if (h < 80) {
			dirt_h = 5;
			dirt_h+= short( 3.0f * FinalNoise(
				( latitude *16+i )*4,
				( longitude*16+j )*4 ) );
		} else if (h < 100) {
			dirt_h = (100 - h) * 6 / 20;
			dirt_h+= short( 3.0f * FinalNoise(
				( latitude *16+i )*4,
				( longitude*16+j )*4 ) );
			if( dirt_h < 0 ) {
				dirt_h = 0;
			}
		} else {
			dirt_h = 0;
		}
		
		for (k = 1; k < h - dirt_h && h < HEIGHT - 1; ++k) {
			PutNormalBlock(STONE, i, j, k);
		}
		for (; k < h; ++k) {
			PutNormalBlock(sub, i, j, k);
		}
	}
}

void Shred::CoverWith(const int kind, const int sub) {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		ushort k=HEIGHT-2;
		for ( ; AIR==Sub(i, j, k); --k);
		SetNewBlock(kind, sub, i, j, ++k);
	}
}

void Shred::RandomDrop(const ushort num,
		const int kind, const int sub,
		const bool on_water)
{
	for (ushort i=0; i<num; ++i) {
		ushort x=qrand()%SHRED_WIDTH;
		ushort y=qrand()%SHRED_WIDTH;
		for (ushort z=HEIGHT-2; z>0; --z) {
			if ( Sub(x, y, z)!=AIR ) {
				if( on_water || Sub(x, y, z)!=WATER ) {
					SetNewBlock(kind, sub, x, y, z+1);
				}
				break;
			}

		}
	}
}

void Shred::PlantGrass() {
	for (ushort i=0; i<SHRED_WIDTH; ++i)
	for (ushort j=0; j<SHRED_WIDTH; ++j) {
		ushort k;
		for (k=HEIGHT-2; Transparent(i, j, k); --k);
		if ( SOIL==Sub(i, j, k++) && AIR==Sub(i, j, k) ) {
			SetNewBlock(GRASS, GREENERY, i, j, k);
		}
	}
}

void Shred::TestShred() {
	ushort level=FlatUndeground()+1;
	//row 1
	SetNewBlock(CLOCK, IRON, 1, 1, level);
	SetNewBlock(CHEST, WOOD, 3, 1, level);
	SetNewBlock(ACTIVE, SAND, 5, 1, level);
	PutNormalBlock(GLASS, 7, 1, level);
	SetNewBlock(PILE, DIFFERENT, 9, 1, level);
	SetNewBlock(PLATE, STONE, 11, 1, level);
	PutNormalBlock(NULLSTONE, 13, 1, level);
	//row 2
	SetNewBlock(LADDER, NULLSTONE, 1, 3, level);
	//tall ladder
	for (ushort i=level+1; i<=level+5 && i<HEIGHT-1; ++i) {
		SetNewBlock(LADDER, WOOD, 1, 3, i);
	}
	SetNewBlock(DWARF, H_MEAT, 3, 3, level);
	SetNewBlock(LIQUID, WATER, 5, 3, level - 3);
	SetNewBlock(LIQUID, WATER, 5, 3, level - 3);
	SetNewBlock(LIQUID, WATER, 5, 3, level - 2);
	PutNormalBlock(AIR, 5, 3, level - 1);
	SetNewBlock(BUSH, GREENERY, 7, 3, level);
	SetNewBlock(RABBIT, A_MEAT, 9, 3, level - 2);
	PutNormalBlock(AIR, 9, 3, level - 1);
	SetNewBlock(WORKBENCH, IRON, 11, 3, level);
	SetNewBlock(DOOR, GLASS, 13, 3, level);
	blocks[13][3][level]->SetDir(NORTH);
	//row 3
	SetNewBlock(WEAPON, IRON, 1, 5, level);
	SetNewBlock(BLOCK, SAND, 3, 5, level);
	SetNewBlock(BLOCK, WATER, 5, 5, level);
	SetNewBlock(ACTIVE, WATER, 7, 5, level);
	SetNewBlock(DOOR, STONE, 9, 5, level);
	blocks[9][5][level]->SetDir(NORTH);
	SetNewBlock(BLOCK, CLAY, 11, 5, level);
	//suicide booth
	/*for (ushort i=1; i<4; ++i)
	for (ushort j=7; j<10; ++j)
	for (ushort k=level; k<level+5; ++k) {
		if ( k<HEIGHT-1 ) {
			PutNormalBlock(GLASS, i, j, k);
		}
	}
	SetNewBlock(RABBIT, A_MEAT, 2, 8, level);*/
}

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

void Shred::Plain() {
	NormalUnderground();
	if ( !FLAT_GENERATION ) {
		AddWater();
	}
	RandomDrop(qrand()%4, BUSH, WOOD);
	RandomDrop(qrand()%4, RABBIT, A_MEAT);
	PlantGrass();
}

void Shred::Forest(const long longi, const long lati) {
	NormalUnderground();
	if ( !FLAT_GENERATION ) {
		AddWater();
	}
	ushort number_of_trees=0;
	for (long i=longi-1; i<=longi+1; ++i)
	for (long j=lati -1; j<=lati +1; ++j) {
		if ( '%'==TypeOfShred(i, j) ) {
			++number_of_trees;
		}
	}
	for (ushort i=0; i<number_of_trees; ++i) {
		const ushort x=qrand()%(SHRED_WIDTH-2);
		const ushort y=qrand()%(SHRED_WIDTH-2);
		for (ushort k=HEIGHT-2; ; --k) {
			const int sub=Sub(x, y, k);
			if ( sub!=AIR && sub!=WATER ) {
				if ( sub!=GREENERY && sub!=WOOD ) {
					Tree(x, y, k+1, 4+qrand()%5);
				}
				break;
			}
		}
	}
	PlantGrass();
}

void Shred::Water(const long, const long) {
	if ( FLAT_GENERATION ) {
		ushort depth=0;
		for (long longi=longitude-1; longi<=longitude+1; ++longi)
		for (long lati =latitude -1; lati <=latitude +1; ++lati ) {
			if ( '~'==TypeOfShred(longi, lati) ) {
				depth+=2;
			}
		}
		NormalUnderground(depth);
		for (ushort i=HEIGHT/2-depth; i<HEIGHT/2; ++i) {
			CoverWith(LIQUID, WATER);
		}
		return;
	}
	NormalUnderground();
	AddWater();
	PlantGrass();
}

void Shred::Pyramid() {
	//pyramid by Panzerschrek
	//'p' - pyramid symbol
	ushort level=FlatUndeground();
	if ( level > 127-16 ) {
		level=127-16;
	}
	ushort z, dz, x, y;
	//пирамида
	for (z=level+1, dz=0; dz<8; z+=2, dz++) {
		for (x=dz; x<(16 - dz); x++) {
			blocks[x][dz][z] =
				blocks[x][15-dz][z]=
				blocks[x][dz][z+1] =
				blocks[x][15-dz][z+1]=Normal(STONE);
		}
		for (y=dz; y<(16-dz); y++) {
			blocks[dz][y][z] =
				blocks[15-dz][y][z]=
				blocks[dz][y][z+1] =
				blocks[15-dz][y][z+1]=Normal(STONE);
		}
	}
	//вход
	blocks[SHRED_WIDTH/2][0][level+1]=Normal(AIR);
	//камера внутри
	for (z=HEIGHT/2-60, dz=0; dz<8; dz++, z++) {
		for (x=1; x<SHRED_WIDTH-1; x++)
		for (y=1; y<SHRED_WIDTH-1; y++) {
			blocks[x][y][z]=Normal(AIR);
		}
	}
	//шахта
	for (z=HEIGHT/2-52; z<=level; z++) {
		blocks[SHRED_WIDTH/2][SHRED_WIDTH/2][z]=Normal(AIR);
	}
}

void Shred::Hill() {
	NormalUnderground();
	if ( FLAT_GENERATION ) {
		ushort x, y, z;
		for (y=0; y<SHRED_WIDTH; ++y) { //north-south '/\'
			for (x=0; x<SHRED_WIDTH; ++x)
			for (z=0; z<SHRED_WIDTH/2-2; ++z) {
				if ( z <= -qAbs(x-SHRED_WIDTH/2)+
						SHRED_WIDTH/2-2 )
				{
					PutNormalBlock(SOIL, x, y, z+HEIGHT/2);
				}
			}
		}
		for (x=0; x<SHRED_WIDTH; ++x) { //east-west '/\'
			for (y=0; y<SHRED_WIDTH; ++y)
			for (z=0; z<SHRED_WIDTH/2-2; ++z) {
				if ( z <= -qAbs(y-SHRED_WIDTH/2)+
						SHRED_WIDTH/2-2 )
				{
					PutNormalBlock(SOIL, x, y, z+HEIGHT/2);
				}
			}
		}
	}
	PlantGrass();
}

void Shred::Mountain() {
	//TODO: add FLAT_GENERATION mountain
	NormalUnderground();
	PlantGrass();
}

void Shred::Desert() {
	NormalUnderground(4, SAND);
	for (ushort i=0; i<4; ++i) {
		CoverWith(ACTIVE, SAND);
	}
	if ( !FLAT_GENERATION ) {
		AddWater();
	}
}

bool Shred::Tree(
		const ushort x, const ushort y, const ushort z,
		const ushort height)
{
	if (
			SHRED_WIDTH<=x+2 ||
			SHRED_WIDTH<=y+2 ||
			HEIGHT-1<=z+height ||
			height<2 )
	{
		return false;
	}
	ushort i, j, k;
	for (i=x; i<=x+2; ++i)
	for (j=y; j<=y+2; ++j)
	for (k=z; k<z+height; ++k) {
		if ( AIR!=Sub(i, j, k) && WATER!=Sub(i, j, k) ) {
			return false;
		}
	}
	for (k=z; k < z + height - 1; ++k) { //trunk
		PutNormalBlock(WOOD, x+1, y+1, k);
	}
	if ( ENVIRONMENT==Movable(x+1, y+1, z-1) ) {
		block_manager.DeleteBlock(blocks[x+1][y+1][z-1]);
		PutNormalBlock(WOOD, x+1, y+1, z-1);
	}
	//branches
	if ( qrand()%2 ) {
		PutNormalBlock(WOOD, x,   y+1, z+height/2, WEST);
	}
	if ( qrand()%2 ) {
		PutNormalBlock(WOOD, x+2, y+1, z+height/2, EAST);
	}
	if ( qrand()%2 ) {
		PutNormalBlock(WOOD, x+1, y, z+height/2, NORTH);
	}
	if ( qrand()%2 ) {
		PutNormalBlock(WOOD, x+1, y+2, z+height/2, SOUTH);
	}
	//leaves
	for (i=x; i<=x+2; ++i)
	for (j=y; j<=y+2; ++j)
	for (k=z+height/2; k<z+height; ++k) {
		if ( AIR==Sub(i, j, k) ) {
			PutNormalBlock(GREENERY, i, j, k);
		}
	}
	return true;
}
