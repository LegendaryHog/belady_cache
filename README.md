# My belady algorithm on C++
Its my algorithm belady (ideal algorithm of cache). I wrote too many comments in `belady.h` to you, I wrote comment on each line to you, you should read them.

**1.Cmake.**

```
cmake -S . -B build
cd build/
make
```

**2.Tests**

Generation of tests:
```
cd test/
chmod "+x" testgen.py
./testgen.py [FILE] [NUMBER_OF_KEYS] [CACHE_CAPACITY] [OPTIONAL]
```
[FILE] - file to put test, "answ_[FILE]" - file to put number of hits

[NUMBER_OF_KEYS] - number of keys

[CACHE_CAPACITY] - capacity of cache

[OPTIONAL] - if argument exist file with answer will not created and hits will  not counted

Test of answers:
```
chmod "+x" test_answ
./test_answ -b [EXE_OF_BELADY] [FILE_TEST1] ...
``` 

**3. Compare with other cache.**
```
chmod "+x" cmp
./cmp -b [EXE_OF_BELADY] -o [EXE_OF_OTHER_CACHE] [FILE_TEST1] ...
```