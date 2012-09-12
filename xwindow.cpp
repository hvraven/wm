#include "xwindow.h"
#include "global.h"
#include <xcb/xcb_icccm.h>

XWindow::XWindow(xcb_window_t id, BasicWindow* parent)
   : BasicWindow(id, parent)
{
  // setup event mask
  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_PROPERTY_CHANGE |
                        XCB_EVENT_MASK_ENTER_WINDOW };
  xcb_change_window_attributes(wm->conn, id, mask, values);
}

void
XWindow::get_focus()
{
  // tell the X server we now have the focus
  xcb_set_input_focus(wm->conn, XCB_INPUT_FOCUS_POINTER_ROOT,
                      get_id(), XCB_CURRENT_TIME);

  reset_focus();
}

void
XWindow::close()
{
  // first try the nice way using WM_DELETE if the window supports it
  bool be_friendly = false;
  xcb_get_property_cookie_t cookie =
    xcb_icccm_get_wm_protocols_unchecked(wm->conn, id, 
                                         wm->atoms.wm_protocols);
  xcb_icccm_get_wm_protocols_reply_t protocols;

  if (xcb_icccm_get_wm_protocols_reply(wm->conn, cookie,
                                       &protocols, NULL) == 1)
    for (uint32_t i = 0; i < protocols.atoms_len; ++i)
      if (protocols.atoms[i] == wm->atoms.wm_delete_window)
        {
          be_friendly = true;
          break;
        }

  xcb_icccm_get_wm_protocols_reply_wipe(&protocols);

  if (be_friendly)
    {
      xcb_client_message_event_t event;
      event.response_type = XCB_CLIENT_MESSAGE;
      event.format = 32;
      event.sequence = 0;
      event.window = id;
      event.type = wm->atoms.wm_protocols;
      event.data.data32[0] = wm->atoms.wm_delete_window;
      event.data.data32[1] = XCB_CURRENT_TIME;
      xcb_send_event(wm->conn, false, id, XCB_EVENT_MASK_NO_EVENT,
                     reinterpret_cast<char*>(&event));
    }
  // the window doesn't understand ICCCM. Use the hard way
  else
    xcb_kill_client(wm->conn, id);

  xcb_flush(wm->conn);
}
