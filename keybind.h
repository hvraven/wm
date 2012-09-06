#ifndef KEYBIND_H
#define KEYBIND_H

#include <map>
#include <functional>
#include <utility>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

class WindowManager;

typedef std::pair<uint16_t, xcb_keycode_t> KeySet;

typedef std::function<void(WindowManager&)> KeyBindFunc;

typedef std::map<KeySet, KeyBindFunc> Keybindings;

#endif /* KEYBIND_H */
