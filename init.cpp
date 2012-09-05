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
