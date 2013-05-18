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

#ifndef BLOCKS_H
#define BLOCKS_H

#include <QObject>
#include <QStack>
#include "header.h"

class QDataStream;
class QTextStream;
class World;
class Shred;
class Inventory;
class Active;
class Animal;

const ushort snow_inv_size=2;
const ushort inv_size=26;

enum before_push_action {
	NO_ACTION,
	MOVE_UP,
	JUMP,
	DAMAGE,
	DESTROY,
	MOVE_SELF
}; //enum before_push_action

enum ACTIVE_FREQUENCY {
	NEVER,
	FREQUENT,
	FREQUENT_AND_RARE,
	RARE
}; //enum ACTIVE_FREQUENCY

enum WEARABLE {
	WEARABLE_NOWHERE,
	WEARABLE_HEAD,
	WEARABLE_ARM,
	WEARABLE_BODY,
	WEARABLE_LEGS
}; //enum WEARABLE

//weights in measures - mz (mezuro)
	const ushort WEIGHT_NULLSTONE=1000;
	const ushort WEIGHT_SAND=100;
	const ushort WEIGHT_WATER=50;
	const ushort WEIGHT_GLASS=150;
	const ushort WEIGHT_IRON=300;
	const ushort WEIGHT_GREENERY=3;
	const ushort WEIGHT_MINIMAL=1;
	const ushort WEIGHT_STONE=200;
	const ushort WEIGHT_AIR=0;

class Block { //blocks without special physics and attributes
	void SetTransparency(quint8 transp);

	protected:
	quint8 transparent;
	quint16 sub;
	quint8 direction;
	QString * note;
	qint16 durability;

	public:
	virtual QString & FullName(QString & str) const;
	virtual int Kind() const;
	virtual bool Catchable() const;
	virtual bool CanBeOut() const;
	virtual int Movable() const;
	///Returns false if block cannot be inscribed, otherwise true.
	virtual void Inscribe(const QString & str);
	virtual int BeforePush(int dir, Block * who);
	virtual int Move(int);
	virtual usage_types Use();
	virtual int Damage(ushort dmg, int dmg_kind);
	virtual Block * DropAfterDamage() const;

	virtual Inventory * HasInventory();
	virtual Animal * IsAnimal();
	virtual Active * ActiveBlock();

	virtual bool Armour() const;
	virtual int Wearable() const;
	virtual int DamageKind() const;
	virtual ushort DamageLevel() const;

	virtual uchar LightRadius() const;
	virtual int Temperature() const;
	virtual ushort Weight() const;
	///Receive text signal.
	virtual void ReceiveSignal(const QString &);

	protected:
	virtual void SaveAttributes(QDataStream &) const;

	public:
	void Restore();
	void SetDir(int dir);

	int GetDir() const;
	int Sub() const;
	short Durability() const;
	QString & GetNote(QString & str) const;
	int Transparent() const;

	bool operator==(const Block &) const;

	void SaveToFile(QDataStream & out) const;

	Block(int sb=STONE, quint8 transp=UNDEF);
	Block(
			QDataStream &,
			int sub_,
			quint8 transp=UNDEF);
	virtual ~Block();
}; //class Block

class Plate : public Block {
	public:
	QString & FullName(QString & str) const;
	int Kind() const;
	Block * DropAfterDamage() const;
	int BeforePush(int dir, Block * who);
	ushort Weight() const;

	Plate(int sub);
	Plate(QDataStream & str, int sub);
}; //class Plate

class Ladder : public Block {
	public:
	QString & FullName(QString & str) const;
	int Kind() const;
	Block * DropAfterDamage() const;
	int BeforePush(int dir, Block * who);
	ushort Weight() const;
	bool Catchable() const;

	Ladder(int sub);
	Ladder(QDataStream & str, int sub);
}; //class Ladder

class Weapon : public Block {
	public:
	int Kind() const;
	int Wearable() const;
	int BeforePush(int dir, Block * who);
	int DamageKind() const;
	ushort DamageLevel() const;
	ushort Weight() const;
	QString & FullName(QString & str) const;

	Weapon(int sub);
	Weapon(QDataStream & str, int sub);
}; //class Weapon

class Pick : public Weapon {
	public:
	int Kind() const;
	int DamageKind() const;
	ushort DamageLevel() const;
	QString & FullName(QString & str) const;

	Pick(int sub);
	Pick(QDataStream & str, int sub);
}; //class Pick

class Active : public QObject, public Block {
	Q_OBJECT

	quint8 fall_height;
	bool falling;

	protected:
	///coordinates in loaded world zone
	ushort x_self, y_self, z_self;
	Shred * whereShred;

	void SendSignalAround(const QString &) const;

	signals:
	void Moved(int);
	void Destroyed();
	void Updated();
	void ReceivedText(const QString &);

	public:
	World * GetWorld() const;
	bool InBounds(ushort x, ushort y, ushort z=0) const;
	QString & FullName(QString & str) const;
	int Kind() const;

	Active * ActiveBlock();
	int Move(int);
	void SetFalling(bool set);
	bool IsFalling() const;
	///Returns true if block is destroyed.
	bool FallDamage();

	ushort X() const;
	ushort Y() const;
	ushort Z() const;

	virtual void ActFrequent();
	virtual void ActRare();
	virtual int ShouldAct() const;

	int Movable() const;
	virtual bool ShouldFall() const;
	int Damage(ushort dmg, int dmg_kind);
	void ReceiveSignal(const QString &);

	void ReloadToNorth();
	void ReloadToSouth();
	void ReloadToWest();
	void ReloadToEast();

	void SaveAttributes(QDataStream & out) const;

	void Register(Shred *, ushort x, ushort y, ushort z);
	void Unregister();

	Active(int sub, quint8 transp=UNDEF);
	Active(QDataStream & str, int sub, quint8 transp=UNDEF);
	~Active();
}; //class Active

class Animal : public Active {
	Q_OBJECT

	protected:
	quint16 breath;
	quint16 satiation;

	public:
	QString & FullName(QString&) const=0;

	ushort Breath() const;
	ushort Satiation() const;
	virtual int Eat(Block * const)=0;

	void ActRare();
	int  ShouldAct() const;

	void SaveAttributes(QDataStream & out) const;

	Animal * IsAnimal();

	Animal(int sub=A_MEAT);
	Animal(QDataStream & str, int sub);
}; //class Animal

class Inventory {
	static const ushort max_stack_size=9;
	const ushort size;
	QStack<Block *> * inventory;

	public:
	virtual int  Kind() const=0;
	virtual int  Sub() const=0;
	virtual int  Drop(ushort num, Inventory * inv_to);
	virtual int  GetAll(Inventory * from);
	virtual bool Access() const;
	virtual bool Get(Block * block);
	virtual void Pull(ushort num);
	virtual void SaveAttributes(QDataStream & out) const;
	virtual bool MoveInside(ushort num_from, ushort num_to);
	virtual ushort Start() const;
	virtual ushort Weight() const;
	virtual usage_types Use();
	virtual QString & FullName(QString&) const=0;
	virtual Inventory * HasInventory();

	///Returns true if block found its place.
	bool GetExact(Block * block, ushort num);
	int  MiniCraft(ushort num);
	int  InscribeInv(ushort num, const QString & str);
	int  GetInvSub(ushort i) const;
	int  GetInvKind(ushort i) const;
	ushort Size() const;
	ushort GetInvWeight(ushort i) const;
	quint8 Number(ushort i) const;
	Block * ShowBlock(ushort num) const;
	QString & GetInvNote(QString & str, ushort num) const;
	QString & InvFullName(QString & str, ushort i) const;
	QString & NumStr(QString & str, ushort i) const;

	bool HasRoom();
	bool IsEmpty();

	void BeforePush(Block * who);

	//it is not recommended to make inventory size more than 26,
	//because it will not be convenient to deal with inventory
	//in console version.
	Inventory(ushort sz=inv_size);
	Inventory(QDataStream & str, ushort size=inv_size);
	~Inventory();
}; //class Inventory

class Dwarf : public Animal, public Inventory {
	Q_OBJECT

	public:
	static const uchar onHead=0;
	static const uchar inRight=1;
	static const uchar inLeft=2;
	static const uchar onBody=3;
	static const uchar onLegs=4;

	int Kind() const;
	int Sub() const;
	QString & FullName(QString & str) const;
	ushort Weight() const;
	ushort Start() const;
	int DamageKind() const;
	ushort DamageLevel() const;

	int Eat(Block * to_eat);

	Inventory * HasInventory();
	bool Access() const;
	Block * DropAfterDamage() const;
	void Inscribe(const QString & str);
	bool MoveInside(ushort num_from, ushort num_to);

	void SaveAttributes(QDataStream & out) const;

	uchar LightRadius() const;

	Dwarf(int sub=H_MEAT);
	Dwarf(QDataStream & str, int sub);
}; //class Dwarf

class Chest : public Block, public Inventory {
	public:
	int Kind() const;
	int Sub() const;
	QString & FullName(QString & str) const;
	Inventory * HasInventory();

	usage_types Use();
	int BeforePush(int dir, Block * who);

	Block * DropAfterDamage() const;
	ushort Weight() const;

	void SaveAttributes(QDataStream & out) const;

	Chest(int s=WOOD);
	Chest(QDataStream & str, int sub);
}; //class Chest

class Pile : public Active, public Inventory {
	Q_OBJECT

	bool ifToDestroy;

	public:
	int Kind() const;
	int Sub() const;
	QString & FullName(QString & str) const;

	Inventory * HasInventory();
	usage_types Use();
	ushort Weight() const;

	void ActRare();
	int  ShouldAct() const;

	int BeforePush(int, Block * who);
	int Drop(ushort n, Inventory * inv);
	void Pull(ushort num);

	void SaveAttributes(QDataStream & out) const;

	Pile(int sub=DIFFERENT);
	Pile(QDataStream & str, int sub);
}; //class Pile

class Liquid : public Active {
	Q_OBJECT

	//return true if there is water near
	bool CheckWater() const;

	public:
	void ActRare();
	int  ShouldAct() const;
	int  Kind() const;
	int  Movable() const;
	int  Temperature() const;
	QString & FullName(QString & str) const;

	Liquid(int sub=WATER);
	Liquid(QDataStream & str, int sub);
}; //class Liquid

class Grass : public Active {
	Q_OBJECT

	public:
	QString & FullName(QString & str) const;
	void ActRare();
	int  ShouldAct() const;
	int  Kind() const;
	bool ShouldFall() const;
	int BeforePush(int dir, Block * who);


	Grass(int sub=GREENERY);
	Grass(QDataStream & str, int sub);
}; //class Grass

class Bush : public Active, public Inventory {
	Q_OBJECT

	static const ushort bush_size=3;

	public:
	int  Kind() const;
	int  Sub() const;
	int  Movable() const;
	bool ShouldFall() const;
	void ActRare();
	int  ShouldAct() const;
	int  BeforePush(int dir, Block * who);
	ushort Weight() const;

	QString & FullName(QString & str) const;
	usage_types Use();
	Inventory * HasInventory();
	Block * DropAfterDamage() const;

	void SaveAttributes(QDataStream & out) const;

	Bush(int sub=WOOD);
	Bush(QDataStream & str, int sub);
}; //class Bush

class Rabbit : public Animal {
	Q_OBJECT

	float Attractive(int kind) const;

	public:
	int  Kind() const;
	void ActFrequent();
	int  ShouldAct() const;
	int  Eat(Block * to_eat);
	Block * DropAfterDamage() const;
	QString & FullName(QString & str) const;

	Rabbit(int sub=A_MEAT);
	Rabbit(QDataStream & str, int sub);
}; //class Rabbit

class Workbench : public Block, public Inventory {
	static const ushort workbench_size=10;

	void Craft();

	public:
	QString & FullName(QString & str) const;
	int Kind() const;
	ushort Weight() const;
	usage_types Use();
	Block * DropAfterDamage() const;
	Inventory * HasInventory();
	int Sub() const;
	ushort Start() const;
	int Drop(ushort num, Inventory * inv_to);
	bool Get(Block * block);
	int GetAll(Inventory * from);

	void SaveAttributes(QDataStream & out) const;

	Workbench(int sub);
	Workbench(QDataStream & str, int sub);
}; //class Workbench

class Door : public Active {
	Q_OBJECT

	bool shifted;
	bool locked;
	int movable;

	public:
	void ActFrequent();
	int  ShouldAct() const;
	int  Kind() const;
	int  Movable() const;
	int  BeforePush(int dir, Block * who);
	bool ShouldFall() const;
	QString & FullName(QString & str) const;
	usage_types Use();
	Block * DropAfterDamage() const;

	void SaveAttributes(QDataStream & out) const;

	Door(int sub);
	Door(QDataStream & str, int sub);
}; //class Door

class Clock : public Active {
	QTextStream * txtStream;
	short alarmTime;
	short timerTime;

	public:

	void ActRare();
	int  ShouldAct() const;
	int  Kind() const;
	int  Movable() const;
	bool ShouldFall() const;
	int  BeforePush(int dir, Block * who);
	void Inscribe(const QString & str);
	ushort Weight() const;
	Block * DropAfterDamage() const;
	usage_types Use();
	QString & FullName(QString & str) const;

	Clock(int sub);
	Clock (QDataStream & str, int sub);
	~Clock();
}; //class Clock

#endif
