#include "conf.h"
#include "global.h"
#include "windowmanager.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

Configuration* config;

uint32_t
get_color(const char* color_name)
{
  xcb_alloc_named_color_cookie_t cookie = 
    xcb_alloc_named_color(wm->conn, wm->root_screen->default_colormap,
                          strlen(color_name), color_name);
  xcb_generic_error_t* error;
  free_ptr<xcb_alloc_named_color_reply_t> reply 
    (xcb_alloc_named_color_reply(wm->conn, cookie, &error), &std::free);

  if (error != nullptr)
    throw std::runtime_error("Couldn't get color value");

  return reply->pixel;
}

KeyBindConfigs
get_keybindconfigs()
{
  KeyBindConfigs configs;
  configs.push_back
    (std::make_tuple(XCB_MOD_MASK_4, XK_Return,
                     std::bind(&WindowManager::spawn,
                               std::placeholders::_1,
                               "/usr/bin/xterm")));
  configs.push_back
    (std::make_tuple(XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT, XK_Q,
                     &WindowManager::close_focus_window));
  configs.push_back
    (std::make_tuple(XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT |
                     XCB_MOD_MASK_CONTROL, XK_Q,
                     &WindowManager::close));

  return configs;
}

MouseBindConfigs
get_mousebindconfigs()
{
  MouseBindConfigs configs;
  configs.push_back
    (std::make_tuple(XCB_MOD_MASK_4, XCB_BUTTON_INDEX_1,
                     &WindowManager::enable_move));
  configs.push_back
    (std::make_tuple(XCB_MOD_MASK_4, XCB_BUTTON_INDEX_2,
                     &WindowManager::enable_resize));

  return configs;
}

Configuration::Configuration()
   : border_width(1),
     border_color_unfocused(),
     border_color_focused(),
     keybindconfigs(get_keybindconfigs()),
     mousebindconfigs(get_mousebindconfigs())
{
  config = this;
}

// some configurations require a connection to the x server. Thus we
// initialize them after the WindowManager established the connection
// to the server.
void
Configuration::complete_initialization()
{
  border_color_unfocused = get_color("grey50");
  border_color_focused   = get_color("goldenrod1");
}
