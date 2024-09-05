#pragma once

#include "jni.hpp"

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>

struct ALooper;

namespace mbgl {
namespace util {

using WatchCallback = std::function<void(int, RunLoop::Event)>;

template <typename T> class Thread;
class Alarm;

class RunLoop::Impl {
public:
    class Runnable {
    public:
        virtual ~Runnable() = default;

        virtual void runTask() = 0;
        virtual TimePoint dueTime() const = 0;
    };

    Impl(RunLoop*, RunLoop::Type);
    ~Impl();

    void wake();

    void addRunnable(Runnable*);
    void removeRunnable(Runnable*);

    Milliseconds processRunnables();

    ALooper* loop = nullptr;
    RunLoop* runLoop = nullptr;
    std::atomic<bool> running;
    std::atomic_flag coalesce = ATOMIC_FLAG_INIT;

    std::unordered_map<int, WatchCallback> readPoll;

private:
    friend RunLoop;

    int fds[2];

    std::unique_ptr<Thread<Alarm>> alarm;

    std::mutex mutex;
    std::list<Runnable*> runnables;
};

class AndroidRunLoop : public RunLoop {
    void push(Priority, std::shared_ptr<WorkTask> task) override {
        std::shared_ptr<WorkTask>* wrapper = new std::shared_ptr<WorkTask>(task);
        // forward task to android looper
        // java_post(wrapper);
    }
};

} // namespace util
} // namespace mbgl

void execute_task(jlong wrapper) {
    std::shared_ptr<WorkTask>* t = (std::shared_ptr<WorkTask>*) wrapper;
    std::shared_ptr<WorkTask> task = *t;
    delete t;

    (*task)();
}
