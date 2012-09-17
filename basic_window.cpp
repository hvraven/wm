#include "basic_window.h"
#include "global.h"
#include "ptr.h"
#include "xwindow.h"
#include <xcb/xcb.h>

void
BasicWindow::remove_focus()
{
  if (focus != nullptr && focus != this)
    focus->remove_focus();

  focus = nullptr;
}

void
BasicWindow::reset_focus(BasicWindow* win)
{
  // if we are part of the old chain and the new focus isn't the
  // old one we are the crossing. Tell the old part about the loss
  if (focus != nullptr && focus != win)
    focus->remove_focus();

  // set focus and move up the chain till top
  focus = win;
  if (parent != nullptr)
    parent->reset_focus(this);
}

void
BasicWindow::get_geometry()
{
  free_ptr<xcb_get_geometry_reply_t> reply
    (xcb_get_geometry_reply(wm->conn, xcb_get_geometry(wm->conn, id), nullptr),
     &std::free);

  if (reply == nullptr)
    {
      elog("Failed to get geometry for window ", id);
      return;
    }

  x = reply->x;
  y = reply->y;
  width = reply->width;
  height = reply->height;
}

/**
 * sends the values given by the mask to the x server
 */
void
BasicWindow::update_configuration(uint16_t mask)
{
  uint32_t values[4];
  int i = -1;
  if (mask & XCB_CONFIG_WINDOW_X)
    values[++i] = x;
  if (mask & XCB_CONFIG_WINDOW_Y)
    values[++i] = y;
  if (mask & XCB_CONFIG_WINDOW_WIDTH)
    values[++i] = width;
  if (mask & XCB_CONFIG_WINDOW_HEIGHT)
    values[++i] = height;

  if (i >= 0)
    {
      xcb_configure_window(wm->conn, id, mask, values);
      xcb_flush(wm->conn);
    }
}

BasicWindow*
WindowStorage::new_xwindow(xcb_window_t id)
{
  return ((*this)[id] = make_unique<XWindow>(XWindow(id))).get();
}
