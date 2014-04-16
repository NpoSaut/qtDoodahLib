#ifndef QUEUES_H
#define QUEUES_H

#include <QDebug>
#include <QThread>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

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
                : QueueWorkerBase(),
                  maxSize (maxSize), queue(), mutex(), notEmpty(), notFull()
            { }


        protected:
            const quint32 maxSize;
            QList<T> queue;
            QMutex mutex;
            QWaitCondition notEmpty, notFull;

            // Выполняет действие над элементом, изъятым из очереди
            virtual void process(T element) = 0;

            // Ставит элемент в очередь
            virtual QueueElementToken enqueue(T element)
            {
                mutex.lock();
                {
                    if ( queue.size () == maxSize )
                        notFull.wait (&mutex);

                    queue.append(element);
                    notEmpty.wakeAll ();
                }
                mutex.unlock();

                return QueueElementToken();
            }

            bool isEmpty()
            {
                bool res;
                mutex.lock();
                    res = queue.empty ();
                mutex.unlock();
                return res;
            }

        private:
            // Извлекает элемент из очереди (в отдельном потоке)
            virtual void peek()
            {
                T element;
                mutex.lock();
                {
                    if ( queue.isEmpty () )
                        notEmpty.wait (&mutex);

                    element = queue.takeFirst ();
                    notFull.wakeAll ();
                }
                mutex.unlock();

                process(element);
            }

    };

    // Очередь с приоритетом, достающая элементы в отдельном потоке
    // ----------------------------------------------------------------------
    //  Работает как SimpleQueueBase<T>, но очередь стоится с учётом приоритета.
    //
    //  Требует, чтобы класс-наследник определил process,
    //  который производит с элементом действие по изъятии его из очереди.
    //  enqueue ставит элемент в очередь.
    template <typename T>
    class PriorityQueueBase : public SimpleQueueBase<T>
    {
        public:
            PriorityQueueBase()
                : SimpleQueueBase<T>()
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
            //  Элемент ставится вслед за элементом с приоритетом равным или большим
            virtual QueueElementToken enqueue(T element)
            {
                this->mutex.lock();
                {
                    if ( this->queue.size () == this->maxSize )
                        this->notFull.wait (&(this->mutex));

                    int i = 0;
                    foreach (T e, this->queue)
                    {
                        if (compare(element, e) > 0) break;
                        i++;
                    }
                    this->queue.insert(i, element);

                    this->notEmpty.wakeAll ();
                }
                this->mutex.unlock();
                return QueueElementToken();
            }
    };

}

#endif // QUEUES_H
