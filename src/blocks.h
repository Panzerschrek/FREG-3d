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

const ushort INV_SIZE=26;
const ushort MAX_STACK_SIZE=9;

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

class QDataStream;
class Inventory;
class Active;
class Animal;

class Block { //blocks without special physics and attributes
	quint8 Transparency(quint8 transp, int sub);

	const quint8 transparent;
	const quint8 sub;
	quint8 direction;

	protected:
	QString * note;
	qint16 durability;

	public:
	virtual QString FullName() const;
	virtual quint8 Kind() const;
	virtual bool Catchable() const;
	virtual int  Movable() const;
	virtual void Inscribe(const QString & str);
	virtual int  BeforePush(int dir, Block * who);
	virtual void Move(int direction);
	virtual int  Damage(ushort dmg, int dmg_kind);
	virtual usage_types Use(Block * who=0);
	/// Usually returns new block of the same kind and sub (except glass).
	/** When reimplemented in derivatives, inside it you can create a pile,
	 *  put several blocks in it, and return pile.
	 */
	virtual Block * DropAfterDamage() const;

	virtual Inventory * HasInventory();
	virtual Animal * IsAnimal();
	virtual Active * ActiveBlock();

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
	QString GetNote() const;
	int Transparent() const;

	bool operator==(const Block &) const;

	void SaveToFile(QDataStream & out) const;

	Block(int sb=STONE, quint8 transp=UNDEF);
	Block(QDataStream &, int sub, quint8 transp=UNDEF);
	virtual ~Block();
}; //class Block

class Plate : public Block {
	public:
	QString FullName() const;
	quint8 Kind() const;
	int BeforePush(int dir, Block * who);
	ushort Weight() const;

	Plate(int sub);
	Plate(QDataStream & str, int sub);
}; //class Plate

class Ladder : public Block {
	public:
	QString FullName() const;
	quint8 Kind() const;
	int BeforePush(int dir, Block * who);
	ushort Weight() const;
	bool Catchable() const;
	Block * DropAfterDamage() const;

	Ladder(int sub);
	Ladder(QDataStream & str, int sub);
}; //class Ladder

class Weapon : public Block {
	public:
	quint8 Kind() const;
	int Wearable() const;
	int BeforePush(int dir, Block * who);
	int DamageKind() const;
	ushort DamageLevel() const;
	ushort Weight() const;
	QString FullName() const;

	Weapon(int sub);
	Weapon(QDataStream & str, int sub);
}; //class Weapon

class Pick : public Weapon {
	public:
	quint8 Kind() const;
	int DamageKind() const;
	ushort DamageLevel() const;
	QString FullName() const;

	Pick(int sub);
	Pick(QDataStream & str, int sub);
}; //class Pick

class DeferredAction;
class Shred;
class World;

class Active : public QObject, public Block {
	Q_OBJECT

	quint8 fall_height;
	bool falling;
	DeferredAction * deferredAction;

	///coordinates in loaded world zone
	ushort x_self, y_self, z_self;
	Shred * whereShred;

	protected:
	void SendSignalAround(const QString &) const;

	signals:
	void Moved(int);
	void Destroyed();
	void Updated();
	void ReceivedText(const QString &);

	public:
	Shred * GetShred() const;
	World * GetWorld() const;
	QString FullName() const;
	quint8 Kind() const;

	Active * ActiveBlock();
	void Move(int direction);
	void SetFalling(bool set);
	bool IsFalling() const;
	void FallDamage();

	ushort X() const;
	ushort Y() const;
	ushort Z() const;

	virtual void ActFrequent();
	virtual void ActRare();
	virtual int ShouldAct() const;
	virtual bool ShouldFall() const;

	void SetDeferredAction(DeferredAction *);
	DeferredAction * GetDeferredAction() const;

	int Movable() const;
	int Damage(ushort dmg, int dmg_kind);
	void ReceiveSignal(const QString &);

	void ReloadToNorth();
	void ReloadToSouth();
	void ReloadToWest();
	void ReloadToEast();

	void EmitUpdated();

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	void SetXYZ(ushort x, ushort y, ushort z);
	void Register(Shred *, ushort x, ushort y, ushort z);
	void SetShredNull();
	private:
	void Unregister();

	public:
	Active(int sub, quint8 transp=UNDEF);
	Active(QDataStream & str, int sub, quint8 transp=UNDEF);
	~Active();
}; //class Active

class Animal : public Active {
	Q_OBJECT

	quint8 breath;
	quint16 satiation;

	virtual quint16 NutritionalValue(int sub) const=0;

	public:
	void ActRare();
	int  ShouldAct() const;
	QString FullName() const=0;
	Animal * IsAnimal();

	ushort Breath() const;
	ushort Satiation() const;
	bool Eat(int sub);

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Animal(int sub=A_MEAT);
	Animal(QDataStream & str, int sub);
}; //class Animal

class Inventory {
	const ushort size;
	QStack<Block *> * inventory;

	protected:
	virtual void SaveAttributes(QDataStream & out) const;

	public:
	virtual quint8 Kind() const=0;
	virtual int Sub() const=0;
	///Returns true on success.
	virtual bool Drop(ushort src, ushort dest, ushort num, Inventory * to);
	///Returns true on success.
	virtual bool GetAll(Inventory * from);
	virtual bool Access() const;
	///Returns true on success.
	virtual bool Get(Block * block, ushort start=0);
	virtual void Pull(ushort num);
	virtual void MoveInside(ushort num_from, ushort num_to, ushort num);
	virtual void ReceiveSignal(const QString &)=0;
	virtual ushort Start() const;
	virtual ushort Weight() const;
	virtual QString FullName() const=0;
	virtual Inventory * HasInventory();

	///Returns true if block found its place.
	bool GetExact(Block * block, ushort num);
	///Returns true on success (something has been crafted).
	bool MiniCraft(ushort num);
	void InscribeInv(ushort num, const QString & str);
	int  GetInvSub(ushort i) const;
	int  GetInvKind(ushort i) const;
	ushort Size() const;
	ushort GetInvWeight(ushort i) const;
	quint8 Number(ushort i) const;
	Block * ShowBlock(ushort num) const;
	QString GetInvNote(ushort num) const;
	QString InvFullName(ushort num) const;
	QString NumStr(ushort num) const;

	bool IsEmpty() const;
	bool HasRoom() const;

	void BeforePush(Block * who);

	//it is not recommended to make inventory size more than 26,
	//because it will not be convenient to deal with inventory
	//in console version.
	Inventory(ushort sz=INV_SIZE);
	Inventory(QDataStream & str, ushort size=INV_SIZE);
	~Inventory();
}; //class Inventory

class Dwarf : public Animal, public Inventory {
	Q_OBJECT

	quint8 activeHand;
	uchar lightRadius;
	quint16 NutritionalValue(int sub) const;
	void UpdateLightRadius();

	public:
	static const uchar ON_HEAD=0;
	static const uchar IN_RIGHT=1;
	static const uchar IN_LEFT=2;
	static const uchar ON_BODY=3;
	static const uchar ON_LEGS=4;

	uchar GetActiveHand() const;
	void  SetActiveHand(bool right);

	quint8 Kind() const;
	int Sub() const;
	int ShouldAct() const;
	QString FullName() const;
	ushort Weight() const;
	ushort Start() const;
	int DamageKind() const;
	ushort DamageLevel() const;

	Inventory * HasInventory();
	bool Access() const;
	Block * DropAfterDamage() const;
	void Inscribe(const QString & str);
	void MoveInside(ushort num_from, ushort num_to, ushort num);
	void ReceiveSignal(const QString &);

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	uchar LightRadius() const;

	Dwarf(int sub=H_MEAT);
	Dwarf(QDataStream & str, int sub);
}; //class Dwarf

class Chest : public Block, public Inventory {
	public:
	quint8 Kind() const;
	int  Sub() const;
	int  BeforePush(int dir, Block * who);
	void ReceiveSignal(const QString &);
	QString FullName() const;
	Inventory * HasInventory();
	usage_types Use(Block * who=0);
	ushort Weight() const;

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Chest(int s=WOOD, ushort size=INV_SIZE);
	Chest(QDataStream & str, int sub, ushort size=INV_SIZE);
}; //class Chest

class Pile : public Active, public Inventory {
	Q_OBJECT

	public:
	quint8 Kind() const;
	int  Sub() const;
	void ReceiveSignal(const QString &);
	void ActRare();
	int  ShouldAct() const;
	int  BeforePush(int, Block * who);
	Block * DropAfterDamage() const;
	QString FullName() const;
	Inventory * HasInventory();
	usage_types Use(Block * who=0);
	ushort Weight() const;

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Pile(int sub=DIFFERENT);
	Pile(QDataStream & str, int sub);
}; //class Pile

class Liquid : public Active {
	Q_OBJECT

	///Return true if there is water near.
	bool CheckWater() const;

	public:
	void ActRare();
	int  ShouldAct() const;
	quint8 Kind() const;
	int  Movable() const;
	int  Temperature() const;
	uchar LightRadius() const;
	QString FullName() const;
	Block * DropAfterDamage() const;

	Liquid(int sub=WATER);
	Liquid(QDataStream & str, int sub);
}; //class Liquid

class Grass : public Active {
	Q_OBJECT

	public:
	QString FullName() const;
	void ActRare();
	int  ShouldAct() const;
	quint8 Kind() const;
	bool ShouldFall() const;
	int  BeforePush(int dir, Block * who);
	Block * DropAfterDamage() const;

	Grass(int sub=GREENERY);
	Grass(QDataStream & str, int sub);
}; //class Grass

class Bush : public Active, public Inventory {
	Q_OBJECT

	static const ushort BUSH_SIZE=3;

	public:
	quint8 Kind() const;
	int  Sub() const;
	int  Movable() const;
	bool ShouldFall() const;
	void ActRare();
	int  ShouldAct() const;
	int  BeforePush(int dir, Block * who);
	void ReceiveSignal(const QString &);
	ushort Weight() const;

	QString FullName() const;
	usage_types Use(Block * who=0);
	Inventory * HasInventory();
	Block * DropAfterDamage() const;

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Bush(int sub=WOOD);
	Bush(QDataStream & str, int sub);
}; //class Bush

class Rabbit : public Animal {
	Q_OBJECT

	short Attractive(int sub) const;
	quint16 NutritionalValue(int sub) const;

	public:
	quint8 Kind() const;
	void ActFrequent();
	void ActRare();
	int  ShouldAct() const;
	Block * DropAfterDamage() const;
	QString FullName() const;

	Rabbit(int sub=A_MEAT);
	Rabbit(QDataStream & str, int sub);
}; //class Rabbit

class Workbench : public Chest {
	static const ushort WORKBENCH_SIZE=10;

	void Craft();

	public:
	quint8 Kind() const;
	bool Drop(ushort src, ushort dest, ushort num, Inventory * inv);
	bool Get(Block * block, ushort start=0);
	bool GetAll(Inventory * from);
	void ReceiveSignal(const QString &);
	ushort Start() const;
	QString FullName() const;

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
	quint8 Kind() const;
	int  Movable() const;
	int  BeforePush(int dir, Block * who);
	bool ShouldFall() const;
	QString FullName() const;
	usage_types Use(Block * who=0);

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Door(int sub);
	Door(QDataStream & str, int sub);
}; //class Door

class Clock : public Active {
	short alarmTime;
	short timerTime;

	public:
	void ActRare();
	int  ShouldAct() const;
	quint8 Kind() const;
	int  Movable() const;
	bool ShouldFall() const;
	int  BeforePush(int dir, Block * who);
	void Inscribe(const QString & str);
	ushort Weight() const;
	usage_types Use(Block * who=0);
	QString FullName() const;

	Clock(int sub);
	Clock (QDataStream & str, int sub);
	~Clock();
}; //class Clock

class Creator : public Active, public Inventory {
	Q_OBJECT

	public:
	quint8 Kind() const;
	int  Sub() const;
	void ReceiveSignal(const QString &);
	int  DamageKind() const;
	ushort DamageLevel() const;
	QString FullName() const;
	Inventory * HasInventory();

	protected:
	void SaveAttributes(QDataStream & out) const;

	public:
	Creator(int sub=DIFFERENT);
	Creator(QDataStream & str, int sub);
}; //class Creator

#endif
