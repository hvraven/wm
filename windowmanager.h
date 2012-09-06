#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "keybind.h"
#include "ptr.h"
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
  Keybindings keybindings;

  xcb_window_t focus;

  void initialize_keybindings();
  void initialize_mousebindings();

  void spawn();
  void event_loop();
  void handle_button_press_event(xcb_button_press_event_t*);
  void handle_button_release_event(xcb_button_release_event_t*);
  void handle_configure_request_event(xcb_configure_request_event_t*);
  void handle_generic_event(xcb_generic_event_t*);
  void handle_key_press_event(xcb_key_press_event_t*);
  void handle_map_request_event(xcb_map_request_event_t*);
  void handle_motion_notify_event(xcb_motion_notify_event_t*);

  keysyms_ptr get_key_symbols();
  keycode_ptr get_key_code(xcb_key_symbols_t *, xcb_keysym_t);
  keycode_ptr get_key_code(const keysyms_ptr& syms, xcb_keysym_t sym)
    { return get_key_code(syms.get(), sym); }
};

#endif /* WINDOWMANAGER_H */
