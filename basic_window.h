#ifndef BASIC_WINDOW_H
#define BASIC_WINDOW_H

#include <map>
#include <memory>
#include <xcb/xcb.h>

class BasicWindow
{
public:
  BasicWindow(xcb_window_t id_, BasicWindow* parent_ = nullptr)
    : focus(nullptr), parent(parent_), id(id_), x(0), y(0), width(0),
      height(0) {}
  virtual ~BasicWindow() {};

  // top -> down
  // adds a subwindow, gets passed from top down
  virtual void add_window() = 0;
  virtual void resize(xcb_query_pointer_reply_t*) = 0;
  virtual void move(xcb_query_pointer_reply_t*) = 0;
  virtual void show() = 0;
  virtual void hide() = 0;

  virtual void close() = 0;

  // tell the winodw it no longer has the focus
  virtual void remove_focus();
  // sets the focus entry to the new value
  void reset_focus() { reset_focus(this); }
  void reset_focus(BasicWindow*);
  // tells the window it get's the focus
  virtual void get_focus() = 0;

  void set_parent(BasicWindow* p) { parent = p; }
  BasicWindow* get_parent() { return parent; }

  BasicWindow* focus;
  BasicWindow* parent;
  xcb_window_t id;

  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;

protected:
  void get_geometry();
  void update_configuration(uint16_t mask);
};

typedef std::map<xcb_window_t, std::unique_ptr<BasicWindow>> WindowMap;
class WindowStorage : public WindowMap
{
public:
  BasicWindow* new_xwindow(xcb_window_t);
};

#endif /* BASIC_WINDOW_H */
