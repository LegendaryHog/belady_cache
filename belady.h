#pragma once
#include <map>
#include <unordered_map>
#include <deque>

namespace belady{

template<typename PageT, typename KeyT>
class bealdy_t
{
    std::unordered_map<KeyT, std::deque<std::size_t>> future;

    struct KeyMap
    {
        std::size_t next_pos;
        bool never;
        bool opearator() (const KeyMap& key_map) const
        {
            if (key_map.never)
                return false;
            else if (never)
                return true;
            else
                return next_pos > key_map.next_pos;
        }
    }
};
} //namespace