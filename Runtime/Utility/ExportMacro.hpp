#pragma once
#include <QtCore/QtGlobal>

#if defined(DLL_EXPORTS)
#  define DLL_API Q_DECL_EXPORT
#else
#  define DLL_API Q_DECL_IMPORT
#endif