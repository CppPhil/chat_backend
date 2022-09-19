#pragma once
#include <pl/algo/ranged_algorithms.hpp>

#include <utility>

namespace lib {
template<typename Container, typename Element>
[[nodiscard]] bool contains(const Container& container, Element&& element)
{
  return pl::algo::find(container, std::forward<Element>(element))
         != std::end(container);
}
} // namespace lib
