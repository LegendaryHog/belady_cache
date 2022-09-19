#include "belady.h"
#include <vector>
#include <iostream>

int slow_get_page(int key) {return key;}

int main()
{
    std::size_t c_sz = 0;
    std::size_t d_sz = 0;

    std::cin >> c_sz >> d_sz;

    std::vector<int> data = {};

    for(std::size_t i = 0; i < d_sz; i++)
    {
        int input = 0;
        std::cin >> input;
        data.push_back(input);
    }

    belady::belady_t<int> my_cache (c_sz, data);

    std::size_t hits = 0;
    std::size_t ind = 4;
    for (auto x: data)
    {
        std::cout << std::endl << ind++ << ':' << std::endl;
        std::cout << "KEY FOR LOOKUP: " << x << std::endl;
        if(my_cache.lookup_update(x, slow_get_page) == true)
        {
            std::cout << "(!!!HIT!!!)" << std::endl;
            hits++;
        }
        my_cache.dump();
    }

    std::cout << hits << std::endl;
    return 0;
}