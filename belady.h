#pragma once
#define DEBUG
#include <map>
#include <unordered_map>
#include <deque>
#include <iterator>
#include <vector>
#include <iostream>

namespace belady{

using std::size_t;
template<typename PageT, typename KeyT = int>
class belady_t
{
    //for seek latest (lowest for cache) elem by O(1)
    std::unordered_map<KeyT, std::deque<size_t>> future;
    //key type for map (cache) to keep sorted by rule: later == bigger
    struct KeyMap
    {
        size_t next_pos = 0;
        bool never  = false;
        KeyT key;

        /*
         * construct KeyMap by key
         * complexity: O(1)
         */
        KeyMap(const KeyT& inkey, const std::unordered_map<KeyT, std::deque<size_t>>& future): key{inkey}
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
         * overload operator less for map, rule: later meet == bigger
         * complexity: O(1)
         */
        bool operator< (const KeyMap& key_map) const
        {
            if (never)
                return false;
            else if (key_map.never)
                return true;
            else
                return next_pos < key_map.next_pos;
        }

        /*
         * construct KeyMap by key
         * complexity: O(1)
         */
        void update(const std::unordered_map<KeyT, std::deque<size_t>>& future)
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

        KeyT rkey() const {return key;}
    };
    //data type for map (cache)
    struct DataT
    {
        PageT page;
        KeyT key;
    };

    //general conteiner
    std::map<KeyMap, DataT> cache;

    //size and capacity
    //for imulation of
    //finitness of memory
    size_t size = 0;
    size_t capacity;

    //type of page for hash_map
    using MapIt = typename std::map<KeyMap, DataT>::iterator;

    //hash_map for seeking key by O(1)
    std::unordered_map<KeyT, MapIt> hash_map;
    public:
        /*
         * constructor of cache: take sequence of requests and capacity
         * complexity: O(N) (!!!now and later M - size of cache, N - number of requests!!!)
         */
        belady_t (size_t cap, const std::vector<KeyT>& key_vec): capacity {cap}
        {
            for (size_t i = 0; i < key_vec.size(); i++)
            {
                auto pair_deq = future.find(key_vec[i]);
                if (pair_deq == future.end())
                    future[key_vec[i]] = {i};
                else
                    pair_deq->second.push_back(i);
            }
        }

        bool full() const {return (size == capacity);}
        bool empty() const {return (size == 0);}

        /*
         * general public function to check element in cache by key and update cache
         * complexity: O(logM)
         */
        template<typename FuncT> //FuncT - PageT slow_get_page(KeyT)
        bool lookup_update(KeyT key, FuncT slow_get_page)
        {
            //complexity: O(1)
            auto hit = hash_map.find(key);
            //complexity: O(1)
            update_future(key);
            if (hit == hash_map.end()) //if not found
            {
                //check inside if complexity: O(1)
                if (need_ins(key))
                {
                    if (full())
                        //complexity: O(1)
                        erase(std::prev(cache.end()));
                    //complexity: O(logM) 
                    insert(key, slow_get_page(key));
                }
                return false;
            }
            //complexity: O(logM)
            replace_in_map (KeyMap(hit->first, future), hit->second);
            return true;
        }

        /*
         * dump of cache for debug
         * complexity: no matter
         */
        void dump()
        {
            std::cout << "DUMP:" << std::endl;
            std::cout << "CAPCITY: " << capacity << std::endl;
            std::cout << "SIZE: "<< cache.size() << std::endl;
            std::cout << "CACHE:\n{" << std::endl;
            for (auto elem: cache)
            {
                std::cout << "\tKEY: " << elem.second.key << std::endl;
                std::cout << "\tMEXT MET: ";
                if (elem.first.never)
                    std::cout << "never";
                else
                    std::cout << elem.first.next_pos;
                std::cout << std::endl;
            }
            std::cout << '}' << std::endl;
        }
    private:
        /*
         * return true if insertion of key is efficiently, else rturn false
         * complexity: O(1)
         */
        bool need_ins(const KeyT& key) const
        {
            KeyMap key_map_cnd (key, future);
            if (key_map_cnd.never)
                return false;
            if (empty())
                return true;
            KeyMap key_map_last = std::prev(cache.end())->first;
            return key_map_cnd < key_map_last;
        }

        /*
         * erase element by iterator from cache
         * complexity: O(1)  
         */ 
        void erase(MapIt itr)
        {
            //erase elem from hash_map
            //complexity: O(1)
            hash_map.erase(itr->second.key);
            //erase eelm from cache
            //complexity: O(1)
            cache.erase(itr);
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
            pair_deq->second.pop_front();
            //erase empty deque
            //complexity: O(1)
            if (pair_deq->second.empty())
                future.erase(key);
        }

        /*
         * insert new element in cache
         * complexity: O(logM)  
         */ 
        void insert(const KeyT& key, const PageT& page)
        {
            //insert elem in key
            //complexity: O(logM)
            auto pair_it = cache.insert({KeyMap(key, future), {page, key}});
            //insert iterator on elem in hash_map
            //complexity: O(1)
            hash_map[key] = pair_it.first;
            size++;
        }

        /*
         * replace elements in map, because key_map became invalidtaed
         * complexity: O(logM)  
         */ 
        void replace_in_map(KeyMap key_map, MapIt itr)
        {
            //update key_map 
            //insert in cache by key_map
            //copy in new node page from iterator on node with old key_map
            //save itr for hash_map
            //complexity: O(logM)
            key_map.update(future);
            //if this element will never meet again - erase
            if (key_map.never)
            {
                erase(itr);
                return;
            }

            auto pair_upd = cache.insert({key_map, {itr->second.page, itr->second.key}});
            MapIt itr_upd = pair_upd.first;
            if (pair_upd.second == false)
                throw std::invalid_argument ("no insert");

            //erase old elem by iterator 
            //complexity: O(1)
            cache.erase(itr);

            //update iterator for key
            //complexity: O(1)
            hash_map[key_map.rkey()] = itr_upd;
        }
};
} //namespace