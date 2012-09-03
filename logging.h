#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>

template <class T>
void
log_helper(std::ostream& s, const T& e)
{
  s << e;
}

template <class T, typename... Args>
void
log_helper(std::ostream& s, const T& e, const Args&... args)
{
  s << e;
  log_helper(s, args...);
}

template <typename... Args>
void
dlog(const Args&... args)
{
  std::cerr << "(DEBUG) ";
  log_helper(std::cerr, args...);
  std::cerr << std::endl;
}

template <typename... Args>
void
wlog(const Args&... args)
{
  std::cerr << "(WARN)  ";
  log_helper(std::cerr, args...);
  std::cerr << std::endl;
}

template <typename... Args>
void
elog(const Args&... args)
{
  std::cerr << "(ERROR) ";
  log_helper(std::cerr, args...);
  std::cerr << std::endl;
}

#endif /* LOGGING_H */
