#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <xcb/xcb.h>

class WindowManager
{
public:
  WindowManager();
  virtual ~WindowManager();

  xcb_connection_t* get_connection() const;
  xcb_window_t get_root_window() const;

private:
  xcb_connection_t* conn;
  xcb_screen_t* root_screen;
  xcb_window_t root;
};

#endif /* WINDOWMANAGER_H */
