/****************************************************************************
** Meta object code from reading C++ file 'glwidget2.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Volume_Rendering_Ver1/glwidget2.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GLWidget2_t {
    QByteArrayData data[9];
    char stringdata0[61];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GLWidget2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GLWidget2_t qt_meta_stringdata_GLWidget2 = {
    {
QT_MOC_LITERAL(0, 0, 9), // "GLWidget2"
QT_MOC_LITERAL(1, 10, 12), // "color_change"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 1), // "r"
QT_MOC_LITERAL(4, 26, 1), // "g"
QT_MOC_LITERAL(5, 28, 1), // "b"
QT_MOC_LITERAL(6, 30, 8), // "sigapply"
QT_MOC_LITERAL(7, 39, 19), // "set_histogram_scale"
QT_MOC_LITERAL(8, 59, 1) // "n"

    },
    "GLWidget2\0color_change\0\0r\0g\0b\0sigapply\0"
    "set_histogram_scale\0n"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GLWidget2[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   29,    2, 0x06 /* Public */,
       6,    0,   36,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,    3,    4,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

void GLWidget2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GLWidget2 *_t = static_cast<GLWidget2 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->color_change((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 1: _t->sigapply(); break;
        case 2: _t->set_histogram_scale((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GLWidget2::*_t)(float , float , float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GLWidget2::color_change)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GLWidget2::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GLWidget2::sigapply)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject GLWidget2::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GLWidget2.data,
      qt_meta_data_GLWidget2,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *GLWidget2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GLWidget2::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_GLWidget2.stringdata0))
        return static_cast<void*>(const_cast< GLWidget2*>(this));
    return QWidget::qt_metacast(_clname);
}

int GLWidget2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void GLWidget2::color_change(float _t1, float _t2, float _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GLWidget2::sigapply()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
