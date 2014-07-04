/****************************************************************************
** Meta object code from reading C++ file 'editor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../editor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor_t {
    QByteArrayData data[21];
    char stringdata[326];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor_t qt_meta_stringdata_Editor = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 9),
QT_MOC_LITERAL(2, 17, 0),
QT_MOC_LITERAL(3, 18, 15),
QT_MOC_LITERAL(4, 34, 11),
QT_MOC_LITERAL(5, 46, 8),
QT_MOC_LITERAL(6, 55, 5),
QT_MOC_LITERAL(7, 61, 18),
QT_MOC_LITERAL(8, 80, 20),
QT_MOC_LITERAL(9, 101, 20),
QT_MOC_LITERAL(10, 122, 17),
QT_MOC_LITERAL(11, 140, 19),
QT_MOC_LITERAL(12, 160, 23),
QT_MOC_LITERAL(13, 184, 19),
QT_MOC_LITERAL(14, 204, 23),
QT_MOC_LITERAL(15, 228, 15),
QT_MOC_LITERAL(16, 244, 14),
QT_MOC_LITERAL(17, 259, 15),
QT_MOC_LITERAL(18, 275, 15),
QT_MOC_LITERAL(19, 291, 17),
QT_MOC_LITERAL(20, 309, 16)
    },
    "Editor\0loadModel\0\0fillModeChanged\0"
    "QtProperty*\0property\0value\0"
    "showTextureChanged\0renderingPathChanged\0"
    "samplerFilterChanged\0clearColorChanged\0"
    "ambientColorChanged\0ambientIntensityChanged\0"
    "diffuseColorChanged\0diffuseIntensityChanged\0"
    "fogColorChanged\0fogTypeChanged\0"
    "fogStartChanged\0fogRangeChanged\0"
    "fogDensityChanged\0enableFogChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    2,   95,    2, 0x08 /* Private */,
       7,    2,  100,    2, 0x08 /* Private */,
       8,    2,  105,    2, 0x08 /* Private */,
       9,    2,  110,    2, 0x08 /* Private */,
      10,    2,  115,    2, 0x08 /* Private */,
      11,    2,  120,    2, 0x08 /* Private */,
      12,    2,  125,    2, 0x08 /* Private */,
      13,    2,  130,    2, 0x08 /* Private */,
      14,    2,  135,    2, 0x08 /* Private */,
      15,    2,  140,    2, 0x08 /* Private */,
      16,    2,  145,    2, 0x08 /* Private */,
      17,    2,  150,    2, 0x08 /* Private */,
      18,    2,  155,    2, 0x08 /* Private */,
      19,    2,  160,    2, 0x08 /* Private */,
      20,    2,  165,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Bool,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Bool,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QColor,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QColor,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QColor,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QColor,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Double,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Double,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int,    5,    6,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Bool,    5,    6,

       0        // eod
};

void Editor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Editor *_t = static_cast<Editor *>(_o);
        switch (_id) {
        case 0: _t->loadModel(); break;
        case 1: _t->fillModeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->showTextureChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->renderingPathChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->samplerFilterChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->clearColorChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 6: _t->ambientColorChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 7: _t->ambientIntensityChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->diffuseColorChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 9: _t->diffuseIntensityChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->fogColorChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 11: _t->fogTypeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->fogStartChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 13: _t->fogRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 14: _t->fogDensityChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->showFogChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Editor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Editor.data,
      qt_meta_data_Editor,  qt_static_metacall, 0, 0}
};


const QMetaObject *Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Editor.stringdata))
        return static_cast<void*>(const_cast< Editor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
