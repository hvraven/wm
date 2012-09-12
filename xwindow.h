#ifndef WINDOW_H
#define WINDOW_H

#include "basic_window.h"

/**
 * The window class contains the grapical part of a container,
 * and every information required for the communication with
 * the X server.
 */
class XWindow : public BasicWindow
{
public:
  XWindow(xcb_window_t id, BasicWindow* parent = nullptr);

  xcb_window_t get_id() const { return id; }

  void move()        override {};
  void show()        override {};
  void hide()        override {};
  void add_window()  override {};
  void close()       override;
  void resize()      override {};
  void get_focus()   override;
};

#endif /* WINDOW_H */
