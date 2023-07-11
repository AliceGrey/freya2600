#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>

template <typename N, typename H>
inline bool IsIn(const N& needle, std::initializer_list<H> haystack)
{
    auto it = std::find(haystack.begin(), haystack.end(), needle);
    return (it != haystack.end());
}

#endif // UTILITY_HPP