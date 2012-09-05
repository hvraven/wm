#include "windowmanager.h"

keysyms_ptr
WindowManager::get_key_symbols()
{
  auto keysym_del = [&](xcb_key_symbols_t* ptr){xcb_key_symbols_free(ptr); };
  return keysyms_ptr(xcb_key_symbols_alloc(conn), keysym_del);
}

keycode_ptr
WindowManager::get_key_code(xcb_key_symbols_t* keysyms,
                            xcb_keysym_t       symbol)
{
  return keycode_ptr(xcb_key_symbols_get_keycode(keysyms, symbol),
                     &std::free);
}
