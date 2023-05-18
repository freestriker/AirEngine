#pragma once
#include <QtCore/QtGlobal>

#if defined(DLL_EXPORTS)
#  define AIR_ENGINE_API Q_DECL_EXPORT
#elif defined(DLL_IMPORTS)
#  define AIR_ENGINE_API Q_DECL_IMPORT
#else
#  define AIR_ENGINE_API
#endif