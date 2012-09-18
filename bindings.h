#ifndef BINDINGS_H
#define BINDINGS_H

#include "ptr.h"
#include <map>
#include <functional>
#include <utility>
#include <vector>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

class WindowManager;
typedef std::function<void(WindowManager&)> BindFunc;

typedef std::pair<uint16_t, uint8_t>             KeyBind;
typedef std::tuple<uint16_t, uint32_t, BindFunc> KeyBindConf;
typedef std::map<KeyBind, BindFunc>              KeyBindings;
typedef std::vector<KeyBindConf>                 KeyBindConfigs;

typedef std::pair<uint16_t, uint8_t>            MouseBind;
typedef std::tuple<uint16_t, uint8_t, BindFunc> MouseBindConf;
typedef std::map<MouseBind, BindFunc>           MouseBindings;
typedef std::vector<MouseBindConf>              MouseBindConfigs;

#endif /* BINDINGS_H */
