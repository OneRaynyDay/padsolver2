#pragma once
#include <map>
/** 
 * Some utility for the main header files 
 */

namespace pad {
namespace detail {
// To aid with value casting:
template <typename T>
inline decltype(auto) enum_value(T&& t) {
    std::decay_t<T> dt = t;
    return static_cast<typename std::underlying_type<std::decay_t<T>>::type>(dt);
}

// To help with getting the constant map values
template <typename K, typename V>
inline V get_value(const std::map<K, V>& m, const K& k){
    auto it = m.find(k);
    if (it != m.end()){
        return it->second;
    }
    throw std::logic_error("get_value encountered an invalid key.");
}
} // namespace detail
} // namespace pad
