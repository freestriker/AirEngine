#pragma once
#include <QObject>
#include <qmetatype.h>
#include <qvariant.h>
#include <QMetaProperty>
#include <QMetaMethod>

#define CAT_IMPL(a, b) a##b
#define CAT(a, b) CAT_IMPL(a, b)

#define TO_STR_IMPL(a) #a
#define TO_STR(a) TO_STR_IMPL(a)

#define CAT_TO_STR(a, b) TO_STR(CAT(a, b))

#define DECLARE_TYPE(type) qRegisterMetaType< ## type ## >( ## TO_STR(type) ## );

#define REGISTRATION															\
static void auto_register_reflection_function_();								\
namespace                                                                       \
{                                                                               \
    struct auto__register__													\
    {                                                                           \
        auto__register__()														\
        {                                                                       \
            auto_register_reflection_function_();								\
        }                                                                       \
    };                                                                          \
}                                                                               \
static const auto__register__ CAT(auto_register__, __LINE__);					\
static void auto_register_reflection_function_()

#define REFLECTABLE_OBJECT      Q_OBJECT

#define PROPERTY    Q_PROPERTY 
#define INVOKABLE   Q_INVOKABLE

namespace AirEngine
{
	namespace Runtime
	{
        namespace Utility
        {
            using ReflectableObject = QObject;
            using MetaType = QMetaType;
            using MetaObject = QMetaObject;
        }
	}
}