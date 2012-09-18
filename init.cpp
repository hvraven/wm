#include "windowmanager.h"
#include "bindings.h"
#include "global.h"

typedef std::unique_ptr<xcb_key_symbols_t,
                        std::function<void(xcb_key_symbols_t*)>>
    keysyms_ptr;

void
WindowManager::initialize_keybindings()
{
  auto keysym_del = [&](xcb_key_symbols_t* ptr){xcb_key_symbols_free(ptr); };
  auto keysyms = keysyms_ptr(xcb_key_symbols_alloc(conn), keysym_del);

  for (const auto& config : config->keybindconfigs)
    {
      auto kcode = make_free_ptr<xcb_keycode_t>
        (xcb_key_symbols_get_keycode(keysyms.get(), std::get<1>(config)));

      // The keycode can contain multiple values for a single keysym.
      // We have to register all of them.
      xcb_keycode_t* keycode = kcode.get();
      while (*keycode != XCB_NO_SYMBOL)
        {
          xcb_grab_key(conn, 0, get_root_window(), std::get<0>(config),
                       *keycode, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
          KeyBind key(std::get<0>(config), *keycode);
          keybindings[key] = std::get<2>(config);
          ++keycode;
        }
    }

  xcb_flush(conn);
}

void
WindowManager::initialize_mousebindings()
{
  // grab mouse buttons with modifier
  for (const auto& config : config->mousebindconfigs)
    {
      xcb_grab_button(conn,
                      0,     // shall the event get to the lower windows
                      get_root_window(),    // grab in this area
                      XCB_EVENT_MASK_BUTTON_PRESS,  // events to grab
                      XCB_GRAB_MODE_ASYNC,  // pointer mode
                      XCB_GRAB_MODE_ASYNC,  // keyboard mode
                      get_root_window(),    // border for mouse
                      XCB_NONE,  // curser type (NONE = unchanged)
                      std::get<1>(config),  // button to grab
                      std::get<0>(config)); // key board modifier to grab

      MouseBind mouse(std::get<0>(config), std::get<1>(config));
      mousebindings[mouse] = std::get<2>(config);
    }
}
