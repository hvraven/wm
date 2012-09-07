#include "windowmanager.h"

#include "logging.h"
#include <stdexcept>
#include <unistd.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

WindowManager::WindowManager()
  : conn(nullptr),
    root_screen(nullptr),
    windows(),
    keybindings(),
    atoms(),
    focus(nullptr)
{
  int screen_num;
  conn = xcb_connect(nullptr, &screen_num);

  root_screen = xcb_aux_get_screen(conn, screen_num);

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
      elog("Failed to spawn xterm");
    }
}

typedef std::unique_ptr<xcb_generic_event_t, decltype(&std::free)>
    generic_event_ptr;

void
WindowManager::event_loop()
{
  for (;;)
    {
      generic_event_ptr event(xcb_wait_for_event(conn), &std::free);
      handle_generic_event(event.get());
    }
}

void
WindowManager::handle_generic_event(xcb_generic_event_t* event)
{
  switch (event->response_type & ~0x80)
    {
    case XCB_BUTTON_PRESS:
      dlog("Event of type \'XCB_BUTTON_PRESS\' found.");
      handle_button_press_event
        (reinterpret_cast<xcb_button_press_event_t*>(event));
      break;
    case XCB_BUTTON_RELEASE:
      dlog("Event of type \'XCB_BUTTON_RELEASE\' found.");
      handle_button_release_event
        (reinterpret_cast<xcb_button_release_event_t*>(event));
      break;
    case XCB_CONFIGURE_REQUEST:
      dlog("Event of type \'XCB_CONFIGURE_REQUEST\' found.");
      handle_configure_request_event
        (reinterpret_cast<xcb_configure_request_event_t*>(event));
      break;
    case XCB_ENTER_NOTIFY:
      dlog("Event of type \'XCB_ENTER_NOTIFY\' found.");
      handle_enter_notify_event
        (reinterpret_cast<xcb_enter_notify_event_t*>(event));
      break;
    case XCB_KEY_PRESS:
      dlog("Event of type \'XCB_KEY_PRESS\' found.");
      handle_key_press_event(reinterpret_cast<xcb_key_press_event_t*>(event));
      break;
    case XCB_KEY_RELEASE:
      dlog("Event of type \'XCB_KEY_RELEASE\' found.");
      break;
    case XCB_MAP_REQUEST:
      dlog("Event of type \'XCB_MAP_REQUEST\' found.");
      handle_map_request_event
        (reinterpret_cast<xcb_map_request_event_t*>(event));
      break;
    case XCB_MAPPING_NOTIFY:
      dlog("Event of type \'XCB_MAPPING_NOTIFY\' found.");
      break;
    case XCB_MOTION_NOTIFY:
      dlog("Event of type \'XCB_MOTION_NOTIFY\' found.");
      handle_motion_notify_event
        (reinterpret_cast<xcb_motion_notify_event_t*>(event));
      break;
    default:
      elog("Unhandled event of type ", event->response_type & ~0x80);
      break;
    }
}

void
WindowManager::handle_button_press_event
    (xcb_button_press_event_t* event)
{
  // check if click happend in root window
  if (event->child == 0)
    return;

  dlog("grabing pointer in focused window");
  xcb_grab_pointer(conn, 0, get_root_window(),
                   XCB_EVENT_MASK_BUTTON_RELEASE |
                   XCB_EVENT_MASK_BUTTON_MOTION,
                   XCB_GRAB_MODE_ASYNC,
                   XCB_GRAB_MODE_ASYNC,
                   get_root_window(),
                   XCB_NONE,
                   XCB_CURRENT_TIME);

  xcb_flush(conn);
}

/**
 * this function should only be called if we are done moving a window
 */
void
WindowManager::handle_button_release_event (xcb_button_release_event_t*)
{
  dlog("Releasing pointer.");
  xcb_ungrab_pointer(conn, XCB_CURRENT_TIME);
  xcb_flush(conn);
}

void
WindowManager::handle_configure_request_event
    (xcb_configure_request_event_t* event)
{
  uint32_t values[7];
  int i = -1;

  if (event->value_mask & XCB_CONFIG_WINDOW_X)
    values[++i] = event->x;
  if (event->value_mask & XCB_CONFIG_WINDOW_Y)
    values[++i] = event->y;
  if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
    values[++i] = event->width;
  if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
    values[++i] = event->height;
  if (event->value_mask & XCB_CONFIG_WINDOW_SIBLING)
    values[++i] = event->sibling;
  if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE)
    values[++i] = event->stack_mode;

  if (i >= 0)
    {
      xcb_configure_window(conn, event->window, event->value_mask, values);
      xcb_flush(conn);
    }
}

void
WindowManager::handle_enter_notify_event(xcb_enter_notify_event_t* event)
{
  // we just care for normal events. In other case pointer is grabbed,
  // or something else we don't care happens
  //if (event->mode != XCB_NOTIFY_MODE_NORMAL)
  //  return;

  // check if we're entering the same window
  if (focus != nullptr && focus->get_id() == event->event)
    {
      dlog("moving into same window");
      return;
    }

  // we're not intrested if we're moving into the root window
  if (event->event == get_root_window())
    {
      dlog("moving into root window");
      return;
    }

  // set the focus and tell the X server about it
  set_focus(event->event);
}

void
WindowManager::handle_key_press_event(xcb_key_press_event_t* event)
{
  KeyBind key(event->state, event->detail);
  KeyBindFunc fun = keybindings[key];
  if (fun)
    fun(*this);
}

void
WindowManager::handle_map_request_event(xcb_map_request_event_t* event)
{
  Window win(conn, event->window);
  windows.insert(win);

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t values[] = { XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_PROPERTY_CHANGE |
                        XCB_EVENT_MASK_ENTER_WINDOW };
  xcb_change_window_attributes(conn, win.get_id(), mask, values);
  xcb_map_window(conn, win.get_id());
  set_focus(win.get_id());
  xcb_flush(conn);
}

/**
 * Handle the motion notify. As the name says it's just a notify.
 * It's necessary to request the pointer information 
 */
void
WindowManager::handle_motion_notify_event(xcb_motion_notify_event_t*)
{
  if (focus == nullptr)
    return;

  free_ptr<xcb_query_pointer_reply_t> pointer
    (xcb_query_pointer_reply(conn, xcb_query_pointer
                                      (conn, get_root_window()), 0),
     &std::free);

  if (pointer == nullptr)
    {
      elog("Failed to get pointer position");
      return;
    }

  // move window to new mouse position
  uint32_t values[2];
  values[0] = pointer->root_x;
  values[1] = pointer->root_y;
  xcb_configure_window(conn, focus->get_id(),
                       XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                       values);
  xcb_flush(conn);
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
}

/**
 * closes the given window. If the window supports it ICCCM is used
 */
void
WindowManager::close_window(xcb_window_t id)
{
  // first try the nice way using WM_DELETE if the window supports it
  bool be_friendly = false;
  xcb_get_property_cookie_t cookie =
    xcb_icccm_get_wm_protocols_unchecked(conn, id, atoms.wm_protocols);
  xcb_icccm_get_wm_protocols_reply_t protocols;

  if (xcb_icccm_get_wm_protocols_reply(conn, cookie, &protocols, NULL) == 1)
    for (uint32_t i = 0; i < protocols.atoms_len; ++i)
      if (protocols.atoms[i] == atoms.wm_delete_window)
        {
          be_friendly = true;
          break;
        }

  xcb_icccm_get_wm_protocols_reply_wipe(&protocols);

  if (be_friendly)
    {
      xcb_client_message_event_t event = {
          .response_type = XCB_CLIENT_MESSAGE,
          .format = 32,
          .sequence = 0,
          .window = id,
          .type = atoms.wm_protocols,
          .data.data32 = { atoms.wm_delete_window,
              XCB_CURRENT_TIME }
      };
      xcb_send_event(conn, false, id, XCB_EVENT_MASK_NO_EVENT,
                     reinterpret_cast<char*>(&event));
    }
  // the window doesn't understand ICCCM. Use the hard way
  else
    xcb_kill_client(conn, id);

  xcb_flush(conn);

  // we just killed our focused window
  focus = nullptr;
}

/**
 * sets the focus to a new window. if focus was set the old window
 * get's informed about the move
 */
void
WindowManager::set_focus(Window* win)
{
  dlog("Giving window focus to window ", win->get_id());
  // if another window had the focus notify it about the loss
  if (focus != nullptr)
    focus->remove_focus();

  win->set_focus();
  focus = win;
}
