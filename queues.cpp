#include "queues.h"

using namespace Queues;

// ------------------------- QueueWorker ----------------------------

void QueueWorker::run(void)
{
    while (true)
    {
        container->peek();
        usleep (0);
    }
}

QueueWorker::QueueWorker(QueueWorkerBase *container, QObject *parent)
    : QThread(parent), container(container)
{
}

// ---------------------- QueueContainerBase ------------------------

QueueWorkerBase::QueueWorkerBase()
    : worker(this)
{
    worker.start();
}

// ----------------------- SimpleQueueBase --------------------------


// ----------------------- PriorityQueueBase ------------------------



