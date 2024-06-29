#pragma once

#include <algorithm>
#include <atomic>
#include <bits/stdc++.h>
#include <functional>

#include "LogMutex.h"

/*
    TaskQueue
    Thread-safe task queue.
    Single-producer single-consumer model.
*/
class TaskQueue
{
public:
    /* Task function type */
    typedef std::function<void(void* context, void* params, float delatTime)> TaskFunction;

    /*
        Task
        A task is a wrapper struct for a callback function which will perform the logic in the consumer thread.
    */
    struct Task
    {
        Task():
            m_fn(nullptr),
            m_context(nullptr)
        {

        }

        /* Execute the task */
        void execute(float deltaTime)
        {
            m_fn(m_context, m_params, deltaTime);
        }

        bool isValid() const
        {
            if(m_fn)
            {
                return true;
            }
            return false;
        }

        TaskFunction m_fn; // the callback function
        char m_params[128]; // allocated memory for the task parameters - bytes
        void* m_context; // the context where the task has been created
    };

    TaskQueue(int maxNumTasks):
        m_tasks(nullptr),
        m_maxNumTasks(maxNumTasks),
        m_head(0),
        m_tail(0),
        m_numTasks(0)

    {
        /* Allocating memory for our tasks */
        m_tasks = new Task[m_maxNumTasks];
    }

    ~TaskQueue()
    {
        delete[] m_tasks;
    }

    // Deleting other special member functions as they may cause shallow copies
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;
    TaskQueue(TaskQueue&& other) = delete;
    TaskQueue& operator=(TaskQueue&& other) = delete;
    

    bool push(TaskFunction fn, void* context, const void* params = nullptr, size_t paramsSize = 0)
    {
        // Pushing task to our queue
        if(m_numTasks.load() == m_maxNumTasks)
        {
            LM_ERROR("TaskQueue error: could not push new task. Maximum number of tasks reached: %i\n", m_maxNumTasks);
            return false;
        }

        // Creating a new task
        Task task;
        task.m_fn = fn;
        task.m_context = context;

        // Creating a copy of the task params as the task will be passed onto another thread
        if(params)
        {
            if(paramsSize > 0)
            {
                if(paramsSize <= sizeof(task.m_params))
                {
                    memcpy(task.m_params, params, std::min(paramsSize, sizeof(task.m_params)));
                }
                else
                {
                    LM_ERROR("TaskQueue error: could not copy task params. Allocated size for task params not sufficient: required %i actual %i\n", paramsSize, sizeof(task.m_params));
                    return false;
                }
            }
        }

        m_tasks[m_head] = task;
        m_head = (m_head + 1) % m_maxNumTasks;
        m_numTasks.fetch_add(1);
        return true;
    }

    bool pop(Task& outTask)
    {
        if(m_numTasks.load() != 0)
        {
            outTask = m_tasks[m_tail];
            m_tail = (m_tail + 1) % m_maxNumTasks;
            m_numTasks.fetch_sub(1);
            return true;
        }

        return false;
    }

    /* Thread safe */
    int getNumTasks()
    {
        return m_numTasks.load();
    }
    
private:
    Task* m_tasks;
    int m_maxNumTasks;
    int m_head;
    int m_tail;
    std::atomic<int> m_numTasks;
};