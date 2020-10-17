/**
 * @file heap.h
 * Helper class to emulate the heap in an ARMv7 computer.
 * @author Rory Pinkney
 * @date 8/10/20
 */

#ifndef IRISC_HEAP_H
#define IRISC_HEAP_H

#include <vector>

class Heap {
  private:
    std::vector<void*> heap;

  public:
    bool allocate();
};

#endif //IRISC_HEAP_H