/****************************************************************************
** Meta object code from reading C++ file 'Active.h'
**
** Created: Wed 11. Sep 12:45:53 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Active.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Active.h' doesn't include <QObject>."
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
QT_END_MOC_NAMESPACE
