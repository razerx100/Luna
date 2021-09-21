#ifndef __EXCEPTION_MESSAGE_BOX_HPP__
#define __EXCEPTION_MESSAGE_BOX_HPP__

#ifdef BUILD_LUNA
#define LUNA_DLL __declspec(dllexport)
#else
#define LUNA_DLL __declspec(dllimport)
#endif

LUNA_DLL void _cdecl ExceptionMessageBox(
	const char* exceptionDetails,
	const char* exceptionType
);

#endif
