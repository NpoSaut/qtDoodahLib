#ifndef QUEUES_H
#define QUEUES_H

#include <QDebug>
#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

#include "queues/PriorityThreadSafeQueue.h"

namespace Queues
{
    // Токен постановки в очередь
    class QueueElementToken
    { };
    class QueueWorkerBase;

    // Поток, используемый в QueueWorkerBase
    class QueueWorker : public QThread
    {
        Q_OBJECT
        public:
            explicit QueueWorker(QueueWorkerBase *container, QObject *parent = 0);

        protected:
            void run(void);

        private:
            QueueWorkerBase *container;
    };

    // Базовый класс для всех очередей
    //  нужен для того, чтобы избежать наследования в шаблонном классе от QObject (не работает)
    class QueueWorkerBase
    {
        friend class QueueWorker;
        public:
            QueueWorkerBase();

        protected:
            // Вызвается в отдельном потоке
            virtual void peek() = 0;

        private:
            // Поток, вызывающий функцию run()
            QueueWorker worker;
    };

    // Простая очередь без приоритетов, достающая элементы в отдельном потоке
    // ----------------------------------------------------------------------
    //  Предполагается, что операция, производимая с элементом после изъятия его из очереди, достаточно долгая.
    //  Поэтому отдельный поток занимается тем, что дожидается окончания этой операции,
    //  изымает следующий элемент и выполняет над ним эту долгую операцию.
    //
    //  Требует, чтобы класс-наследник определил process,
    //  который производит с элементом действие по изъятии его из очереди.
    //  enqueue ставит элемент в очередь.
    template <typename T>
    class SimpleQueueBase : private QueueWorkerBase
    {
        public:
            SimpleQueueBase(quint32 maxSize = 100000)
                : QueueWorkerBase(), queue(maxSize)
            { }


        protected:
            // Выполняет действие над элементом, изъятым из очереди
            virtual void process(T element) = 0;

            // Ставит элемент в очередь
            virtual QueueElementToken enqueue(T element)
            {
                queue.enqueue(element);
                return QueueElementToken();
            }

            bool isEmpty()
            {
                queue.isEmpty();
            }

        private:
            // Извлекает элемент из очереди (в отдельном потоке)
            virtual void peek()
            {
                process(queue.dequeue());
            }

            LinerThreadSafeQueue<T> queue;
    };

    // Очередь с приоритетом, достающая элементы в отдельном потоке
    // ----------------------------------------------------------------------
    //  Работает как SimpleQueueBase<T>, но очередь стоится с учётом приоритета.
    //
    //  Требует, чтобы класс-наследник определил process,
    //  который производит с элементом действие по изъятии его из очереди.
    //  enqueue ставит элемент в очередь.
    template <typename T>
    class PriorityQueueBase : private QueueWorkerBase
    {
        public:
            PriorityQueueBase(quint32 maxSize = 100000)
                : QueueWorkerBase(), queue(maxSize, this)
            { }

        protected:
            // Сравнивает приоритеты элементов. Возвращает:
            //  >0, если приоритет первого аргумента больше, чем у второго.
            //  =0, если они равны
            //  <0  во всех остальных случаях.
            virtual int compare(T a, T b) = 0;

            // Выполняет действие над элементом, изъятым из очереди
            virtual void process(T element) = 0;

            // Ставит элемент в очередь
            virtual QueueElementToken enqueue(T element)
            {
                queue.enqueue(element);
                return QueueElementToken();
            }

            bool isEmpty()
            {
                queue.isEmpty();
            }

        private:
            // Извлекает элемент из очереди (в отдельном потоке)
            virtual void peek()
            {
                process(queue.dequeue());
            }

            class MyPriorityThreadSafeQueue : public PriorityThreadSafeQueue<T>
            {
            public:
                MyPriorityThreadSafeQueue(unsigned maxSize, PriorityQueueBase<T> *parent)
                    : PriorityThreadSafeQueue<T> (maxSize), parent (parent)
                { }

            private:
                PriorityQueueBase<T> *parent;
                int compare (T a, T b)
                {
                    return parent->compare(a, b);
                }
            };
            MyPriorityThreadSafeQueue queue;

    };

}

#endif // QUEUES_H
