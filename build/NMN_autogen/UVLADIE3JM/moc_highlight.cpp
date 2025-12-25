/****************************************************************************
** Meta object code from reading C++ file 'highlight.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/highlight.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'highlight.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14HighLightLabelE_t {};
} // unnamed namespace

template <> constexpr inline auto HighLightLabel::qt_create_metaobjectdata<qt_meta_tag_ZN14HighLightLabelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HighLightLabel",
        "LeftClick",
        "",
        "ind",
        "RightClick",
        "Framed",
        "ResultReady",
        "image",
        "Note",
        "tune",
        "QList<Note>",
        "notes",
        "onLeftClick",
        "onRightClick",
        "onFramed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'LeftClick'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'RightClick'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'Framed'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'ResultReady'
        QtMocHelpers::SignalData<void(const QPixmap &, const Note &, const QList<Note> &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QPixmap, 7 }, { 0x80000000 | 8, 9 }, { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onLeftClick'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onRightClick'
        QtMocHelpers::SlotData<void(int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onFramed'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HighLightLabel, qt_meta_tag_ZN14HighLightLabelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HighLightLabel::staticMetaObject = { {
    QMetaObject::SuperData::link<QLabel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14HighLightLabelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14HighLightLabelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14HighLightLabelE_t>.metaTypes,
    nullptr
} };

void HighLightLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HighLightLabel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->LeftClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->RightClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->Framed(); break;
        case 3: _t->ResultReady((*reinterpret_cast< std::add_pointer_t<QPixmap>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Note>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QList<Note>>>(_a[3]))); break;
        case 4: _t->onLeftClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onRightClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onFramed(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HighLightLabel::*)(int )>(_a, &HighLightLabel::LeftClick, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HighLightLabel::*)(int )>(_a, &HighLightLabel::RightClick, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HighLightLabel::*)()>(_a, &HighLightLabel::Framed, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HighLightLabel::*)(const QPixmap & , const Note & , const QList<Note> & )>(_a, &HighLightLabel::ResultReady, 3))
            return;
    }
}

const QMetaObject *HighLightLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HighLightLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14HighLightLabelE_t>.strings))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int HighLightLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void HighLightLabel::LeftClick(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void HighLightLabel::RightClick(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void HighLightLabel::Framed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HighLightLabel::ResultReady(const QPixmap & _t1, const Note & _t2, const QList<Note> & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}
namespace {
struct qt_meta_tag_ZN9HighLightE_t {};
} // unnamed namespace

template <> constexpr inline auto HighLight::qt_create_metaobjectdata<qt_meta_tag_ZN9HighLightE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HighLight",
        "onImageProcessed",
        "",
        "pixmap",
        "QList<Note>",
        "notes"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onImageProcessed'
        QtMocHelpers::SlotData<void(const QPixmap &, const QList<Note> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QPixmap, 3 }, { 0x80000000 | 4, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HighLight, qt_meta_tag_ZN9HighLightE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HighLight::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9HighLightE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9HighLightE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9HighLightE_t>.metaTypes,
    nullptr
} };

void HighLight::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HighLight *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onImageProcessed((*reinterpret_cast< std::add_pointer_t<QPixmap>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<Note>>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *HighLight::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HighLight::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9HighLightE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HighLight::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
