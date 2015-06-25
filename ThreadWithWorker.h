#ifndef THREADWITHWORKER_H
#define THREADWITHWORKER_H

#include <QThread>
#include "IThreadWorker.h"

// Помещает worker в создаваемый поток и запускает worker->run() при старте потока
class ThreadWithWorker : public QThread
{
    Q_OBJECT
public:
    explicit ThreadWithWorker(IThreadWorker *worker, QObject *parent = 0);
    IThreadWorker *worker;

protected slots:
    virtual void run ();

};

#endif // THREADWITHWORKER_H
