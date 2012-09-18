#ifndef PTR_H
#define PTR_H

#include <cstdlib>
#include <memory>

template <class T>
using free_ptr = std::unique_ptr<T, decltype(&std::free)>;

template<class T, class...Args>
std::unique_ptr<T>
make_unique(Args&&... args)
{
  std::unique_ptr<T> ptr(new T(std::forward<Args>(args)...));
  return ptr;
}

template<class T, class... Args>
free_ptr<T>
make_free(Args&&... args)
{
  return free_ptr<T>(new T(std::forward<Args>(args)...), &std::free);
}

template<class T>
free_ptr<T>
make_free_ptr(T* arg)
{
  return free_ptr<T>(arg, &std::free);
}

#endif /* PTR_H */
