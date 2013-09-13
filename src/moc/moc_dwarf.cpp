/****************************************************************************
** Meta object code from reading C++ file 'Dwarf.h'
**
** Created: Wed 11. Sep 12:45:53 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Dwarf.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Dwarf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
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
QT_END_MOC_NAMESPACE
