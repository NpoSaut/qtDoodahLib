#ifndef LINERTHREADSAFEQUEUE_H
#define LINERTHREADSAFEQUEUE_H

#include "IThreadSafeQueue.h"

#include <QList>
#include <QMutex>
#include <QWaitCondition>

template <typename T>
class LinerThreadSafeQueue : public IThreadSafeQueue<T>
{
public:
    LinerThreadSafeQueue(unsigned maxSize)
        : maxSize (maxSize), queue(), mutex(), notEmpty(), notFull()
    { }

    virtual void enqueue (const T &t)
    {
        mutex.lock();
        {
            if ( queue.size () == maxSize )
                notFull.wait (&mutex);

            queue.append(t);
            notEmpty.wakeAll ();
        }
        mutex.unlock();
    }

    virtual T dequeue ()
    {
        T element;
        mutex.lock();
        {
            if ( queue.isEmpty () )
                notEmpty.wait (&mutex);

            element = queue.takeFirst ();
            notFull.wakeAll ();
        }
        mutex.unlock();
        return element;
    }

    virtual T &head ()
    {
        T &element;
        mutex.lock();
        {
            if ( queue.isEmpty () )
                notEmpty.wait (&mutex);

            element = queue.first ();
            notFull.wakeAll ();
        }
        mutex.unlock();
        return element;
    }

    virtual const T &head () const
    {
        const T &element;
        mutex.lock();
        {
            if ( queue.isEmpty () )
                notEmpty.wait (&mutex);

            element = queue.first ();
            notFull.wakeAll ();
        }
        mutex.unlock();
        return element;
    }

    virtual bool isEmpty () const
    {
        bool res;
        mutex.lock();
            res = queue.empty ();
        mutex.unlock();
        return res;
    }

protected:
    const unsigned maxSize;
    QList<T> queue;
    QMutex mutex;
    QWaitCondition notEmpty, notFull;
};

#endif // LINERTHREADSAFEQUEUE_H
