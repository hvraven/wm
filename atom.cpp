#include "atom.h"
#include "ptr.h"
#include <cstring>
#include <stdexcept>
#include <string>

/**
 * ask the X server for a atom 
 */
xcb_atom_t
request_atom(xcb_connection_t* conn, const char* atom_name)
{
  xcb_intern_atom_cookie_t cookie = 
    xcb_intern_atom(conn, 0, strlen(atom_name), atom_name);
  free_ptr<xcb_intern_atom_reply_t> reply
    (xcb_intern_atom_reply(conn, cookie, nullptr), &std::free);

  if (reply == nullptr)
    throw std::runtime_error(std::string("Failed to get atom ") + 
                             atom_name + " from X server");

  return reply->atom;
}

/**
 * ask and store the required atoms
 */
Atoms::Atoms(xcb_connection_t* conn)
  : wm_delete_window(request_atom(conn, "WM_DELETE_WINDOW")),
    wm_protocols(request_atom(conn, "WM_PROTOCOLS"))
{
}
