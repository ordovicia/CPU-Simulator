#pragma once

#include <deque>

template <typename Type, int Size>
struct SizedDeque {
    std::deque<Type> deque;

    using Iterator = typename std::deque<Type>::const_iterator;

    decltype(std::prev(deque.end())) push(Type v)
    {
        if (deque.size() >= Size)
            deque.pop_front();
        deque.emplace_back(std::move(v));
        return std::prev(deque.end());
    }
};
