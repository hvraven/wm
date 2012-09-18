#ifndef CONF_H
#define CONF_H

#include "bindings.h"
#include <cstdint>

class Configuration
{
public:
  Configuration();
  void complete_initialization();

  int      border_width;
  uint32_t border_color_unfocused;
  uint32_t border_color_focused;

  KeyBindConfigs   keybindconfigs;
  MouseBindConfigs mousebindconfigs;
};

#endif /* CONF_H */
