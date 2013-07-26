#ifndef QUEUES_H
#define QUEUES_H

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
            PriorityQueueBase();

        protected:
            // Сравнивает приоритеты элементов.
            // Выдаёт >0, если приоритет первого аргумента больше, чем у второго. 0, если они равны и <0 во всех остальных случаях.
            virtual int compare(T a, T b) = 0;
            virtual QueueElementToken enqueue(T element);
    };

}

#endif // QUEUES_H
