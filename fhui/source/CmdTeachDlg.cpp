#include "StdAfx.h"
#include "CmdTeachDlg.h"

namespace FHUI
{


void CmdTeachDlg::InitGroups()
{
    m_Groups = gcnew array<Group>(TECH_MAX);

    m_Groups[TECH_MI].En        = EnMI;
    m_Groups[TECH_MI].EnLimit   = EnLimitMI;
    m_Groups[TECH_MI].Limit     = LimitMI;

    m_Groups[TECH_MA].En        = EnMA;
    m_Groups[TECH_MA].EnLimit   = EnLimitMA;
    m_Groups[TECH_MA].Limit     = LimitMA;

    m_Groups[TECH_ML].En        = EnML;
    m_Groups[TECH_ML].EnLimit   = EnLimitML;
    m_Groups[TECH_ML].Limit     = LimitML;

    m_Groups[TECH_GV].En        = EnGV;
    m_Groups[TECH_GV].EnLimit   = EnLimitGV;
    m_Groups[TECH_GV].Limit     = LimitGV;

    m_Groups[TECH_LS].En        = EnLS;
    m_Groups[TECH_LS].EnLimit   = EnLimitLS;
    m_Groups[TECH_LS].Limit     = LimitLS;

    m_Groups[TECH_BI].En        = EnBI;
    m_Groups[TECH_BI].EnLimit   = EnLimitBI;
    m_Groups[TECH_BI].Limit     = LimitBI;

    for( int i = 0; i < TECH_MAX; ++i )
    {
        m_Groups[i].Limit->Maximum  = GameData::Player->TechLevelsAssumed[i];
        m_Groups[i].Limit->Value    = GameData::Player->TechLevelsAssumed[i];
    }
}

void CmdTeachDlg::AddCommand(CmdTeach ^cmd)
{
    m_Groups[cmd->m_Tech].En->Checked = true;
    if( cmd->m_Level > 0 )
    {
        m_Groups[cmd->m_Tech].EnLimit->Checked = true;
        m_Groups[cmd->m_Tech].Limit->Value =
            Math::Min( cmd->m_Level, GameData::Player->TechLevelsAssumed[cmd->m_Tech] );
    }
}

void CmdTeachDlg::UpdateEnables()
{
    for( int i = 0; i < TECH_MAX; ++i )
    {
        if( m_Groups[i].En->Checked )
        {
            m_Groups[i].EnLimit->Enabled = true;
            m_Groups[i].Limit->Enabled = m_Groups[i].EnLimit->Checked;
        }
        else
        {
            m_Groups[i].EnLimit->Enabled = false;
            m_Groups[i].Limit->Enabled = false;
        }
    }
}

int CmdTeachDlg::GetLevel(TechType tech)
{
    if( m_Groups[tech].En->Checked )
        return m_Groups[tech].EnLimit->Checked
            ? Decimal::ToInt32(m_Groups[tech].Limit->Value)
            : 0;
    return -1;
}

CmdTeach^ CmdTeachDlg::GetCommand(TechType tech)
{
    int level = GetLevel(tech);
    if( level >= 0 )
        return gcnew CmdTeach(m_Alien, tech, level);
    return nullptr;
}

} // end namespace FHUI
