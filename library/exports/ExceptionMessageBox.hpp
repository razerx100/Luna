#ifndef EXCEPTION_MESSAGE_BOX_HPP_
#define EXCEPTION_MESSAGE_BOX_HPP_

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
