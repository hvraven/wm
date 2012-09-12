#include <unistd.h>
#include "windowmanager.h"

WindowManager* wm;

int main()
{
  WindowManager windowmanager;
  wm = &windowmanager;
  windowmanager.run();
}
