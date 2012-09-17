#include "conf.h"
#include "global.h"
#include "ptr.h"
#include <cstring>
#include <stdexcept>
#include <xcb/xcb.h>

Configuration::Configuration()
   : border_width(1),
     border_color_unfocused(get_color("grey40")),
     border_color_focused(get_color("goldenrod1"))
{
}

uint32_t
Configuration::get_color(const char* color_name)
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
