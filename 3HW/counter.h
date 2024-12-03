#ifndef COUNTER_H
#define COUNTER_H

#include <atomic>

extern std::atomic<int> counter;

void start_counter_increment();

#endif // COUNTER_H
