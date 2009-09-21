#include "StdAfx.h"
#include "GameData.h"
#include "Commands.h"
#include "FHStrings.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// Name

String^ CmdPlanetName::Print()
{
    return String::Format("Name {0} {1} PL {2}",
        m_System->PrintLocation(),
        m_PlanetNum,
        m_Name );
}

////////////////////////////////////////////////////////////////
// Relations

String^ CmdAlienRelation::Print()
{
    switch( m_Relation )
    {
    case SP_NEUTRAL:
        return "Neutral SP " + m_Alien->Name;

    case SP_ENEMY:
        return "Enemy SP " + m_Alien->Name;

    case SP_ALLY:
        return "Ally SP " + m_Alien->Name;

    default:
        throw gcnew FHUIDataIntegrityException("Invalid alien relation command: " + ((int)m_Relation).ToString() );
    }
}

////////////////////////////////////////////////////////////////
// Relations

String^ CmdTeach::Print()
{
    return String::Format("Teach {0} {1} SP {2}",
        FHStrings::TechToString(m_Tech),
        m_Level,
        m_Alien->Name );
}

} // end namespace FHUI
