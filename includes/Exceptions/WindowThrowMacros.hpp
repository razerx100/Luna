#ifndef __WINDOW_THROW_MACROS_HPP__
#define __WINDOW_THROW_MACROS_HPP__

#include <WindowExceptions.hpp>

#define WIN32_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WIN32_NOGFX_EXCEPT() NoGfxException(__LINE__, __FILE__)
#define WIN32_LAST_EXCEPT() WindowException(__LINE__, __FILE__, static_cast<HRESULT>(GetLastError()))
#define WIN32_GENERIC_THROW(errorMsg) throw GenericException(__LINE__, __FILE__, errorMsg)

#endif
