#include <settings/Bool.hpp>
#include "common.hpp"

namespace hacks::antitaunts
{
#if ENABLE_TEXTMODE
static settings::Boolean enable{ "remove.taunts", "true" };
#else
static settings::Boolean enable{ "remove.taunts", "false" };
#endif

void cm()
{
    if (!*enable)
        return;
    CachedEntity *ent;

    for (unsigned int i = 1; i <= g_IEngine->GetMaxClients(); i++)
    {
        ent = ENTITY(i);
        if (CE_BAD(ent) || ent->m_Type() != ENTITY_PLAYER)
            continue;

        RemoveCondition<TFCond_Taunting>(ent);
    }
}
static InitRoutine EC(
    []()
    {
        EC::Register(EC::CreateMove, cm, "antitaunts", EC::average);
        EC::Register(EC::CreateMoveWarp, cm, "antitaunts_w", EC::average);
    });
} // namespace hacks::antitaunts
