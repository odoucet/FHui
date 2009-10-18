#pragma once

#include "Enums.h"
#include "Calculators.h"
#include "GameData.h"
#include "StarSystem.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

ref class StarSystem;
ref class Alien;

////////////////////////////////////////////////////////////

public enum class CommandPhase
{
    Combat,
    PreDeparture,
    Jump,
    Production,
    PostArrival,
    Strike
};

public enum class CommandType
{
    Disband,
    Name,
    AlienRelation,  // Enemy/Neutral/Ally
    Install,
    Teach,
    Message,
    // Ship commands:
    Upgrade,
    RecycleShip,
    Jump,
    Wormhole,
    Scan,
    Unload,
    // Production commands:
    Hide,
    Shipyard,
    Research,
    BuildIuAu,
    BuildShip,
    Recycle,
};

public enum class CommandOrigin
{
    GUI,
    Auto,
    Plugin
};

public interface class ICommand : public IComparable
{
    CommandPhase    GetPhase();
    CommandType     GetCmdType();
    
    property CommandOrigin  Origin;
    property bool           UsageMarker;

    StarSystem^     GetRefSystem();

    // EU/CU/Inventory modifiers to budget / colony inventory
    // POSITIVE value mean:
    //  - EU SPENT (negative modifier to budget)
    //  - Population USED for production (negative modifier to budget)
    //  + Inventory moved TRANSFERRED TO colony from transport ship or other colony
    int             GetEUCost();
    int             GetPopCost();
    int             GetInvMod(InventoryType);

    String^         Print();
    String^         PrintForUI();
    String^         PrintOriginSuffix();
};

////////////////////////////////////////////////////////////

template<CommandPhase Phase, CommandType CmdType>
public ref class CmdBase abstract : public ICommand
{
public:
    CmdBase()
    {
        Origin = CommandOrigin::GUI;
        UsageMarker = false;
    }

    virtual CommandPhase    GetPhase()                  { return Phase; }
    virtual CommandType     GetCmdType()                { return CmdType; }

    virtual property CommandOrigin  Origin;
    virtual property bool           UsageMarker;

    virtual StarSystem^     GetRefSystem()              { return nullptr; }

    virtual int             GetEUCost()                 { return 0; }
    virtual int             GetPopCost()                { return 0; }
    virtual int             GetInvMod(InventoryType)    { return 0; }

    virtual String^         Print() abstract;
    virtual String^         PrintForUI()                { return Print(); }

    virtual int CompareTo(Object ^obj)
    {
        ICommand ^cmd = dynamic_cast<ICommand^>(obj);
        if( cmd == nullptr )
            return -1;

        int n = (int)GetPhase() - (int)cmd->GetPhase();
        if( n == 0 )
            n = (int)GetCmdType() - (int)cmd->GetCmdType();
        if( n == 0 )
        {
            String ^o1 = Print();
            String ^o2 = cmd->Print();
            n = o1->CompareTo(o2);
        }
        return n;
    }

    virtual String^ PrintOriginSuffix()
    {
        switch( Origin )
        {
        case CommandOrigin::Auto:       return " ; [Auto]"; 
        case CommandOrigin::Plugin:     return " ; [Plugin]";
        default:
            return "";
        }
    }
};

template<CommandType CmdType>
public ref class CmdProdBase abstract
    : public CmdBase<CommandPhase::Production, CmdType>
{
};

////////////////////////////////////////////////////////////

// Upgrade
public ref class ShipCmdUpgrade : public CmdProdBase<CommandType::Upgrade>
{
public:
    ShipCmdUpgrade(Ship ^ship) : m_Ship(ship) {}

    virtual int     GetEUCost() override    { return Calculators::ShipUpgradeCost(m_Ship->Age, m_Ship->OriginalCost); }
    virtual String^ Print() override        { return "Upgrade " + m_Ship->PrintClassWithName(); }
    virtual String^ PrintForUI() override   { return "Upgrade"; }

    Ship^   m_Ship;
};

// Recycle
public ref class ShipCmdRecycle : public CmdProdBase<CommandType::RecycleShip>
{
public:
    ShipCmdRecycle(Ship ^ship) : m_Ship(ship) {}

    virtual int     GetEUCost() override    { return -m_Ship->GetRecycleValue(); }
    virtual String^ Print() override        { return "Recycle " + m_Ship->PrintClassWithName(); }
    virtual String^ PrintForUI() override   { return "Recycle"; }

    Ship^   m_Ship;
};

// Unload
public ref class ShipCmdUnload : public CmdBase<CommandPhase::PreDeparture, CommandType::Unload>
{
public:
    ShipCmdUnload(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override        { return "Unload " + m_Ship->PrintClassWithName(); }
    virtual String^ PrintForUI() override   { return "Unload"; }

    Ship^   m_Ship;
};

// Scan
public ref class ShipCmdScan : public CmdBase<CommandPhase::PostArrival, CommandType::Scan>
{
public:
    ShipCmdScan(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override        { return "Scan " + m_Ship->PrintClassWithName(); }
    virtual String^ PrintForUI() override   { return "Scan"; }

    Ship^   m_Ship;
};

// Jump
public ref class ShipCmdJump
    : public CmdBase<CommandPhase::Jump, CommandType::Jump>
{
public:
    ShipCmdJump(Ship ^ship, StarSystem ^target, int planetNum)
        : m_Ship(ship), m_JumpTarget(target), m_PlanetNum(planetNum)
    {}

    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Jump to " + m_Ship->PrintJumpDestination(); }

    Ship^       m_Ship;
    StarSystem^ m_JumpTarget;
    int         m_PlanetNum;
};

// Wormhole
public ref class ShipCmdWormhole
    : public CmdBase<CommandPhase::Jump, CommandType::Wormhole>
{
public:
    ShipCmdWormhole(Ship ^ship, StarSystem ^target, int planetNum)
        : m_Ship(ship), m_JumpTarget(target), m_PlanetNum(planetNum)
    {}

    virtual String^ Print() override        { return String::Format("Wormhole {0}, {1}",
                                                        m_Ship->PrintClassWithName(),
                                                        m_PlanetNum ); }
    virtual String^ PrintForUI() override   { return "Wormhole to " + m_Ship->PrintJumpDestination(); }

    Ship^       m_Ship;
    StarSystem^ m_JumpTarget;
    int         m_PlanetNum;
};

////////////////////////////////////////////////////////////

// Disband
public ref class CmdDisband
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Disband>
{
public:
    CmdDisband(String ^name) : m_Name(name) {}

    virtual String^ Print() override { return "Disband PL " + m_Name; }

    String^         m_Name;
};

////////////////////////////////////////////////////////////

// Name
public ref class CmdPlanetName
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Name>
{
public:
    CmdPlanetName(StarSystem ^system, int plNum, String ^name)
        : m_System(system)
        , m_PlanetNum(plNum)
        , m_Name(name)
    {}

    virtual String^ Print() override;

    StarSystem^     m_System;
    int             m_PlanetNum;
    String^         m_Name;
};

////////////////////////////////////////////////////////////

// Install
public ref class CmdInstall
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Install>
{
public:
    CmdInstall(int amount, String ^unit, Colony ^colony)
        : m_Amount(amount), m_Unit(unit), m_Colony(colony)
    {}

    virtual StarSystem^ GetRefSystem() override { return m_Colony->System; }

    virtual String^ Print() override    { return String::Format("Install {0} {1}, PL {2}", m_Amount, m_Unit, m_Colony->Name); }

    int             m_Amount;
    String^         m_Unit;
    Colony^         m_Colony;
};

////////////////////////////////////////////////////////////

// Ally / Neutral / Enemy
public ref class CmdAlienRelation
    : public CmdBase<CommandPhase::PreDeparture, CommandType::AlienRelation>
{
public:
    CmdAlienRelation(Alien ^alien, SPRelType rel)
        : m_Alien(alien)
        , m_Relation(rel)
    {}

    virtual String^ Print() override;

    Alien^          m_Alien;
    SPRelType       m_Relation;
};

////////////////////////////////////////////////////////////

// Message
public ref class CmdMessage
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Message>
{
public:
    CmdMessage(Alien ^alien, String ^text)
        : m_Alien(alien)
        , m_Text(text)
    {}

    virtual String^ Print() override { return String::Format("Message SP {0}\r\n{1}\r\nZzz", m_Alien->Name, m_Text); }

    Alien^          m_Alien;
    String^         m_Text;
};

// Teach
public ref class CmdTeach
    : public CmdBase<CommandPhase::PostArrival, CommandType::Teach>
{
public:
    CmdTeach(Alien ^alien, TechType tech, int level)
        : m_Alien(alien)
        , m_Tech(tech)
        , m_Level(level)
    {}

    virtual String^ Print() override;

    Alien^          m_Alien;
    TechType        m_Tech;
    int             m_Level;
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Hide
public ref class ProdCmdHide : public CmdProdBase<CommandType::Hide>
{
public:
    ProdCmdHide(Colony ^colony) : m_Colony(colony) {}

    virtual int     GetEUCost() override    { return Calculators::ColonyHideCost(m_Colony); }
    virtual String^ Print() override        { return "Hide"; } 

    Colony^     m_Colony;
};

////////////////////////////////////////////////////////////

// Shipyard
public ref class ProdCmdShipyard : public CmdProdBase<CommandType::Shipyard>
{
public:
    virtual int     GetEUCost() override    { return Calculators::ShipyardCost(GameData::Player->TechLevels[TECH_MA]); }
    virtual String^ Print() override        { return "Shipyard"; } 
};

////////////////////////////////////////////////////////////

// Research
public ref class ProdCmdResearch : public CmdProdBase<CommandType::Research>
{
public:
    ProdCmdResearch(TechType tech, int amount)
        : m_Tech(tech), m_Amount(amount) {}

    virtual int     GetEUCost() override    { return m_Amount; }
    virtual String^ Print() override        { return String::Format("Research {0} {1}",
                                                    m_Amount,
                                                    FHStrings::TechToString(m_Tech) ); }

    TechType    m_Tech;
    int         m_Amount;
};

////////////////////////////////////////////////////////////

// Build CU/IU/AU
public ref class ProdCmdBuildIUAU : public CmdProdBase<CommandType::BuildIuAu>
{
public:
    ProdCmdBuildIUAU(int amount, InventoryType unit)
        : m_Amount(amount), m_PopCost(0), m_Unit(unit)
    {
        if ( unit == INV_CU )
        {
            m_PopCost = amount;
        }
    }

    virtual int     GetEUCost() override    { return m_Amount; }
    virtual int     GetPopCost() override   { return m_PopCost; }
    virtual int     GetInvMod(InventoryType i) override { return i == m_Unit ? m_Amount : 0; }

    virtual String^ Print() override        { return String::Format("Build {0} {1}", m_Amount, FHStrings::InvToString(m_Unit)); }

    int             m_Amount;
    int             m_PopCost;
    InventoryType   m_Unit;
};

////////////////////////////////////////////////////////////

// Build Ship
public ref class ProdCmdBuildShip : public CmdProdBase<CommandType::BuildShip>
{
public:
    ProdCmdBuildShip(ShipType type, int size, bool sublight, String ^name)
        : m_Type(type), m_Size(size), m_Sublight(sublight), m_Name(name) {}

    virtual int     GetEUCost() override    { return Calculators::ShipBuildCost(m_Type, m_Size, m_Sublight); }
    virtual String^ Print() override        { return String::Format("Build {0}{1}{2} {3}",
                                                    FHStrings::ShipToString(m_Type),
                                                    m_Type == SHIP_TR ? m_Size.ToString() : "",
                                                    m_Sublight ? "S" : "",
                                                    m_Name ); }

    ShipType    m_Type;
    int         m_Size;
    bool        m_Sublight;
    String^     m_Name;
};

////////////////////////////////////////////////////////////

// Recycle
public ref class ProdCmdRecycle : public CmdProdBase<CommandType::Recycle>
{
public:
    ProdCmdRecycle(InventoryType type, int amount)
        : m_Type(type), m_Amount(amount) {}

    virtual int     GetEUCost() override    { return -Calculators::RecycleValue(m_Type, m_Amount); }
    virtual int     GetPopCost() override   { return -Calculators::RecycleValuePop(m_Type, m_Amount); }
    virtual int     GetInvMod(InventoryType i) override { return i == m_Type ? -m_Amount : 0; }

    virtual String^ Print() override        { return String::Format("Recycle {0} {1}",
                                                    m_Amount,
                                                    FHStrings::InvToString(m_Type) ); }

    InventoryType   m_Type;
    int             m_Amount;
};

////////////////////////////////////////////////////////////

} // end namespace FHUI
