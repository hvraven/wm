#ifndef KEYBIND_H
#define KEYBIND_H

#include <cstdlib>
#include <map>
#include <memory>
#include <functional>
#include <utility>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

class WindowManager;

typedef std::pair<uint16_t, xcb_keycode_t> KeySet;

typedef std::function<void(WindowManager&)> KeyBindFunc;

typedef std::map<KeySet, KeyBindFunc> Keybindings;

typedef std::unique_ptr<xcb_key_symbols_t,
                        std::function<void(xcb_key_symbols_t*)>>
    keysyms_ptr;
typedef std::unique_ptr<xcb_keycode_t, decltype(&std::free)> keycode_ptr;

#endif /* KEYBIND_H */
