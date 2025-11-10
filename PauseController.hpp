// PauseController.hpp
#pragma once
#include <mutex>
#include <condition_variable>

class PauseController {
public:
    void pause() {
        std::lock_guard<std::mutex> lk(m_);
        paused_ = true;
    }

    void resume() {
        {
            std::lock_guard<std::mutex> lk(m_);
            paused_ = false;
        }
        cv_.notify_all();
    }

    void toggle() {
        {
            std::lock_guard<std::mutex> lk(m_);
            paused_ = !paused_;
        }
        if (!is_paused()) cv_.notify_all();
    }

    bool is_paused() const {
        std::lock_guard<std::mutex> lk(m_);
        return paused_;
    }

    // Call this at the TOP of your simulation update loop.
    void wait_if_paused() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [this]{ return !paused_; });
    }

private:
    mutable std::mutex m_;
    std::condition_variable cv_;
    bool paused_{false};
};
