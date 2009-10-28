#include "StdAfx.h"
#include "GameData.h"
#include "Commands.h"
#include "StarSystem.h"
#include "Ship.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////
// ShipCmdJump

String^ ShipCmdJump::Print()
{
    if( m_JumpTarget == nullptr )
        return String::Format("Jump {0}, 0 0 0 -1",
            m_Ship->PrintClassWithName() );

    return String::Format("Jump {0}, {1} {2} {3} {4}",
        m_Ship->PrintClassWithName(),
        m_JumpTarget->X,
        m_JumpTarget->Y,
        m_JumpTarget->Z,
        m_PlanetNum ); }

String^ ShipCmdJump::PrintForUI()
{
    return "Jump to " + m_Ship->PrintJumpDestination();
}

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

////////////////////////////////////////////////////////////////
// Develop

String^ ProdCmdDevelop::Print()
{
    if( m_Ship && m_Colony )
        return String::Format("Develop {0} PL {1}, {2}",
            m_Amount,
            m_Colony->Name,
            m_Ship->PrintClassWithName() );

    if( m_Colony )
        return String::Format("Develop {0} PL {1}",
            m_Amount,
            m_Colony->Name );

    return "Develop " + m_Amount.ToString();
}

////////////////////////////////////////////////////////////////
// Build CU/IU/AU
String^ ProdCmdBuildIUAU::Print()
{
    String ^target = "";
    if( m_Colony )
        target = " PL " + m_Colony->Name;
    else if( m_Ship )
        target = " " + m_Ship->PrintClassWithName();

    return String::Format("Build {0} {1}{2}",
        m_Amount,
        FHStrings::InvToString(m_Unit),
        target);
}

////////////////////////////////////////////////////////////////
// Ship upgrade
int ShipCmdUpgrade::GetEUCost()
{
    return Calculators::ShipUpgradeCost(m_Ship->Age, m_Ship->OriginalCost);
}

String^ ShipCmdUpgrade::Print()
{
    return "Upgrade " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Recycle ship
int ShipCmdRecycle::GetEUCost()
{
    return -m_Ship->GetRecycleValue();
}

String^ ShipCmdRecycle::Print()
{
    return "Recycle " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Unload
String^ ShipCmdUnload::Print()
{
    return "Unload " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Land
String^ ShipCmdLand::Print()
{
    return "Land " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Deep
String^ ShipCmdDeep::Print()
{
    return "Deep " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Orbit
String^ ShipCmdOrbit::Print()
{
    return String::Format("Orbit {0}, {1}", m_Ship->PrintClassWithName(), GetOrbitTarget());
}

String^ ShipCmdOrbit::GetOrbitTarget()
{
    return String::IsNullOrEmpty(m_Planet->Name)
        ? m_Planet->Number.ToString() : m_Planet->Name;
}


////////////////////////////////////////////////////////////////
// Scan
String^ ShipCmdScan::Print()
{
    return "Scan " + m_Ship->PrintClassWithName();
}

////////////////////////////////////////////////////////////////
// Wormhole
String^ ShipCmdWormhole::Print()
{
    return String::Format("Wormhole {0}, {1}",
        m_Ship->PrintClassWithName(),
        m_PlanetNum );
}

String^ ShipCmdWormhole::PrintForUI()
{
    return "Wormhole to " + m_Ship->PrintJumpDestination();
}

////////////////////////////////////////////////////////////
// Shipyard
int ProdCmdShipyard::GetEUCost()
{
    return Calculators::ShipyardCost(GameData::Player->TechLevels[TECH_MA]);
}

////////////////////////////////////////////////////////////
// Install
StarSystem^ CmdInstall::GetRefSystem()
{
    return m_Colony->System;
}

String^ CmdInstall::Print()
{
    return String::Format("Install {0} {1} PL {2}", m_Amount, m_Unit, m_Colony->Name);
}

////////////////////////////////////////////////////////////
// Message
String^ CmdMessage::Print()
{
    return String::Format("Message SP {0}\r\n{1}\r\nZzz", m_Alien->Name, m_Text);
}

////////////////////////////////////////////////////////////
// Visited
String^ CmdVisited::Print()
{
    return "Visited " + m_System->PrintLocation();
}

////////////////////////////////////////////////////////////
// Estimate
String^ ProdCmdEstimate::Print()
{
    return "Estimate SP " + m_Alien->Name;
}

} // end namespace FHUI
