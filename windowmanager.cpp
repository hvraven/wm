#include "windowmanager.h"

#include "logging.h"
#include "ptr.h"
#include "xwindow.h"
#include <stdexcept>
#include <unistd.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

WindowManager::WindowManager()
  : BasicWindow(0),
    conn(nullptr),
    root_screen(nullptr),
    windows(),
    atoms(),
    keybindings(),
    window_state(None)
{
  int screen_num;
  conn = xcb_connect(nullptr, &screen_num);

  root_screen = xcb_aux_get_screen(conn, screen_num);
  id = get_root_window();

  atoms = Atoms(conn);

  // grab events
  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_PROPERTY_CHANGE |
                        XCB_EVENT_MASK_ENTER_WINDOW };
  xcb_void_cookie_t cookie =
    xcb_change_window_attributes_checked(conn, get_root_window(),
                                         mask, values);
  xcb_generic_error_t* error = xcb_request_check(conn, cookie);
  if (error != nullptr)
    throw std::runtime_error("Another wm seems to be running.");

  initialize_keybindings();
  initialize_mousebindings();

  dlog("done with initialization.");
}

WindowManager::~WindowManager()
{
  xcb_disconnect(conn);
}

void
WindowManager::run()
{
  event_loop();
}

void
WindowManager::spawn(const char* program)
{
  if (fork() == 0)
    {
      setsid();
      dlog("Spawning ", program);
      execlp(program, "");
      elog("Failed to spawn ", program);
    }
}

/**
 * closes the focused window
 */
void
WindowManager::close_focus_window()
{
  if (focus == nullptr)
    {
      dlog("Tried to close focused window, but no focus set.");
      return;
    }
  close_window(focus->get_id());

  // we just killed our focused window
  focus = nullptr;
}

/**
 * @brief closes a window
 */
void
WindowManager::close_window(xcb_window_t id)
{
  auto& window = windows[id];

  // the window may have to clean up some things
  window->close();

  // remove the window from our knowledge
  windows.erase(id);
}

void
WindowManager::close()
{
  exit(0);
}

