#include "StdAfx.h"
#include "Commands.h"
#include "GameData.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// Disband

void CmdDisband::Print(List<String^> ^orders)
{
    orders->Add("  Disband PL " + m_Name);
}

////////////////////////////////////////////////////////////////
// Name

void CmdPlanetName::Print(List<String^> ^orders)
{
    orders->Add(String::Format("  Name {0} {1} PL {2}",
        m_System->PrintLocation(),
        m_PlanetNum,
        m_Name) );
}

////////////////////////////////////////////////////////////////
// Relations

void CmdAlienRelation::Print(List<String^> ^orders)
{
    switch( m_Relation )
    {
    case SP_NEUTRAL:
        orders->Add("  Neutral SP " + m_Alien->Name);
        break;

    case SP_ENEMY:
        orders->Add("  Enemy SP " + m_Alien->Name);
        break;

    case SP_ALLY:
        orders->Add("  Ally SP " + m_Alien->Name);
        break;
    }
}

////////////////////////////////////////////////////////////////
// Relations

void CmdTeach::Print(List<String^> ^orders)
{
    if( m_Alien->Relation == SP_ALLY ||
        m_Alien->Relation == SP_NEUTRAL )
    {
        orders->Add( String::Format("  Teach {0} {1} SP {2}",
            FHStrings::TechToString(m_Tech),
            m_Level,
            m_Alien->Name) );
    }
}

} // end namespace FHUI
