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
            virtual void run() = 0;

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
            SimpleQueueBase()
                : QueueWorkerBase(),
                  queueMutex(), queue()
            { }


        protected:
            QList<T> queue;
            QMutex queueMutex;

            // Выполняет действие над элементом, изъятым из очереди
            virtual void process(T element) = 0;

            // Ставит элемент в очередь
            virtual QueueElementToken enqueue(T element)
            {
                queueMutex.lock();
                    queue.append(element);
                    qDebug() << "enq";
                queueMutex.unlock();

                return QueueElementToken();
            }

            bool isEmpty()
            {
                bool res;
                queueMutex.lock();
                    res = queue.empty ();
                queueMutex.unlock();
                return res;
            }

        private:
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
