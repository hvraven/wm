#include "windowmanager.h"
#include "keybind.h"

void
WindowManager::initialize_keybindings()
{
  keysyms_ptr keysyms = get_key_symbols();

  keycode_ptr kcode = get_key_code(keysyms, XK_Return);

  // The keycode can contain multiple values for a single keysym.
  // We have to register all of them.
  xcb_keycode_t* keycode = kcode.get();
  while (*keycode != XCB_NO_SYMBOL)
    {
      xcb_grab_key(conn, 0, get_root_window(), XCB_MOD_MASK_4, *keycode,
                   XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
      KeySet key(XCB_MOD_MASK_4, *keycode);
      keybindings[key] = &WindowManager::spawn;
      ++keycode;
    }

  xcb_flush(conn);
}

void
WindowManager::initialize_mousebindings()
{
  for (auto button : { XCB_BUTTON_INDEX_1,    // left
                       XCB_BUTTON_INDEX_2,    // right
                       XCB_BUTTON_INDEX_3 } ) // middle
    xcb_grab_button(conn,
                    0,     // shall the event get to the lower windows
                    get_root_window(),    // grab in this area
                    XCB_EVENT_MASK_BUTTON_PRESS,  // events to grab
                    XCB_GRAB_MODE_ASYNC,  // pointer mode
                    XCB_GRAB_MODE_ASYNC,  // keyboard mode
                    get_root_window(),    // border for mouse
                    XCB_NONE,  // curser type (NONE = unchanged)
                    button,    // button to grab
                    XCB_MOD_MASK_4); // key board modifier to grab
}
