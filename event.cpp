#include "global.h"
#include "ptr.h"

typedef free_ptr<xcb_generic_event_t> generic_event_ptr;

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
    case XCB_CONFIGURE_NOTIFY:
      dlog("Event of type \'XCB_CONFIGURE_NOTIFY\' found.");
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

  // we need a focus to handle this event
  if (focus == nullptr)
    return;

  switch(event->detail)
    {
    case XCB_BUTTON_INDEX_1:
      dlog("Starting window movement");
      window_state = Move;
      xcb_warp_pointer(conn, XCB_NONE, focus->id,
                       0, 0, 0, 0, 1, 1);
      break;
    case XCB_BUTTON_INDEX_2:
      dlog("Starting resizing");
      window_state = Resize;
      xcb_warp_pointer(conn, XCB_NONE, focus->id,
                       0, 0, 0, 0, focus->width, focus->height);
      break;
    default:
      return;
    }

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
  window_state = None;
  xcb_flush(conn);
}

void
WindowManager::handle_configure_request_event
    (xcb_configure_request_event_t* event)
{
  uint32_t values[6];
  int i = -1;

  auto win = windows[event->window].get();
  if (win != nullptr)
    {
      if (event->value_mask & XCB_CONFIG_WINDOW_X)
        {
          values[++i] = event->x;
          win->x = event->x;
        }
      if (event->value_mask & XCB_CONFIG_WINDOW_Y)
        {
          values[++i] = event->x;
          win->y = event->y;
        }
      if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
        {
          values[++i] = event->width;
          win->width = event->width;
        }
      if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
        {
          values[++i] = event->height;
          win->height = event->height;
        }
      if (event->value_mask & XCB_CONFIG_WINDOW_SIBLING)
        values[++i] = event->sibling;
      if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE)
        values[++i] = event->stack_mode;
    }
  else
    {
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
    }

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
  // or something else we don't care what happens
  if (event->mode != XCB_NOTIFY_MODE_NORMAL)
    return;

  // we're not intrested if we're moving into the root window
  if (event->event == get_root_window())
    {
      dlog("moving into root window");
      return;
    }

  // tells the matching window it will get the focus
  auto& win = windows[event->event];
  if (win == nullptr)
    elog("Moved into unknown window");
  else
   win->get_focus();
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
  BasicWindow* win = windows.new_xwindow(event->window);

  // for now every window is a child of the root window
  win->set_parent(this);

  dlog("Initialized new XWindow with id=", win->id);

  xcb_map_window(conn, win->id);
  win->get_focus();
  xcb_flush(conn);
}

/**
 * Handle the motion notify. As the name says it's just a notify.
 * It's necessary to request the pointer information 
 */
void
WindowManager::handle_motion_notify_event(xcb_motion_notify_event_t*)
{
  // we just move if the have a focused window
  if (focus == nullptr)
    {
      dlog("No focused window. Not moving");
      return;
    }

  free_ptr<xcb_query_pointer_reply_t> pointer
    (xcb_query_pointer_reply(conn, xcb_query_pointer
                             (conn, get_root_window()), 0),
     &std::free);

  if (pointer == nullptr)
    {
      elog("Failed to get pointer position");
      return;
    }

  switch (window_state)
    {
    case Move:
      focus->move(pointer.get());
      break;
    case Resize:
      focus->resize(pointer.get());
      break;
    case None:
      elog("Got a motion notify with window_state none");
      return;
    }
}

