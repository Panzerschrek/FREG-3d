/****************************************************************************
** Meta object code from reading C++ file 'VirtScreen.h'
**
** Created: Wed 11. Sep 12:45:52 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../VirtScreen.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VirtScreen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VirtScreen[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,   11,   11,   11, 0x08,
      35,   11,   11,   11, 0x0a,
      51,   11,   11,   11, 0x0a,
      70,   11,   62,   11, 0x0a,
      97,   91,   11,   11, 0x0a,
     126,   11,   11,   11, 0x0a,
     138,   11,   11,   11, 0x0a,
     148,   11,   11,   11, 0x0a,
     175,  163,   11,   11, 0x0a,
     217,   11,   11,   11, 0x0a,
     232,   11,   11,   11, 0x0a,
     245,   11,   11,   11, 0x0a,
     263,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VirtScreen[] = {
    "VirtScreen\0\0ExitReceived()\0Print()\0"
    "Notify(QString)\0CleanAll()\0QString\0"
    "PassString(QString&)\0x,y,z\0"
    "Update(ushort,ushort,ushort)\0UpdateAll()\0"
    "Move(int)\0UpdatePlayer()\0x,y,z,range\0"
    "UpdateAround(ushort,ushort,ushort,ushort)\0"
    "ConnectWorld()\0UpdatesEnd()\0"
    "PlayerDestroyed()\0DeathScreen()\0"
};

void VirtScreen::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VirtScreen *_t = static_cast<VirtScreen *>(_o);
        switch (_id) {
        case 0: _t->ExitReceived(); break;
        case 1: _t->Print(); break;
        case 2: _t->Notify((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->CleanAll(); break;
        case 4: { QString _r = _t->PassString((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: _t->Update((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        case 6: _t->UpdateAll(); break;
        case 7: _t->Move((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->UpdatePlayer(); break;
        case 9: _t->UpdateAround((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3])),(*reinterpret_cast< ushort(*)>(_a[4]))); break;
        case 10: _t->ConnectWorld(); break;
        case 11: _t->UpdatesEnd(); break;
        case 12: _t->PlayerDestroyed(); break;
        case 13: _t->DeathScreen(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VirtScreen::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VirtScreen::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VirtScreen,
      qt_meta_data_VirtScreen, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VirtScreen::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VirtScreen::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VirtScreen::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VirtScreen))
        return static_cast<void*>(const_cast< VirtScreen*>(this));
    return QObject::qt_metacast(_clname);
}

int VirtScreen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void VirtScreen::ExitReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
