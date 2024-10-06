#ifndef WINDOW_THROW_MACROS_HPP_
#define WINDOW_THROW_MACROS_HPP_

#include <WindowExceptions.hpp>

#define WIN32_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WIN32_LAST_EXCEPT() WindowException(__LINE__, __FILE__, static_cast<HRESULT>(GetLastError()))
#define WIN32_GENERIC_THROW(errorMsg) throw GenericException(__LINE__, __FILE__, errorMsg)

#endif
