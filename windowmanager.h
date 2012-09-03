#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#define MULTIPLICITY
#include <ev++.h>
#include <xcb/xcb.h>

class WindowManager
{
public:
  WindowManager();
  virtual ~WindowManager();

  void run();

  xcb_connection_t* get_connection() { return conn; }
  xcb_screen_t*     get_root_screen() { return root_screen; }
  xcb_window_t      get_root_window() { return root_screen->root; }

private:
  xcb_connection_t* conn;
  xcb_screen_t* root_screen;

  void test_setup_keybinding();

  void event_loop();
  void handle_event(xcb_generic_event_t* event);
};

#endif /* WINDOWMANAGER_H */
