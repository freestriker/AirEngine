#pragma once
#include <QtCore/QtGlobal>

#if defined(DLL_EXPORTS)
#  define DLL_API Q_DECL_EXPORT
#elif defined(DLL_IMPORTS)
#  define DLL_API Q_DECL_IMPORT
#else
#  define DLL_API
#endif