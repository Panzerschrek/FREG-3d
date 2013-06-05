/****************************************************************************
** Meta object code from reading C++ file 'world.h'
**
** Created: Thu 30. May 19:10:49 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../world.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'world.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_World[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      23,    6,    6,    6, 0x05,
      46,   43,    6,    6, 0x05,
      76,    6,    6,    6, 0x05,
     101,   89,    6,    6, 0x05,
     144,    6,    6,    6, 0x05,
     155,    6,    6,    6, 0x05,
     167,    6,    6,    6, 0x05,
     182,   43,    6,    6, 0x05,
     215,    6,    6,    6, 0x05,
     232,    6,    6,    6, 0x05,
     250,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     265,    6,    6,    6, 0x0a,
     276,    6,    6,    6, 0x0a,
     294,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_World[] = {
    "World\0\0Notify(QString)\0GetString(QString&)\0"
    ",,\0Updated(ushort,ushort,ushort)\0"
    "UpdatedAll()\0x,y,z,level\0"
    "UpdatedAround(ushort,ushort,ushort,ushort)\0"
    "Moved(int)\0ReConnect()\0UpdatesEnded()\0"
    "NeedPlayer(ushort,ushort,ushort)\0"
    "StartReloadAll()\0FinishReloadAll()\0"
    "ExitReceived()\0CleanAll()\0ReloadShreds(int)\0"
    "PhysEvents()\0"
};

void World::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        World *_t = static_cast<World *>(_o);
        switch (_id) {
        case 0: _t->Notify((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->GetString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->Updated((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        case 3: _t->UpdatedAll(); break;
        case 4: _t->UpdatedAround((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3])),(*reinterpret_cast< ushort(*)>(_a[4]))); break;
        case 5: _t->Moved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->ReConnect(); break;
        case 7: _t->UpdatesEnded(); break;
        case 8: _t->NeedPlayer((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        case 9: _t->StartReloadAll(); break;
        case 10: _t->FinishReloadAll(); break;
        case 11: _t->ExitReceived(); break;
        case 12: _t->CleanAll(); break;
        case 13: _t->ReloadShreds((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->PhysEvents(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData World::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject World::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_World,
      qt_meta_data_World, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &World::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *World::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *World::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_World))
        return static_cast<void*>(const_cast< World*>(this));
    return QThread::qt_metacast(_clname);
}

int World::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void World::Notify(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< World *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void World::GetString(QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< World *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void World::Updated(ushort _t1, ushort _t2, ushort _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void World::UpdatedAll()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void World::UpdatedAround(ushort _t1, ushort _t2, ushort _t3, ushort _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void World::Moved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void World::ReConnect()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void World::UpdatesEnded()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void World::NeedPlayer(ushort _t1, ushort _t2, ushort _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void World::StartReloadAll()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void World::FinishReloadAll()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void World::ExitReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}
QT_END_MOC_NAMESPACE
