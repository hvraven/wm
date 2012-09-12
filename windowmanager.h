#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "atom.h"
#include "basic_window.h"
#include "keybind.h"
#include "ptr.h"
#include <xcb/xcb.h>

class WindowManager : BasicWindow
{
public:
  WindowManager();
  virtual ~WindowManager();

  void run();

  xcb_connection_t* get_connection()  { return conn; }
  xcb_screen_t*     get_root_screen() { return root_screen; }
  xcb_window_t      get_root_window() { return root_screen->root; }

  void add_window()    override {};
  void resize()        override {};
  void move()          override {};
  void get_focus()     override {};
  void hide()          override {};
  void close()         override {};
  void show()          override {};

  // public members which are used regulary by other classes
  xcb_connection_t* conn;
  xcb_screen_t* root_screen;
  WindowStorage windows;
  Atoms atoms;

private:
  Keybindings keybindings;


  void initialize_keybindings();
  void initialize_mousebindings();

  BasicWindow* new_xwindow();
  void spawn(const char*);
  void close_focus_window();
  void close_window(xcb_window_t);
  void set_focus(xcb_window_t id) { set_focus(windows[id].get()); }
  void set_focus(BasicWindow* win);

  void event_loop();
  void handle_button_press_event(xcb_button_press_event_t*);
  void handle_button_release_event(xcb_button_release_event_t*);
  void handle_configure_request_event(xcb_configure_request_event_t*);
  void handle_enter_notify_event(xcb_enter_notify_event_t*);
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
