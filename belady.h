#pragma once
#define DEBUG
#include <map>
#include <unordered_map>
#include <deque>
#include <iterator>
#include <vector>

namespace belady{

using std::size_t;
template<typename PageT, typename KeyT>
class belady_t
{
    //for seek latest (lowest for cache) elem by O(1)
    std::unordered_map<KeyT, std::deque<size_t>> future;
    //key type for map (cache) to keep sorted by rule: later == lower
    struct KeyMap
    {
        size_t next_pos = 0;
        bool never  = false;
        KeyT key;

        /*
         * construct KeyMap by key
         * complexity: O(1)
         */
        KeyMap(const KeyT& inkey): key{inkey}
        {
            //look in deque of key
            //complexity: O(1)
            auto pair_deq = future.find(inkey);
            //if deque of key not exist
            //than key will never meet again
            if (pair_deq == future.end())
                never = true;
            //else - like key was position of first next meeting
            else
                next_pos = pair_deq->second[0];
        }

        /*
         * overload operator less for map, rule: later meet == lower
         * complexity: O(1)
         */
        bool operator< (const KeyMap& key_map) const
        {
            if (key_map.never)
                return false;
            else if (never)
                return true;
            else
                return next_pos > key_map.next_pos;
        }

        /*
         * construct KeyMap by key
         * complexity: O(1)
         */
        void update()
        {
            //look in deque by key
            auto pair_ftr = future.find(key);
            //if not found elem - this elem will never meet again
            if (pair_ftr == future.end())
            {
                next_pos = 0;
                never = true;
            }
            //else write new position in next_pos
            else
            {
                never = false;
                next_pos = pair_ftr->second[0];
            }
        }

        void rkey() const {return key;}
    };
    //data type for map (cache)
    struct DataT
    {
        PageT data;
        KeyT key;
    };

    //general conteiner
    std::map<KeyMap, DataT> cache;

    //size and capacity
    //for imulation of
    //finitness of memory
    size_t size = 0;
    size_t capacity;

    //type of data for hash_map
    using MapIt = typename std::map<KeyMap, DataT>::iterator;

    //hash_map for seeking key by O(1)
    std::unordered_map<KeyT, MapIt> hash_map;
    public:
        belady_t (size_t cap, const std::vector<KeyT>& key_vec): capacity {cap}
        {
            for (size_t i = 0; i < key_vec.size(); i++)
            {
                auto pair_deq = future.find(key_vec[i]);
                if (pair_deq == future.end())
                    future.emplace(key_vec[i], {i});
                else
                    pair_deq->second.push_back(i);
            }
        }

        bool full() const {return (size == capacity);}
        bool empty() const {return (size == 0);}

        template<typename FuncT> //FuncT - PageT slow_get_page(KeyT)
        bool lookup_update(KeyT key, FuncT slow_get_page)
        {
            auto hit = hash_map.find(key);
            update_future(key);
            if (hit == hash_map.end()) //if not found
            {
                if (need_ins(key))
                {
                    if (full())
                        erase_last();
                    insert(key, slow_get_page(key));
                }
                return false;
            }
            replace_in_map (hit->first, hit->second);
            return true;
        }
    private:
        /*
         * return true if insertion of key is efficiently, else rturn false
         * complexity: O(1)
         */
        bool need_ins(const KeyT& key) const
        {
            KeyMap key_map_cnd (key);
            KeyMap key_map_last = std::prev(cache.end())->first;
            return key_map_last < key_map_cnd;
        }
        /*
         * erase last element from cache("last" equal to "meet later")
         * complexity: O(1)  
         */ 
        void erase_last()
        {
            //erase elem from hash_map
            //complexity: O(1)
            hash_map.erase(std::prev(cache.end())->key);
            //erase eelm from cache
            //complexity: O(1)
            cache.erase(std::prev(cache.end()));
            size--;
        }

        /*
         * delete front index of met elem and delete all deque if its empty
         * complexity: O(1)
         */
        void update_future(KeyT key)
        {
            //seek deque of key
            //complexity: O(1)
            auto pair_deq = future.find(key);

            if (pair_deq == future.end())
                throw std::invalid_argument ("Meet elem that not in future");

            //pop meet index from deque
            //complexity: O(1)
            pair_deq->second.pop_frot();
            //erase empty deque
            //complexity: O(1)
            if (pair_deq.second.empty())
                future.erase(key);
        }

        /*
         * insert new element in cache
         * complexity: O(logN)  
         */ 
        void insert(const KeyT& key, const PageT& page)
        {
            //insert elem in key
            //complexity: O(logN)
            auto pair_it = cache.insert({KeyMap(key), page});
            //insert iterator on elem in hash_map
            //complexity: O(1)
            hash_map[key] = pair_it->first;
            size++;
        }

        /*
         * replace elements in map, because key_map became invalidtaed
         * complexity: O(logN)  
         */ 
        void replace_in_map(const KeyMap key_map, MapIt itr)
        {
            //update key_map 
            //insert in cache by key_map
            //copy in new node data from iterator on node with old key_map
            //save itr for hash_map
            //complexity: O(logN)
            MapIt itr_upd = cache.insert(key_map.update(), {itr->data, itr->key})->first;

            //erase old elem by iterator 
            //complexity: O(1)
            cache.erase(itr);

            //update iterator for key
            //complexity: O(1)
            hash_map[key_map.rkey()] = itr_upd;
        }
};
} //namespace