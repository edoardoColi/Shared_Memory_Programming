# Releted to Lesson 13-1h and 14-1h. Slides 'C++ LockFree Basics'
Shown what are and how can be user the atomic. Using active wait grant more precision.  
Example on Sequential Consistency(SC) of atomic valiables with different memory ordering.  
Used Fences to provide sincronization also in fully relaxed memory ordering.
## Here it is a structure of the example folder
```
example/
│
├── include/
|   ├── hpc_helpers.hpp
│   └── threadPool.hpp              (Implementation of a Thread Pool)
│
├── atomic_counting.cpp             (Example of atomic that is more fast that a mutex in increment a shared counter)
├── atomic_max.cpp                  (With two atomic counter we want to find maximum, one is false other true)
│
├── sequential_ordering.cpp         (Spawn 4 threads and performe load and store atomicly, using SC as memory order. Garantee to be correct in all platform)
├── relaxed_ordering.cpp            (Same as above but testing outcome with relaxed ordering)
├── acq-rel_ordering.cpp            (We use to aquire ordering for loads and release orderind for store)
├── acq-rel_ordering2.cpp           (Previous was not exact outcome becouse there was not sincronization among variables. Garantee to be correct in all platform)
├── fence_ordering.cpp              (Testing barrier/fance behaviour to see correctness during execution. We specify use of memory_order_release/aquire otherwise compiler use the strictier one of SC,but more costly)
├── fence_ordering2.cpp
├── spin-lock.cpp                   (Implementation of spin-lock with while on a atomic_flag)
└── Makefile
```
