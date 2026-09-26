#include "EventBus.h"

void EventBus::Dispatch()
{
    // --------------------------------------------------------
    // Take ownership of the current queue.
    //
    // We don't want to hold the mutex while executing
    // callbacks.
    // --------------------------------------------------------

    std::queue<EventPtr> events;

    {
        std::lock_guard lock(m_EventMutex);

        std::swap(
            events,
            m_EventQueue
        );
    }


    // --------------------------------------------------------
    // Process events without holding the mutex.
    // --------------------------------------------------------

    while (!events.empty())
    {
        EventPtr& event = events.front();

        auto iterator =
            m_Subscribers.find(
                event->GetType()
            );


        // No subscribers for this event.
        if (iterator != m_Subscribers.end())
        {
            for (auto& callback : iterator->second)
            {
                callback(*event);
            }
        }


        events.pop();
    }
}