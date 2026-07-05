#pragma once
#include "combined_reading.h"

template<int QueueSize>
class ReadingQueue {
    private:
        CombinedReading readings[QueueSize];
        int head = 0;
        int tail = 0;
        int count = 0;
    public:
        void add(CombinedReading newReading);
        bool take(CombinedReading& output);
        bool take();
        int size();
        void clear();
        bool empty();
};

template <int QueueSize>
inline void ReadingQueue<QueueSize>::add(CombinedReading newReading)
{

    if (tail == head && count != 0){
        take();
    }
    
    readings[tail] = newReading;
    count ++;
    tail ++;
}

template<int QueueSize>
inline bool ReadingQueue<QueueSize>::take(CombinedReading& output)
{
    if (count == 0) {
        return false;
    }

    output = readings[head];
    head = (head + 1) % QueueSize;
    count--;

    return true;
}


template<int QueueSize>
inline bool ReadingQueue<QueueSize>::take()
{
    if (count == 0) {
        return false;
    }

    head = (head + 1) % QueueSize;
    count--;

    return true;
}

template<int QueueSize>
inline void ReadingQueue<QueueSize>::clear()
{
    head = 0;
    tail = 0;
    count = 0;
}

template <int QueueSize>
inline int ReadingQueue<QueueSize>::size()
{
    return count;
}

template <int QueueSize>
inline bool ReadingQueue<QueueSize>::empty()
{
    return count == 0;
}
