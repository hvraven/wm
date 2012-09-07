#include "window.h"

void
WindowStorage::insert(const Window& win)
{
  windows[win.get_id()] = std::unique_ptr<Window>(new Window(win));
}

void
Window::set_focus()
{
  // tell the X server we now have the focus
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT,
                      get_id(), XCB_CURRENT_TIME);
}

/**
 * this function allows the window to remove decorations
 */
void
Window::remove_focus()
{

}
