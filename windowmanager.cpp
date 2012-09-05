#include "windowmanager.h"

#include "logging.h"
#include <stdexcept>
#include <unistd.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_keysyms.h>

// from X11/keysymdef.h
const uint32_t XK_Num_Lock = 0xff7f;

WindowManager::WindowManager()
{
  int screen_num;
  conn = xcb_connect(nullptr, &screen_num);

  root_screen = xcb_aux_get_screen(conn, screen_num);

  // grab events
  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_POINTER_MOTION |
                        XCB_EVENT_MASK_PROPERTY_CHANGE |
                        XCB_EVENT_MASK_ENTER_WINDOW |
                        XCB_EVENT_MASK_BUTTON_PRESS };
  xcb_void_cookie_t cookie =
    xcb_change_window_attributes_checked(conn, get_root_window(),
                                         mask, values);
  xcb_generic_error_t* error = xcb_request_check(conn, cookie);
  if (error != nullptr)
    throw std::runtime_error("Another wm seems to be running.");

  dlog("done with initialization.");
}

WindowManager::~WindowManager()
{
  xcb_disconnect(conn);
}

void
WindowManager::run()
{
  event_loop();
}

void
WindowManager::test_setup_keybinding()
{
  dlog("Setup Test Keygrab");

  uint32_t keycode = 1;
  /*
   * enum MODS {
   *   NONE = 0
   *   XCB_MOD_MASK_SHIFT (1 << 1)
   *   XCB_MOD_MASK_CONTROL (1 << 2)
   *   XCB_MOD_MASK_1 (1 << 3)
   *   ...
   *   XCB_MOD_MASK_5 (1 << 7)
   *   XCB_MOD_MASK_ANY (???)
   * }
   */
  uint32_t mods = 0;

  xcb_grab_key(conn, 0, get_root_window(), mods, keycode,
               XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
}

void
WindowManager::event_loop()
{
  for (;;)
    {
      xcb_generic_event_t* event = xcb_wait_for_event(conn);
      handle_generic_event(event);
      delete event;
    }
}

void
WindowManager::handle_generic_event(xcb_generic_event_t* event)
{
  switch (event->response_type & ~0x80)
    {
    case XCB_BUTTON_PRESS:
      dlog("Event of type \'XCB_BUTTON_PRESS\' found.");
      handle_button_press_event
        (reinterpret_cast<xcb_button_press_event_t*>(event));
      break;
    case XCB_CONFIGURE_REQUEST:
      dlog("Event of type \'XCB_CONFIGURE_REQUEST\' found.");
      handle_configure_request_event
        (reinterpret_cast<xcb_configure_request_event_t*>(event));
      break;
    case XCB_MAP_REQUEST:
      dlog("Event of type \'XCB_MAP_REQUEST\' found.");
      handle_map_request_event
        (reinterpret_cast<xcb_map_request_event_t*>(event));
      break;
    case XCB_KEY_PRESS:
      dlog("Event of type \'XCB_KEY_PRESS\' found.");
      //handle_key_press_event(reinterpret_cast<xcb_key_press_event_t*>(event));
      break;
    default:
      elog("Unhandled event of type ", event->response_type & ~0x80);
      break;
    }
}

void
WindowManager::handle_button_press_event(xcb_button_press_event_t*)
{
  if (fork() == 0)
    {
      setsid();
      dlog("Spawning xterm");
      execlp("/usr/bin/xterm", "");
      elog("Failed to spawn xterm");
    }
}

void
WindowManager::handle_configure_request_event
    (xcb_configure_request_event_t* event)
{
  uint32_t values[7];
  int i = -1;

  if (event->value_mask & XCB_CONFIG_WINDOW_X)
    values[++i] = event->x;
  if (event->value_mask & XCB_CONFIG_WINDOW_Y)
    values[++i] = event->y;
  if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
    values[++i] = event->width;
  if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
    values[++i] = event->height;
  if (event->value_mask & XCB_CONFIG_WINDOW_SIBLING)
    values[++i] = event->sibling;
  if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE)
    values[++i] = event->stack_mode;

  if (i >= 0)
    {
      xcb_configure_window(conn, event->window, event->value_mask, values);
      xcb_flush(conn);
    }
}

void
WindowManager::handle_map_request_event(xcb_map_request_event_t* event)
{
  Window win(event->window);
  windows.push_back(win);
  xcb_map_window(conn, win.get_id());
  xcb_flush(conn);
}
