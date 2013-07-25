#ifndef QUEUES_H
#define QUEUES_H

#include <QThread>
#include <QList>
#include <QMutex>

namespace Queues
{
    class QueueElementToken;
    class QueueContainerBase;

    class QueueWorker : public QThread
    {
        Q_OBJECT
        private:
            QueueContainerBase *container;

        protected:

        public:
            void run();
            explicit QueueWorker(QueueContainerBase *container, QObject *parent = 0);
    };

    // Базовый класс для всех очередей
    class QueueContainerBase
    {
        public:
            QueueContainerBase();
            virtual void run() = 0;     // TODO: плохо-плохо, что он так публичен!!

        protected:

        private:
            QueueWorker worker;
    };

    // Простая очередь
    template <typename T>
    class SimpleQueueBase : QueueContainerBase
    {
        public:
            SimpleQueueBase();

        signals:

        public slots:

        protected:
            QMutex queueMutex;
            void run();
            virtual void process(T element) = 0;
            virtual QueueElementToken enqueue(T element);

        private:
            QList<T> queue;

    };


    // Очередь с приоритетом
    template <typename T>
    class PriorityQueueBase : SimpleQueueBase<T>
    {
            //Q_OBJECT
        public:
            explicit PriorityQueueBase(QObject *parent = 0);

        signals:

        public slots:

        protected:
            // Сравнивает приоритеты элементов.
            // Выдаёт >0, если приоритет первого аргумента больше, чем у второго. 0, если они равны и <0 во всех остальных случаях.
            virtual int compare(T a, T b) = 0;
            QueueElementToken enqueue(T element);

        private:
    };


    // Токен постановки в очередь
    class QueueElementToken
    {

    };
}

#endif // QUEUES_H
