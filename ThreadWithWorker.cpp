#include "ThreadWithWorker.h"

ThreadWithWorker::ThreadWithWorker(IThreadWorker *worker, QObject *parent) :
    QThread(parent), worker(worker)
{
    worker->moveToThread(this);
}

void ThreadWithWorker::run()
{
    QThread::run();
    worker->run();
}
