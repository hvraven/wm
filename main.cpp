#include <unistd.h>
#include "conf.h"
#include "windowmanager.h"

int main()
{
  Configuration configuration;
  WindowManager windowmanager;
  windowmanager.run();
}
