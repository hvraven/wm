#ifndef WINDOW_H
#define WINDOW_H

#include <map>
#include <memory>
#include <xcb/xcb.h>

class Window;

class WindowStorage
{
public:
  typedef std::map<xcb_window_t, std::shared_ptr<Window>> Windows;
  typedef xcb_window_t key_type;
  typedef Window* mapped_type;

  Window* operator[](key_type key) { return windows[key].get(); }
  void insert(const Window&);

private:
  Windows windows;
};

/**
 * The window class contains the grapical part of a container,
 * and every information required for the communication with
 * the X server.
 */
class Window
{
public:
  Window(xcb_connection_t* conn_, xcb_window_t id_)
    : conn(conn_), id(id_) {}

  xcb_window_t get_id() const { return id; }

  void set_focus();
  void remove_focus();

private:
  xcb_connection_t* conn;
  xcb_window_t id;
};

#endif /* WINDOW_H */
