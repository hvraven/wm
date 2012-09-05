#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "window.h"
#include <list>
#include <xcb/xcb.h>

class WindowManager
{
public:
  WindowManager();
  virtual ~WindowManager();

  void run();

  xcb_connection_t* get_connection()  { return conn; }
  xcb_screen_t*     get_root_screen() { return root_screen; }
  xcb_window_t      get_root_window() { return root_screen->root; }

private:
  xcb_connection_t* conn;
  xcb_screen_t* root_screen;

  // very simple window management
  std::list<Window> windows;

  void test_setup_keybinding();

  void event_loop();
  void handle_generic_event(xcb_generic_event_t*);
  void handle_button_press_event(xcb_button_press_event_t*);
  void handle_configure_request_event(xcb_configure_request_event_t*);
  void handle_map_request_event(xcb_map_request_event_t*);
};

#endif /* WINDOWMANAGER_H */
