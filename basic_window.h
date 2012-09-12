#ifndef BASIC_WINDOW_H
#define BASIC_WINDOW_H

#include <map>
#include <memory>
#include <xcb/xcb.h>

class BasicWindow
{
public:
  BasicWindow(xcb_window_t id_, BasicWindow* parent_ = nullptr)
    : focus(nullptr), parent(parent_), id(id_) {}
  virtual ~BasicWindow() {};

  // top -> down
  // adds a subwindow, gets passed from top down
  virtual void add_window() = 0;
  virtual void resize() = 0;
  virtual void move() = 0;
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

  xcb_window_t get_id() const { return id; }

protected:
  BasicWindow* focus;
  BasicWindow* parent;

  xcb_window_t id;
};

class XWindow;

typedef std::map<xcb_window_t, std::unique_ptr<BasicWindow>> WindowMap;
class WindowStorage : public WindowMap
{
public:
  XWindow* new_xwindow(xcb_window_t);
};

#endif /* BASIC_WINDOW_H */
