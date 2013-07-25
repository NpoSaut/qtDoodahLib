#include "queues.h"

using namespace Queues;


QueueContainerBase::QueueContainerBase()
    : worker(this)
{
    worker.start();
}

void QueueWorker::run()
{
    container->run();
}

QueueWorker::QueueWorker(QueueContainerBase *container, QObject *parent)
    : QThread(parent), container(container)
{
}

// ---------------------------------------------------------------

template <typename T>
SimpleQueueBase<T>::SimpleQueueBase() :
    QThread(parent),
    queueMutex(), queue()
{
}

template <typename T>
void SimpleQueueBase<T>::run()
{
    while (true)
    {
        queueMutex.lock();
        int qsize = queue->size ();
        queueMutex.unlock();

        if (qsize > 0)
        {
            queueMutex.lock();
            T element = queue->at(0);
            queue->remove(0);
            queueMutex.unlock();

            process(element);
        }
    }
}

template <typename T>
QueueElementToken SimpleQueueBase<T>::enqueue(T element)
{
    speachMutex.lock();
    if (queue != nullptr)
    {
        queue->append(element);
        return QueueElementToken();
    }
    speachMutex.unlock();
}


// ---------------------------------------------------------------

template <typename T>
QueueElementToken PriorityQueueBase<T>::enqueue(T element)
{
    speachMutex.lock();
    if (queue != nullptr)
    {
        int i = 0;
        foreach (e, queue) {
            if (compare(element, e) > 0) break;
            i++;
        }
        queue.insert(i, element);
    }
    speachMutex.unlock();
}


