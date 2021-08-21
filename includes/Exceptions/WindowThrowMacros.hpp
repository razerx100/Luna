#ifndef __WINDOW_THROW_MACROS_HPP__
#define __WINDOW_THROW_MACROS_HPP__

#include <WindowExceptions.hpp>

#define HWND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define HWND_NOGFX_EXCEPT() NoGfxException(__LINE__, __FILE__)
#define HWND_LAST_EXCEPT() WindowException(__LINE__, __FILE__, static_cast<HRESULT>(GetLastError()))

#endif
