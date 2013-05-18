/****************************************************************************
** Meta object code from reading C++ file 'screen.h'
**
** Created: Wed 15. May 18:09:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../screen.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FREGGLWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      32,   13,   13,   13, 0x0a,
      50,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FREGGLWidget[] = {
    "FREGGLWidget\0\0setXRotation(int)\0"
    "setYRotation(int)\0setZRotation(int)\0"
};

void FREGGLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FREGGLWidget *_t = static_cast<FREGGLWidget *>(_o);
        switch (_id) {
        case 0: _t->setXRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setYRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setZRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FREGGLWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FREGGLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_FREGGLWidget,
      qt_meta_data_FREGGLWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FREGGLWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FREGGLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FREGGLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FREGGLWidget))
        return static_cast<void*>(const_cast< FREGGLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int FREGGLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_Screen[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      25,   23,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,    7,    7,    7, 0x08,
      56,    7,    7,    7, 0x0a,
      72,    7,    7,    7, 0x0a,
      83,    7,    7,    7, 0x0a,
     107,  104,    7,    7, 0x0a,
     160,    7,    7,    7, 0x0a,
     176,  172,    7,    7, 0x0a,
     186,    7,    7,    7, 0x0a,
     210,  201,    7,    7, 0x0a,
     252,    7,    7,    7, 0x0a,
     267,    7,    7,    7, 0x0a,
     278,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Screen[] = {
    "Screen\0\0ExitReceived()\0,\0"
    "InputReceived(int,int)\0Print()\0"
    "Notify(QString)\0CleanAll()\0"
    "PassString(QString&)\0,,\0"
    "Update(unsigned short,unsigned short,unsigned short)\0"
    "UpdateAll()\0dir\0Move(int)\0UpdateAround()\0"
    ",,,range\0UpdateAround(ushort,ushort,ushort,ushort)\0"
    "UpdatePlayer()\0Flushinp()\0RePrint()\0"
};

void Screen::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Screen *_t = static_cast<Screen *>(_o);
        switch (_id) {
        case 0: _t->ExitReceived(); break;
        case 1: _t->InputReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->Print(); break;
        case 3: _t->Notify((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->CleanAll(); break;
        case 5: _t->PassString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->Update((*reinterpret_cast< const unsigned short(*)>(_a[1])),(*reinterpret_cast< const unsigned short(*)>(_a[2])),(*reinterpret_cast< const unsigned short(*)>(_a[3]))); break;
        case 7: _t->UpdateAll(); break;
        case 8: _t->Move((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 9: _t->UpdateAround(); break;
        case 10: _t->UpdateAround((*reinterpret_cast< const ushort(*)>(_a[1])),(*reinterpret_cast< const ushort(*)>(_a[2])),(*reinterpret_cast< const ushort(*)>(_a[3])),(*reinterpret_cast< const ushort(*)>(_a[4]))); break;
        case 11: _t->UpdatePlayer(); break;
        case 12: _t->Flushinp(); break;
        case 13: _t->RePrint(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Screen::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Screen::staticMetaObject = {
    { &VirtScreen::staticMetaObject, qt_meta_stringdata_Screen,
      qt_meta_data_Screen, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Screen::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Screen::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Screen::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Screen))
        return static_cast<void*>(const_cast< Screen*>(this));
    return VirtScreen::qt_metacast(_clname);
}

int Screen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VirtScreen::qt_metacall(_c, _id, _a);
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
void Screen::ExitReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Screen::InputReceived(int _t1, int _t2)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(const_cast< Screen *>(this), &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
