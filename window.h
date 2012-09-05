#ifndef WINDOW_H
#define WINDOW_H

#include <xcb/xcb.h>

/**
 * The window class contains the grapical part of a container,
 * and every information required for the communication with
 * the X server.
 */
class Window
{
public:
  Window(xcb_window_t);

  xcb_window_t get_id() const { return id; }

private:
  xcb_window_t id;
};

#endif /* WINDOW_H */
