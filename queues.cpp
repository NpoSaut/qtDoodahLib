#include "queues.h"

using namespace Queues;

// ------------------------- QueueWorker ----------------------------

void QueueWorker::run(void)
{
    container->run();
}

QueueWorker::QueueWorker(QueueContainerBase *container, QObject *parent)
    : QThread(parent), container(container)
{
}

// ---------------------- QueueContainerBase ------------------------

QueueContainerBase::QueueContainerBase()
    : worker(this)
{
    worker.start();
}

// ----------------------- SimpleQueueBase --------------------------

//template<typename T>
//SimpleQueueBase<T>::SimpleQueueBase ()
//    : QueueContainerBase(),
//      queueMutex(), queue()
//{ }

//template <typename T>
//void SimpleQueueBase<T>::run()
//{
//    while (true)
//    {
//        queueMutex.lock();
//            T element;
//            bool nonempty = !queue.isEmpty ();
//            if ( nonempty )
//                element = queue.takeFirst ();
//        queueMutex.unlock();

//        if (nonempty)
//            process(element);
//    }
//}

//template <typename T>
//QueueElementToken SimpleQueueBase<T>::enqueue(T element)
//{
//    queueMutex.lock();
//        queue.append(element);
//    queueMutex.unlock();

//    return QueueElementToken();
//}


// ----------------------- PriorityQueueBase ------------------------

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

