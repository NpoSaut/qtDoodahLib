#include "queues.h"

using namespace Queues;


QueueContainerBase::QueueContainerBase()
    : worker(this)
{
    worker.start();
}

void QueueWorker::run(void)
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
    QueueContainerBase(),
    queueMutex(), queue()
{
}

template <typename T>
void SimpleQueueBase<T>::run(void)
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
    queueMutex.lock();
    if (queue != nullptr)
    {
        queue->append(element);
    }
    queueMutex.unlock();
    return QueueElementToken();
}


// ---------------------------------------------------------------

template <typename T>
PriorityQueueBase<T>::PriorityQueueBase()
    : SimpleQueueBase<T>()
{
}

template <typename T>
QueueElementToken PriorityQueueBase<T>::enqueue(T element)
{
    this->queueMutex.lock();
    if (this->queue != nullptr)
    {
        int i = 0;
        foreach (T e, this->queue) {
            if (compare(element, e) > 0) break;
            i++;
        }
        this->queue.insert(i, element);
    }
    this->queueMutex.unlock();
    return QueueElementToken();
}


