/*
 * OutOfFOVArrows.cpp
 *
 *  Created on: Sep 9, 2021
 *      Author: user
 */

// UNFINISHED

#include <settings/Bool.hpp>
#include "common.hpp"
#include "PlayerTools.hpp"

namespace hacks::outoffovarrows
{
static settings::Boolean enable{ "oof-arrows.enable", "false" };
static settings::Float angle{ "oof-arrows.angle", "5" };
static settings::Float length{ "oof-arrows.length", "5" };
static settings::Float min_dist{ "oof-arrows.min-dist", "0" };
static settings::Float max_dist{ "oof-arrows.max-dist", "0" };

bool warning_triggered  = false;
bool backstab_triggered = false;
float last_say          = 0.0f;
Timer lastVoicemenu{};

bool ShouldRun(CBaseEntity *pLocal)
{
    if (!enable || g_IEngineVGui->IsGameUIVisible())
        return false;

    if (g_pLocalPlayer->life_state)
        return false;

    return true;
}

void Draw(const Vector &vecFromPos, const Vector &vecToPos, rgba_t color)
{
    color.a                = 150;
    auto GetClockwiseAngle = [&](const Vector &vecViewAngle, const Vector &vecAimAngle) -> float
    {
        Vector vecAngle = Vector();
        AngleVectors2(VectorToQAngle(vecViewAngle), &vecAngle);

        Vector vecAim = Vector();
        AngleVectors2(VectorToQAngle(vecAimAngle), &vecAim);

        return -atan2(vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y);
    };

    auto MapFloat = [&](float x, float in_min, float in_max, float out_min, float out_max) -> float { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; };

    Vector vecAngleTo = CalcAngle(vecFromPos, vecToPos);
    g_IEngine->GetViewAngles(vecViewAngle);

    const float deg  = GetClockwiseAngle(vecViewAngle, vecAngleTo);
    const float xrot = cos(deg - PI / 2);
    const float yrot = sin(deg - PI / 2);

    int ScreenWidth, ScreenHeight;
    g_IEngine->GetScreenSize(ScreenWidth, ScreenHeight);

    const float x1 = ((ScreenWidth * 0.15) + 5.0f) * xrot;
    const float y1 = ((ScreenWidth * 0.15) + 5.0f) * yrot;
    const float x2 = ((ScreenWidth * 0.15) + 15.0f) * xrot;
    const float y2 = ((ScreenWidth * 0.15) + 15.0f) * yrot;

    float arrow_angle  = DEG2RAD(angle);
    float arrow_length = *length;

    const Vector line{ x2 - x1, y2 - y1, 0.0f };
    const float length = line.Length();

    const float fpoint_on_line = arrow_length / (atanf(arrow_angle) * length);
    const Vector point_on_line = Vector(x2, y2, 0) + (line * fpoint_on_line * -1.0f);
    const Vector normal_vector{ -line.y, line.x, 0.0f };
    const Vector normal = Vector(arrow_length, arrow_length, 0.0f) / (length * 2);

    const Vector rotation = normal * normal_vector;
    const Vector left     = point_on_line + rotation;
    const Vector right    = point_on_line - rotation;

    float cx = static_cast<float>(ScreenWidth / 2);
    float cy = static_cast<float>(ScreenHeight / 2);

    float fMap       = clamp(MapFloat(vecFromPos.DistTo(vecToPos), max_dist, min_dist, 0.0f, 1.0f), 0.0f, 1.0f);
    rgba_t HeatColor = color;
    HeatColor.a      = static_cast<byte>(fMap * 255.0f);

    g_Draw.Line(cx + x2, cy + y2, cx + left.x, cy + left.y, HeatColor);
    g_Draw.Line(cx + x2, cy + y2, cx + right.x, cy + right.y, HeatColor);
    g_Draw.Line(cx + left.x, cy + left.y, cx + right.x, cy + right.y, HeatColor);
}

#if ENABLE_VISUALS
static InitRoutine EC(
    []()
    {
        EC::Register(EC::Draw, Draw, "outoffovarrows", EC::average);
    });
#endif
} // namespace hacks::outoffovarrows

void CPlayerArrows::Run()
{

    if (const auto &pLocal = g_EntityCache.m_pLocal)
    {
        if (!ShouldRun(pLocal))
            return;

        Vector vLocalPos = pLocal->GetWorldSpaceCenter();

        m_vecPlayers.clear();

        for (const auto &pEnemy : g_EntityCache.GetGroup(EGroupType::PLAYERS_ENEMIES))
        {
            if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsCloaked() || pEnemy->IsAGhost())
                continue;

            if (Vars::Visuals::SpyWarningIgnoreFriends.m_Var && g_EntityCache.Friends[pEnemy->GetIndex()])
                continue;

            Vector vEnemyPos = pEnemy->GetWorldSpaceCenter();

            Vector vAngleToEnemy = CalcAngle(vLocalPos, vEnemyPos);
            Vector viewangless   = g_Interfaces.Engine->GetViewAngles();
            viewangless.x        = 0;
            float fFovToEnemy    = CalcFov(viewangless, vAngleToEnemy);

            if (fFovToEnemy < Vars::Visuals::FieldOfView.m_Var)
                continue;

            m_vecPlayers.push_back(vEnemyPos);
        }
        if (m_vecPlayers.empty())
            return;

        for (const auto &Player : m_vecPlayers)
        {
            rgba_t teamColor;
            if (!Vars::ESP::Main::EnableTeamEnemyColors.m_Var)
            {
                if (pLocal->GetTeamNum() == 2)
                {
                    teamColor = Colors::TeamBlu;
                }
                else
                {
                    teamColor = Colors::TeamRed;
                }
            }
            else
            {
                teamColor = Colors::Enemy;
            }
            DrawArrowTo(vLocalPos, Player, teamColor);
        }
    }
}
