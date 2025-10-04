#pragma once

#include <functional>
#include <mutex>
#include <queue>
namespace Engine {
class ThreadPool {
public:
  ThreadPool();
  ~ThreadPool();
  static ThreadPool &getInstance() {
    static ThreadPool instance;
    return instance;
  }
  void stop();
  void enqueueTask(const std::function<void()>& task);
private:
  void threadLoop();
  bool m_shouldTerminate = false;
  std::vector<std::thread> m_workers;
  std::mutex m_queueMutex;
  std::condition_variable m_condition;
  std::queue<std::function<void()>> m_tasks;
};
}
