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

#endif /* PTR_H */
