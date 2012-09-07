#ifndef KEYBIND_H
#define KEYBIND_H

#include "ptr.h"
#include <map>
#include <functional>
#include <utility>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

class WindowManager;


typedef std::function<void(WindowManager&)> KeyBindFunc;

typedef std::pair<uint16_t, xcb_keycode_t> KeyBind;

typedef std::tuple<uint16_t, xcb_keysym_t, KeyBindFunc> KeyBindConf;

typedef std::map<KeyBind, KeyBindFunc> Keybindings;

using keysyms_ptr = std::unique_ptr<xcb_key_symbols_t,
                                    std::function
                                        <void(xcb_key_symbols_t*)>>;

using keycode_ptr = free_ptr<xcb_keycode_t>;

#endif /* KEYBIND_H */
