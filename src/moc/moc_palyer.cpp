/****************************************************************************
** Meta object code from reading C++ file 'Player.h'
**
** Created: Wed 15. May 18:09:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Player.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Player.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Player[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      24,    7,    7,    7, 0x05,
      44,    7,    7,    7, 0x05,
      54,    7,    7,    7, 0x05,
      74,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,    7,    7,    7, 0x0a,
      97,    7,    7,    7, 0x0a,
     116,    7,    7,    7, 0x0a,
     131,    7,    7,    7, 0x0a,
     154,    7,    7,    7, 0x0a,
     196,  178,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Player[] = {
    "Player\0\0Notify(QString)\0OverstepBorder(int)\0"
    "Updated()\0GetString(QString&)\0Destroyed()\0"
    "CleanAll()\0CheckOverstep(int)\0"
    "BlockDestroy()\0WorldSizeReloadStart()\0"
    "WorldSizeReloadFinish()\0set_x,set_y,set_z\0"
    "SetPlayer(ushort,ushort,ushort)\0"
};

void Player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Player *_t = static_cast<Player *>(_o);
        switch (_id) {
        case 0: _t->Notify((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->OverstepBorder((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->Updated(); break;
        case 3: _t->GetString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->Destroyed(); break;
        case 5: _t->CleanAll(); break;
        case 6: _t->CheckOverstep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->BlockDestroy(); break;
        case 8: _t->WorldSizeReloadStart(); break;
        case 9: _t->WorldSizeReloadFinish(); break;
        case 10: _t->SetPlayer((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Player::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Player::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Player,
      qt_meta_data_Player, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Player::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Player::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Player))
        return static_cast<void*>(const_cast< Player*>(this));
    return QObject::qt_metacast(_clname);
}

int Player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Player::Notify(const QString & _t1)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< Player *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Player::OverstepBorder(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Player::Updated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Player::GetString(QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Player::Destroyed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
