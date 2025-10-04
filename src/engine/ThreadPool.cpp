#include "engine/ThreadPool.hpp"
Engine::ThreadPool::ThreadPool() {
  unsigned int threadCount = std::thread::hardware_concurrency();
  if (threadCount == 0) {
    threadCount = 2; // Fallback to 2 threads if hardware_concurrency can't detect
  }
  for (unsigned int i = 0; i < threadCount; ++i) {
    m_workers.emplace_back(&ThreadPool::threadLoop, this);
  }
}

void Engine::ThreadPool::enqueueTask(const std::function<void()> &task) {
  {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_tasks.push(task);
  }
  m_condition.notify_one();
}

void Engine::ThreadPool::threadLoop() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(m_queueMutex);
      m_condition.wait(lock, [this] {
        return m_shouldTerminate || !m_tasks.empty();
      });
      if (m_shouldTerminate) {
        return;
      }
      task = std::move(m_tasks.front());
      m_tasks.pop();
    }
    task();
  }
}

void Engine::ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_shouldTerminate = true;
  }
  m_condition.notify_all();
  for (std::thread &worker : m_workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
  m_workers.clear();
}
Engine::ThreadPool::~ThreadPool() {
  stop();
}
