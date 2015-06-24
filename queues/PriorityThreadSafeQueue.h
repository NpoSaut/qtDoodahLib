#ifndef PRIORITYTHREADSAFEQUEUE_H
#define PRIORITYTHREADSAFEQUEUE_H

#include "LinerThreadSafeQueue.h"

template <typename T>
class PriorityThreadSafeQueue : public LinerThreadSafeQueue<T>
{
public:
    PriorityThreadSafeQueue(unsigned maxSize)
        : LinerThreadSafeQueue<T>(maxSize)
    { }

    virtual void enqueue (const T &t)
    {
        this->mutex.lock();
        {
            if ( this->queue.size () == this->maxSize )
                this->notFull.wait (&(this->mutex));

            int i = 0;
            foreach (T e, this->queue)
            {
                if (compare(t, e) > 0) break;
                i++;
            }
            this->queue.insert(i, t);

            this->notEmpty.wakeAll ();
        }
        this->mutex.unlock();
    }

protected:
    // Сравнивает приоритеты элементов. Возвращает:
    //  >0, если приоритет первого аргумента больше, чем у второго.
    //  =0, если они равны
    //  <0  во всех остальных случаях.
    virtual int compare(T a, T b) = 0;
};

#endif // PRIORITYTHREADSAFEQUEUE_H
