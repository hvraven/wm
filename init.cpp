#include "windowmanager.h"
#include "keybind.h"

#include <vector>

void
WindowManager::initialize_keybindings()
{
  // temporary initialisation, should come from config
  std::vector<KeyBindConf> bindings;
  bindings.push_back
    (std::make_tuple(XCB_MOD_MASK_4, XK_Return,
                     std::bind(&WindowManager::spawn,
                               std::placeholders::_1,
                               "/usr/bin/xterm")));
  bindings.push_back
    (std::make_tuple(XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT, XK_Q,
                     &WindowManager::close_focus_window));

  keysyms_ptr keysyms = get_key_symbols();

  for (const auto& binding : bindings)
    {
      keycode_ptr kcode = get_key_code(keysyms, std::get<1>(binding));

      // The keycode can contain multiple values for a single keysym.
      // We have to register all of them.
      xcb_keycode_t* keycode = kcode.get();
      while (*keycode != XCB_NO_SYMBOL)
        {
          xcb_grab_key(conn, 0, get_root_window(), std::get<0>(binding), 
                       *keycode, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
          KeyBind key(std::get<0>(binding), *keycode);
          keybindings[key] = std::get<2>(binding);
          ++keycode;
        }
    }

  xcb_flush(conn);
}

void
WindowManager::initialize_mousebindings()
{
  // grab mouse buttons with modifier
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
