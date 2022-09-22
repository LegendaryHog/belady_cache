#!/usr/bin/env python3

from io import TextIOWrapper
from typing import List
import random, sys
from pathlib import Path
import os 

class arr_int:
    size: int = 0
    arr: List[int] = []
    def __init__(self, sz: int):
        self.size = sz
        for i in range (self.size):
            self.arr.append(0)


def generate_test(quantity_of_keys: int):
    keys_arr: arr_int = arr_int(quantity_of_keys)
    for i in range(keys_arr.size):
        keys_arr.arr[i] = random.randint(1, max(5, int(quantity_of_keys/2)))
    return keys_arr

#for simplification data an key is the same
class _cache_elem_:
    key: int = 0
    def __init__(self, key: int):
        self = key

class _cache_:
    size: int = 0
    cache: List[_cache_elem_] = []
    future: List[int] = []
    ind_in_ftr: int = 0

    def __init__(self, sz: int, ftr: arr_int):
        self.future = ftr.arr
        self.size = sz

    def find(self, key: int) -> int:
        hit: int = -1 
        for i in range(len(self.cache)):
            if self.cache[i] == key:
                hit = i
                return hit
        return hit

    def look_in_future(self, key: int) -> int:
        ret: int = len(self.future)
        for i in range(self.ind_in_ftr, len(self.future)):
            if (self.future[i] == key):
                ret = i
                return ret
        return ret


    def need_ins(self, key_ins: int) ->bool:
        hit = self.look_in_future(key_ins)
        if len(self.cache) == 0 & hit != len(self.future):
            return True
        for i in range(len(self.cache)):
            hit_i = self.look_in_future(self.cache[i])
            if (hit_i > hit):
                return True
        return False

    def pop_lower(self):
        lower_pos: int = self.look_in_future(self.cache[0])
        ind_lower: int = 0
        for i in range(1, len(self.cache)):
            hit_i = self.look_in_future(self.cache[i])
            if hit_i > lower_pos:
                lower_pos = hit_i
                ind_lower = i
        self.cache.pop(ind_lower)

    def push_back(self, key: int):
        self.cache.append(key)

    def remove_if_never(self, ind: int):
        if (self.look_in_future(self.cache[ind]) == -1):
            self.cache.pop(ind)

    def full(self) -> bool:
        if self.size == len(self.cache):
            return True
        return False

    def lookup_update(self, key: int) -> bool:
        hit = self.find(key)
        self.ind_in_ftr += 1
        if hit == -1: #if not found
            if (self.need_ins(key)):
                if(self.full()):
                    self.pop_lower()
                self.push_back(key)
            return False
        else: #if found
            self.remove_if_never(hit)  
            return True

def generate_answer (keys_arr: arr_int, cache_capacity: int):
    cache = _cache_(cache_capacity, keys_arr)
    hits: int = 0
    for i in range(keys_arr.size):
        if cache.lookup_update(keys_arr.arr[i]) == True:
            hits += 1
    return cache, hits


def print_in_file_test (f: TextIOWrapper, keys_arr: arr_int, capacity: int):
    f.write(str(capacity))
    f.write('\n\n')
    f.write(str(keys_arr.size))
    f.write('\n\n')
    for i in range(keys_arr.size):
        f.write(str(keys_arr.arr[i]))
        f.write(' ')
    f.write('\n\n')
    
def print_in_file_answ (f: TextIOWrapper, cache: _cache_, hits: int):
    f.write(str(hits))

def main():
    quantity_of_keys: int = max(10, int(sys.argv[2]))
    keys_arr: arr_int = generate_test(quantity_of_keys)
    
    path, file_name_test = os.path.split(sys.argv[1])
    if (len(path) != 0):
        path += '/'
    f_test: TextIOWrapper = open(path + file_name_test, 'w')

    print_in_file_test(f_test, keys_arr, max(int(quantity_of_keys/32), 4))
    f_test.close()

    if len(sys.argv) <= 3:
        file_name_answ: str = 'answ_' + file_name_test
        f_answ: TextIOWrapper = open(path + file_name_answ, 'w')
        cache, hits = generate_answer(keys_arr, max(int(quantity_of_keys/32), 4))
        print_in_file_answ(f_answ, cache, hits)
        f_answ.close()

main()
    


