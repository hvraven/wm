#include "windowmanager.h"

#include <stdexcept>

WindowManager::WindowManager()
{
  int screen_num;
  connection = xcb_connect(nullptr, &screen_num);

  root_screen = xcb_aux_get_screen(conn, conn_screen);
  root = root_screen->root;

  // grab events
  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_POINTER_MOTION |
                        XCB_EVENT_MASK_PROPERTY_CHANGE |
                        XCB_EVENT_MASK_ENTER_WINDOW };
  xcb_void_cookie_t cookie;
  cookie = xcb_change_window_attributes_checked(conn, root, mask, values);
  xcb_generic_error_t* error = xcb_request_check(conn, cookie);
  if (error != nullptr)
    throw std::runtime_error("Another wm seems to be running.");
}

WindowManager::~WindowManager()
{
  xcb_disconnect(connection);
}

inline xcb_connection_t*
WindowManager::get_connection()
{
  return connection;
}

inline xcb_window_t
WindowManager::get_root_window() const
{
  return screen->root;
}
