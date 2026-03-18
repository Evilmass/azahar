// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "core/hle/service/gsp/gsp_gpu.h"

namespace Frontend {
class GraphicsContext;
}

namespace VideoCore {

class GPU;

class GPUCommandQueue {
public:
    explicit GPUCommandQueue(GPU& gpu, std::unique_ptr<Frontend::GraphicsContext> context);
    ~GPUCommandQueue();

    void QueueCommand(const Service::GSP::Command& command);
    void WaitForIdle();

private:
    void ProcessCommandQueue();

    GPU& gpu;
    std::unique_ptr<Frontend::GraphicsContext> graphics_context;
    std::queue<Service::GSP::Command> command_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::condition_variable idle_cv;
    std::thread worker_thread;
    bool shutdown_requested{false};
    bool is_idle{true};
};

} // namespace VideoCore
