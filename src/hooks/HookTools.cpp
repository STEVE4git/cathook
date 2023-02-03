#include "common.hpp"
#include "HookTools.hpp"

namespace EC
{

struct EventCallbackData
{
    static std::vector<EventCallbackData> events[ec_types::EcTypesSize];
    inline constexpr EventCallbackData(const ec_types type, const EventFunction &function, std::string_view name, enum ec_priority priority) : function{ function }, priority{ int(priority) }, event_name{ name }
    {
        events[type].emplace_back(*this);
        std::sort(events[type].begin(), events[type].end(), [](EventCallbackData &a, EventCallbackData &b) { return a.priority < b.priority; });
    }

    EventFunction function;
    int priority;
    std::string_view event_name;
};

CatCommand evt_print("debug_print_events", "Print EC events",
                     []()
                     {
                         for (int i = 0; i < int(ec_types::EcTypesSize); ++i)
                         {
                             logging::Info("%d events:", i);

                             for (auto it = EventCallbackData::events[i].begin(); it != EventCallbackData::events[i].end(); ++it)
                                 logging::Info("%s", it->event_name);
                             logging::Info("");
                         }
                     });

constexpr void Register(enum ec_types type, const EventFunction &function, const std::string_view &name, enum ec_priority priority)
{
    EventCallbackData(type, function, name, priority);
    // Order vector to always keep priorities correct
}

void Unregister(enum ec_types type, const std::string &name)
{
    auto &e = EventCallbackData::events[type];
    for (auto it = e.begin(); it != e.end(); ++it)
        if (it->event_name == name)
        {
            e.erase(it);
            break;
        }
}

inline void run(ec_types type)
{
    auto &vector = EventCallbackData::events[type];
    for (auto &i : vector)
    {
#if ENABLE_PROFILER
        volatile ProfilerNode node(i.section);
#endif
        i.function();
    }
}

} // namespace EC
