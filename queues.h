#ifndef QUEUES_H
#define QUEUES_H

#include <QThread>
#include <QList>
#include <QMutex>

namespace Queues
{
    class QueueElementToken;

    // Простая очередь
    template <typename T>
    class SimpleQueueBase : public QThread
    {
            //Q_OBJECT
        public:
            explicit SimpleQueueBase(QObject *parent = 0);

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
