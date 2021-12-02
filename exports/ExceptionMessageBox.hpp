#ifndef __EXCEPTION_MESSAGE_BOX_HPP__
#define __EXCEPTION_MESSAGE_BOX_HPP__

#ifdef BUILD_LUNA
#define LUNA_DLL __declspec(dllexport)
#else
#define LUNA_DLL __declspec(dllimport)
#endif

LUNA_DLL void __cdecl ExceptionMessageBox(
	const char* exceptionDetails,
	const char* exceptionType
);

#endif
