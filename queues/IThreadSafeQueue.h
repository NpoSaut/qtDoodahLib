#ifndef ITHREADSAFEQUEUE_H
#define ITHREADSAFEQUEUE_H

template <typename T>
class IThreadSafeQueue
{
public:
    virtual void enqueue (const T &t) = 0;

    virtual T dequeue () = 0;
    virtual T &head () = 0;
    virtual const T &head () const = 0;
};

#endif // ITHREADSAFEQUEUE_H
