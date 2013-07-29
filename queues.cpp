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


// ----------------------- PriorityQueueBase ------------------------



