#include "StdAfx.h"
#include "Commands.h"
#include "GameData.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// Name

void CmdPlanetName::Print(List<String^> ^orders)
{
    if( String::IsNullOrEmpty(m_Name) )
    {
        orders->Add(String::Format("  Disband {0} {1}",
            m_System->PrintLocation(),
            m_PlanetNum) );
    }
    else
    {
        orders->Add(String::Format("  Name {0} {1} PL {2}",
            m_System->PrintLocation(),
            m_PlanetNum,
            m_Name) );
    }
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
    orders->Add( String::Format("  Teach {0} {1} SP {2}",
        FHStrings::TechToString(m_Tech),
        m_Level,
        m_Alien->Name) );
}

} // end namespace FHUI
