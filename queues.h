#ifndef QUEUES_H
#define QUEUES_H

#include <QDebug>
#include <QThread>
#include <QList>
#include <QMutex>

namespace Queues
{
    // Токен постановки в очередь
    class QueueElementToken
    { };
    class QueueContainerBase;

    class QueueWorker : public QThread
    {
        Q_OBJECT
        public:
            explicit QueueWorker(QueueContainerBase *container, QObject *parent = 0);

        protected:
            void run(void);

        private:
            QueueContainerBase *container;
    };

    // Базовый класс для всех очередей
    class QueueContainerBase
    {
        public:
            QueueContainerBase();
            virtual void run(void) = 0;     // TODO: плохо-плохо, что он так публичен!!

        protected:

        private:
            QueueWorker worker;
    };

    // Простая очередь
    template <typename T>
    class SimpleQueueBase : public QueueContainerBase
    {
        public:
            SimpleQueueBase()
                : QueueContainerBase(),
                  queueMutex(), queue()
            { }

            virtual void run()
            {
                while (true)
                {
                    queueMutex.lock();
                        T element;
                        bool nonempty = !queue.isEmpty ();
                        if ( nonempty )
                            element = queue.takeFirst ();
                    queueMutex.unlock();

                    if (nonempty)
                        process(element);
                }
            }

        protected:
            QList<T> queue;
            QMutex queueMutex;
            virtual void process(T element) = 0;
            virtual QueueElementToken enqueue(T element)
            {
                queueMutex.lock();
                    queue.append(element);
                    qDebug() << "enq";
                queueMutex.unlock();

                return QueueElementToken();
            }

        private:

    };

    // Очередь с приоритетом
    template <typename T>
    class PriorityQueueBase : public SimpleQueueBase<T>
    {
        public:
            PriorityQueueBase()
                : SimpleQueueBase<T>()
            { }

        protected:
            // Сравнивает приоритеты элементов.
            // Выдаёт >0, если приоритет первого аргумента больше, чем у второго. 0, если они равны и <0 во всех остальных случаях.
            virtual int compare(T a, T b) = 0;
            virtual QueueElementToken enqueue(T element)
            {
                this->queueMutex.lock();
                    int i = 0;
                    foreach (T e, this->queue) {
                        if (compare(element, e) > 0) break;
                        i++;
                    }
                    this->queue.insert(i, element);
                this->queueMutex.unlock();
                return QueueElementToken();
            }
    };

}

#endif // QUEUES_H
