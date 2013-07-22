/****************************************************************************
** Meta object code from reading C++ file 'blocks.h'
**
** Created: Fri 12. Jul 15:28:40 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../blocks.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blocks.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Active[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      19,    7,    7,    7, 0x05,
      31,    7,    7,    7, 0x05,
      41,    7,    7,    7, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Active[] = {
    "Active\0\0Moved(int)\0Destroyed()\0Updated()\0"
    "ReceivedText(QString)\0"
};

void Active::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Active *_t = static_cast<Active *>(_o);
        switch (_id) {
        case 0: _t->Moved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->Destroyed(); break;
        case 2: _t->Updated(); break;
        case 3: _t->ReceivedText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Active::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Active::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Active,
      qt_meta_data_Active, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Active::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Active::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Active::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Active))
        return static_cast<void*>(const_cast< Active*>(this));
    if (!strcmp(_clname, "Block"))
        return static_cast< Block*>(const_cast< Active*>(this));
    return QObject::qt_metacast(_clname);
}

int Active::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Active::Moved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Active::Destroyed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Active::Updated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Active::ReceivedText(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
static const uint qt_meta_data_Animal[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Animal[] = {
    "Animal\0"
};

void Animal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Animal::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Animal::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Animal,
      qt_meta_data_Animal, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Animal::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Animal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Animal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Animal))
        return static_cast<void*>(const_cast< Animal*>(this));
    return Active::qt_metacast(_clname);
}

int Animal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Dwarf[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Dwarf[] = {
    "Dwarf\0"
};

void Dwarf::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Dwarf::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Dwarf::staticMetaObject = {
    { &Animal::staticMetaObject, qt_meta_stringdata_Dwarf,
      qt_meta_data_Dwarf, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Dwarf::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Dwarf::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Dwarf::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Dwarf))
        return static_cast<void*>(const_cast< Dwarf*>(this));
    if (!strcmp(_clname, "Inventory"))
        return static_cast< Inventory*>(const_cast< Dwarf*>(this));
    return Animal::qt_metacast(_clname);
}

int Dwarf::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Animal::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pile[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Pile[] = {
    "Pile\0"
};

void Pile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pile::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Pile,
      qt_meta_data_Pile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pile))
        return static_cast<void*>(const_cast< Pile*>(this));
    if (!strcmp(_clname, "Inventory"))
        return static_cast< Inventory*>(const_cast< Pile*>(this));
    return Active::qt_metacast(_clname);
}

int Pile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Liquid[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Liquid[] = {
    "Liquid\0"
};

void Liquid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Liquid::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Liquid::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Liquid,
      qt_meta_data_Liquid, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Liquid::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Liquid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Liquid::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Liquid))
        return static_cast<void*>(const_cast< Liquid*>(this));
    return Active::qt_metacast(_clname);
}

int Liquid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Grass[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Grass[] = {
    "Grass\0"
};

void Grass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Grass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Grass::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Grass,
      qt_meta_data_Grass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Grass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Grass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Grass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Grass))
        return static_cast<void*>(const_cast< Grass*>(this));
    return Active::qt_metacast(_clname);
}

int Grass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Bush[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Bush[] = {
    "Bush\0"
};

void Bush::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Bush::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Bush::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Bush,
      qt_meta_data_Bush, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Bush::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Bush::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Bush::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Bush))
        return static_cast<void*>(const_cast< Bush*>(this));
    if (!strcmp(_clname, "Inventory"))
        return static_cast< Inventory*>(const_cast< Bush*>(this));
    return Active::qt_metacast(_clname);
}

int Bush::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Rabbit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Rabbit[] = {
    "Rabbit\0"
};

void Rabbit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Rabbit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Rabbit::staticMetaObject = {
    { &Animal::staticMetaObject, qt_meta_stringdata_Rabbit,
      qt_meta_data_Rabbit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Rabbit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Rabbit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Rabbit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Rabbit))
        return static_cast<void*>(const_cast< Rabbit*>(this));
    return Animal::qt_metacast(_clname);
}

int Rabbit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Animal::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Door[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Door[] = {
    "Door\0"
};

void Door::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Door::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Door::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Door,
      qt_meta_data_Door, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Door::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Door::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Door::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Door))
        return static_cast<void*>(const_cast< Door*>(this));
    return Active::qt_metacast(_clname);
}

int Door::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Creator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Creator[] = {
    "Creator\0"
};

void Creator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Creator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Creator::staticMetaObject = {
    { &Active::staticMetaObject, qt_meta_stringdata_Creator,
      qt_meta_data_Creator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Creator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Creator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Creator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Creator))
        return static_cast<void*>(const_cast< Creator*>(this));
    if (!strcmp(_clname, "Inventory"))
        return static_cast< Inventory*>(const_cast< Creator*>(this));
    return Active::qt_metacast(_clname);
}

int Creator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Active::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
