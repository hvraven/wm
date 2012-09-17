#include <unistd.h>
#include "conf.h"
#include "windowmanager.h"

WindowManager* wm;
Configuration* config;

int main()
{
  WindowManager windowmanager;
  wm = &windowmanager;
  Configuration configuration;
  config = &configuration;
  windowmanager.run();
}
