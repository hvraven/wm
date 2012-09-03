#include "windowmanager.h"

#include "debug.h"
#include <stdexcept>
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
                        XCB_EVENT_MASK_ENTER_WINDOW };
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
      handle_event(event);
      free(event);
    }
}

void
WindowManager::handle_event(xcb_generic_event_t* event)
{
  switch (event->response_type & ~0x80)
    {
    default:
      dlog("Unhandled event of type ", event->response_type & ~0x80);
    }
}
