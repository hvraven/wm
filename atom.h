#ifndef ATOM_H
#define ATOM_H

#include <xcb/xcb.h>

class Atoms
{
public:
  Atoms() {};
  Atoms(xcb_connection_t*);

  xcb_atom_t wm_delete_window;
  xcb_atom_t wm_protocols;

};

#endif /* ATOM_H */
