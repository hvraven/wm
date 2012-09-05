#include "windowmanager.h"
#include "keybind.h"

void
WindowManager::initialize_keybindings()
{
  keysyms_ptr keysyms = get_key_symbols();

  keycode_ptr keycode = get_key_code(keysyms, XK_Return);

  /*
   * enum MODS {
   *   NONE = 0
   *   XCB_MOD_MASK_SHIFT (1 << 1)
   *   XCB_MOD_MASK_CONTROL (1 << 2)
   *   XCB_MOD_MASK_1 (1 << 3)
   *   ...
   *   XCB_MOD_MASK_5 (1 << 7)
   *   XCB_MOD_MASK_ANY (???)
   * }
   */
  xcb_grab_key(conn, 0, get_root_window(), XCB_MOD_MASK_4, *keycode,
               XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
  KeySet key(XCB_MOD_MASK_4, *keycode);
  keybindings[key] = &WindowManager::spawn;

  xcb_flush(conn);
}
