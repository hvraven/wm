#ifndef XWINDOW_H
#define XWINDOW_H

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

  void move(xcb_query_pointer_reply_t*) override;
  void resize(xcb_query_pointer_reply_t*) override;
  void show()        override {};
  void hide()        override {};
  void add_window()  override {};
  void close()       override;
  void get_focus()   override;
  void remove_focus() override;
};

#endif /* XWINDOW_H */
