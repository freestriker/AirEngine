#pragma once

#if defined(_WIN32) || defined(__WIN32__)

	#define DLL_CALLCONV __stdcall

	#ifdef DLL_EXPORTS
		#define DLL_API __declspec(dllexport)
	#else
		#define DLL_API __declspec(dllimport)
	#endif
#else 
	#if defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
		#ifndef GCC_HASCLASSVISIBILITY
			#define GCC_HASCLASSVISIBILITY
		#endif
	#endif

	#define DLL_CALLCONV

	#if defined(GCC_HASCLASSVISIBILITY)
		#define DLL_API __attribute__ ((visibility("default")))
	#else
		#define DLL_API
	#endif		
#endif
