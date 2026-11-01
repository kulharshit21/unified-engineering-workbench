#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <stdexcept>

namespace Workbench::Core::Parallel {

class ThreadPool {
public:
    explicit ThreadPool(size_t numWorkers = 4)
        : m_stop(false), m_activeTasks(0) {
        if (numWorkers == 0) {
            throw std::invalid_argument("ThreadPool requires at least 1 worker thread.");
        }
        for (size_t i = 0; i < numWorkers; ++i) {
            m_workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_cvQueue.wait(lock, [this]() {
                            return m_stop || !m_tasks.empty();
                        });

                        if (m_stop && m_tasks.empty()) {
                            return;
                        }

                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                        m_activeTasks++;
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_activeTasks--;
                        if (m_tasks.empty() && m_activeTasks == 0) {
                            m_cvDone.notify_all();
                        }
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
        }
        m_cvQueue.notify_all();
        for (auto &worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop) {
                throw std::runtime_error("Cannot enqueue on a stopped ThreadPool.");
            }
            m_tasks.push(std::move(task));
        }
        m_cvQueue.notify_one();
    }

    void waitAll() {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_cvDone.wait(lock, [this]() {
            return m_tasks.empty() && m_activeTasks == 0;
        });
    }

    size_t workerCount() const {
        return m_workers.size();
    }

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_cvQueue;
    std::condition_variable m_cvDone;
    std::atomic<bool> m_stop;
    size_t m_activeTasks;
};

} // namespace Workbench::Core::Parallel
