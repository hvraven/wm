#ifndef PTR_H
#define PTR_H

#include <cstdlib>
#include <memory>

template <class T>
using free_ptr = std::unique_ptr<T, decltype(&std::free)>;

using keysyms_ptr = std::unique_ptr<xcb_key_symbols_t,
                                    std::function
                                        <void(xcb_key_symbols_t*)>>;

using keycode_ptr = free_ptr<xcb_keycode_t>;

#endif /* PTR_H */
