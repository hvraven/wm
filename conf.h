#ifndef CONF_H
#define CONF_H

#include <cstdint>

class Configuration
{
public:
  Configuration();

  int      border_width;
  uint32_t border_color_unfocused;
  uint32_t border_color_focused;

private:
  uint32_t get_color(const char* color_name);
};

#endif /* CONF_H */
