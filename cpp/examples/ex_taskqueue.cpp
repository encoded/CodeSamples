#include <stdlib.h>
#include <thread>

#include "LogMutex.h"
#include "TaskQueue.h"

/*
    Example task queue.
    Implementation of a thread-safe queue in a multi-threaded environment.
    The queue here is used to send some jobs from a producer to a consumer thread.
*/

/************************ PARAMS ****************************/

const int g_taskQueueSize = 10;

/************************************************************/

class Worker
{
public:
    Worker():
        m_queue(g_taskQueueSize),
        m_workerThread(&Worker::update, this),
        m_workerThreadRunningFlag(true)
    {

    }

    ~Worker()
    {
        m_workerThreadRunningFlag.store(false); // signal the update thread to stop
        if (m_workerThread.joinable())
        {
            m_workerThread.join(); //wait for the write thread to finish
        }
    }

    void addJob()
    {
        /* For simplicity the job is just a text message */
        auto task = [](void* context, void* params, float delatTime)
        {
            Worker* worker = (Worker*)context;
            LM_LOG("This is a task...");
        };

        m_queue.push(task, this);
    }

private:
    /* worker thread update function */
    void update()
    {
        while (m_workerThreadRunningFlag.load())
        {   
            // No processing if there are no tasks to process
            if(!m_queue.getNumTasks())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep to avoid busy-waiting
                continue;
            }

            LM_LOG("Begin frame.");

            /* For simplicity we define a fix size of number of tasks per frame. */
            int numProcessedTasks = 0;
            int deltaTimeMilliseconds = 0; //delta time between tasks in milliseconds
            while(numProcessedTasks < m_numMaxTasksPerFrame)
            {
                TaskQueue::Task task;
                if(!m_queue.pop(task))
                {
                    break;
                }

                task.execute(deltaTimeMilliseconds);

                /* Wait some time to simulate some time spent processing tasks */
                deltaTimeMilliseconds = 200; // this is arbitrary for demonstration porpuses
                std::this_thread::sleep_for(std::chrono::milliseconds(deltaTimeMilliseconds));

                ++numProcessedTasks;
            }

            LM_LOG("End frame.");
        }
    }

    TaskQueue m_queue;
    std::thread m_workerThread;
    std::atomic<bool> m_workerThreadRunningFlag; //atomic flag to control the lifetime of the update thread

    const int m_numMaxTasksPerFrame = 2;
};

int main(int argc, char* argv[])
{
    printf("Example task queue...\n");

    Worker worker;

    worker.addJob();
    worker.addJob();
    worker.addJob();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    worker.addJob();
    worker.addJob();
    worker.addJob();
    worker.addJob();
    worker.addJob();
    worker.addJob();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return EXIT_SUCCESS;
}