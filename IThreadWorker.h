#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

#include <QObject>

// Объект, который должен выполнять работу в отдельном потоке
class IThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit IThreadWorker();

public slots:
    virtual void run () = 0;
};

#endif // ITHREADWORKER_H
