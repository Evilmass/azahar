// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "video_core/gpu_command_queue.h"

#include "core/frontend/emu_window.h"
#include "video_core/gpu.h"
#include "video_core/gpu_impl.h"

namespace VideoCore {

GPUCommandQueue::GPUCommandQueue(GPU& gpu_, std::unique_ptr<Frontend::GraphicsContext> context)
    : gpu{gpu_}, graphics_context{std::move(context)},
      worker_thread([this] { ProcessCommandQueue(); }) {
}

GPUCommandQueue::~GPUCommandQueue() {
    {
        std::lock_guard lock(queue_mutex);
        shutdown_requested = true;
    }
    queue_cv.notify_all();

    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

void GPUCommandQueue::QueueCommand(const Service::GSP::Command& command) {
    {
        std::lock_guard lock(queue_mutex);
        command_queue.push(command);
        is_idle = false;
    }
    queue_cv.notify_one();
}

void GPUCommandQueue::WaitForIdle() {
    std::unique_lock lock(queue_mutex);
    idle_cv.wait(lock, [this] { return is_idle && command_queue.empty(); });
}

void GPUCommandQueue::ProcessCommandQueue() {
    const auto process_queue = [this] {
        while (true) {
            Service::GSP::Command command{};

            {
                std::unique_lock lock(queue_mutex);
                queue_cv.wait(lock, [this] { return shutdown_requested || !command_queue.empty(); });

                if (shutdown_requested && command_queue.empty()) {
                    is_idle = true;
                    idle_cv.notify_all();
                    return;
                }

                command = command_queue.front();
                command_queue.pop();
            }

            {
                std::lock_guard rasterizer_lock(gpu.impl->rasterizer_mutex);
                gpu.ExecuteCommand(command);
            }

            {
                std::lock_guard lock(queue_mutex);
                if (command_queue.empty()) {
                    is_idle = true;
                    idle_cv.notify_all();
                }
            }
        }
    };

    if (graphics_context) {
        const auto scope = graphics_context->Acquire();
        process_queue();
        return;
    }

    process_queue();
}

} // namespace VideoCore
