#include "ThreadWithWorker.h"

ThreadWithWorker::ThreadWithWorker(IThreadWorker *worker, QObject *parent) :
    QThread(parent), worker(worker)
{
    worker->moveToThread(this);
    QObject::connect(this, SIGNAL(started()), worker, SLOT(run()), Qt::QueuedConnection);
}
