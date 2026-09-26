#pragma once

#include "Event.h"

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <utility>

class EventBus
{
public:

    EventBus() = default;
    ~EventBus() = default;

    // --------------------------------------------------------
    // Post an event.
    //
    // This is thread-safe.
    //
    // Any thread can call this.
    // --------------------------------------------------------

    template<typename T, typename... Args>
    void Post(Args&&... args)
    {
        auto event =
            std::make_unique<T>(
                std::forward<Args>(args)...
            );

        {
            std::lock_guard lock(m_EventMutex);

            m_EventQueue.push(std::move(event));
        }
    }


    // --------------------------------------------------------
    // Subscribe to an event type.
    //
    // Currently intended to be called from the main thread.
    // --------------------------------------------------------

    template<typename T, typename F>
    void Subscribe(F&& callback)
    {
        Callback wrapper =
            [callback = std::forward<F>(callback)]
            (const Event& event)
            {
                callback(
                    static_cast<const T&>(event)
                );
            };

        m_Subscribers[typeid(T)].push_back(
            std::move(wrapper)
        );
    }


    // --------------------------------------------------------
    // Dispatch all queued events.
    //
    // This should be called by the thread that owns the
    // subscribers -- normally your main/game thread.
    // --------------------------------------------------------

    void Dispatch();


private:

    using EventPtr = std::unique_ptr<Event>;

    using Callback =
        std::function<void(const Event&)>;


    // --------------------------------------------------------
    // Event Queue
    //
    // Protected because multiple threads can Post().
    // --------------------------------------------------------

    std::mutex m_EventMutex;

    std::queue<EventPtr> m_EventQueue;


    // --------------------------------------------------------
    // Subscribers
    //
    // These are only accessed by the dispatching thread.
    // --------------------------------------------------------

    std::unordered_map<
        std::type_index,
        std::vector<Callback>
    > m_Subscribers;
};