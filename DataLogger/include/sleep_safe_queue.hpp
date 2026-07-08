#pragma once

#include <iterator>
#include <type_traits>

template<typename T, int QueueSize>
class SleepSafeQueue {
    static_assert(QueueSize > 0, "QueueSize must be greater than 0");
    static_assert(std::is_trivially_copyable<T>::value,
                  "SleepSafeQueue requires trivially copyable T for sleep-safe retention");

private:
    T readings[QueueSize];
    int head = 0;
    int tail = 0;
    int count = 0;

public:
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = int;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const SleepSafeQueue* queueRef, int offsetRef)
            : queue(queueRef), offset(offsetRef)
        {
        }

        reference operator*() const
        {
            return queue->readings[(queue->head + offset) % QueueSize];
        }

        pointer operator->() const
        {
            return &(**this);
        }

        const_iterator& operator++()
        {
            ++offset;
            return *this;
        }

        bool operator==(const const_iterator& other) const
        {
            return queue == other.queue && offset == other.offset;
        }

        bool operator!=(const const_iterator& other) const
        {
            return !(*this == other);
        }

    private:
        const SleepSafeQueue* queue;
        int offset;
    };

    void add(const T& newReading);
    bool take(T& output);
    bool take();
    int size() const;
    bool empty() const;
    void clear();
    const_iterator begin() const;
    const_iterator end() const;
};

template<typename T, int QueueSize>
inline void SleepSafeQueue<T, QueueSize>::add(const T& newReading)
{
    if (count == QueueSize) {
        take(); // discard oldest
    }

    readings[tail] = newReading;
    tail = (tail + 1) % QueueSize;
    count++;
}

template<typename T, int QueueSize>
inline bool SleepSafeQueue<T, QueueSize>::take(T& output)
{
    if (count == 0) return false;

    output = readings[head];
    head = (head + 1) % QueueSize;
    count--;

    return true;
}

template<typename T, int QueueSize>
inline bool SleepSafeQueue<T, QueueSize>::take()
{
    if (count == 0) return false;

    head = (head + 1) % QueueSize;
    count--;

    return true;
}

template<typename T, int QueueSize>
inline void SleepSafeQueue<T, QueueSize>::clear()
{
    head = 0;
    tail = 0;
    count = 0;
}

template<typename T, int QueueSize>
inline int SleepSafeQueue<T, QueueSize>::size() const
{
    return count;
}

template<typename T, int QueueSize>
inline bool SleepSafeQueue<T, QueueSize>::empty() const
{
    return count == 0;
}

template<typename T, int QueueSize>
inline typename SleepSafeQueue<T, QueueSize>::const_iterator SleepSafeQueue<T, QueueSize>::begin() const
{
    return const_iterator(this, 0);
}

template<typename T, int QueueSize>
inline typename SleepSafeQueue<T, QueueSize>::const_iterator SleepSafeQueue<T, QueueSize>::end() const
{
    return const_iterator(this, count);
}