#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>

namespace EngineCore
{
    using ListenerHandle = size_t;

    // Basic event, NOTE: not very safe, should be always deep copied when stored as member
    template <typename... Args>
    class Event
    {
    public:
        [[nodiscard]] ListenerHandle Subscribe(const std::function<void(Args...)>& Listener)
        {
            const auto id = m_NextListenerHandle++;
            m_Listeners[id] = std::make_shared<std::function<void(Args...)>>(Listener);
            return id;
        }

        void Unsubscribe(ListenerHandle Handle)
        {
            m_Listeners.erase(Handle);
           // printf("Unsubscribed, left listeners: %i \n", m_Listeners.size());
        }

        [[nodiscard]] std::shared_ptr<Event> Copy() const
        {
            return std::make_shared<Event>(*this);
        }

        ~Event()
        {
            //printf("Destroy event, left listeners: %i \n", m_Listeners.size());
        }

        void Invoke(Args... args)
        {
            std::vector<std::shared_ptr<std::function<void(Args...)>>> ListenersToInvoke;

            /* collect listeners to invoke (invocation is done in separate step to prevent exceptions
                when m_Listeners would be modified during invoke (eg. listener will unsubscribe on Invoke) */
            for (auto it = m_Listeners.begin(); it != m_Listeners.end();)
            {
                if (auto listener = it->second)
                {
                    ListenersToInvoke.push_back(listener);
                    ++it;
                }
                else
                {
                    // clear deprecated listeners
                    it = m_Listeners.erase(it);
                }
            }

            // invoke callbacks
            for (const auto& Listener : ListenersToInvoke)
            {
                if (Listener)
                {
                    (*Listener)(args...);
                }
            }
        }

    private:
        ListenerHandle m_NextListenerHandle = 0;
        std::unordered_map<ListenerHandle, std::shared_ptr<std::function<void(Args...)>>> m_Listeners;
    };
}
