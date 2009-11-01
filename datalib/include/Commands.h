#pragma once

#include "Enums.h"
#include "FHStrings.h"
#include "Calculators.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

ref class StarSystem;
ref class Alien;
ref class Ship;

////////////////////////////////////////////////////////////

public enum class CommandPhase
{
    Combat,
    PreDeparture,
    Jump,
    Production,
    PostArrival,
    Strike,
    Custom
};

public enum class CommandType
{
    Disband,
    Name,
    Visited,
    AlienRelation,  // Enemy/Neutral/Ally
    Install,
    Teach,
    Message,
    Transfer,
    Custom,
    // Ship commands:
    Upgrade,
    RecycleShip,
    Jump,
    Wormhole,
    Move,
    Scan,
    Unload,
    Land,
    Deep,
    Orbit,
    // Production commands:
    Hide,
    Shipyard,
    Research,
    BuildIuAu,
    BuildShip,
    Recycle,
    Estimate,
    Develop,
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
    property bool           RequiresPhasePrefix;

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
        RequiresPhasePrefix = false;
    }

    virtual CommandPhase    GetPhase()                  { return Phase; }
    virtual CommandType     GetCmdType()                { return CmdType; }

    virtual property CommandOrigin  Origin;
    virtual property bool           UsageMarker;
    virtual property bool           RequiresPhasePrefix;

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

public ref class CmdPhaseWrapper
    : public CmdBase<CommandPhase::Custom, CommandType::Custom>
{
public:
    CmdPhaseWrapper(CommandPhase phase, ICommand ^command)
        : m_Phase(phase), m_Command(command)
    {
        RequiresPhasePrefix = true;
    }

    virtual CommandPhase    GetPhase() override                 { return m_Phase; }
    virtual CommandType     GetCmdType() override               { return m_Command->GetCmdType(); }

    virtual StarSystem^     GetRefSystem() override             { return m_Command->GetRefSystem(); }

    virtual int             GetEUCost() override                { return m_Command->GetEUCost(); }
    virtual int             GetPopCost() override               { return m_Command->GetPopCost(); }
    virtual int             GetInvMod(InventoryType i) override { return m_Command->GetInvMod(i); }

    virtual String^         Print() override                    { return m_Command->Print(); }
    virtual String^         PrintForUI() override               { return m_Command->PrintForUI(); }

    CommandPhase    m_Phase;
    ICommand^       m_Command;
};

////////////////////////////////////////////////////////////

public ref class CmdCustom
    : public CmdBase<CommandPhase::Custom, CommandType::Custom>
{
public:
    CmdCustom(CommandPhase phase, String ^command, int cost)
        : m_Phase(phase), m_Command(command) , m_EUCost(cost)
    {
        RequiresPhasePrefix = true;
    }

    CmdCustom^ operator = (CmdCustom ^cmd)
    {
        Origin      = cmd->Origin;
        m_Phase     = cmd->m_Phase;
        m_Command   = cmd->m_Command;
        m_EUCost    = cmd->m_EUCost;
        return this;
    }

    static String^          PhaseAsString(CommandPhase phase)
    {
        switch( phase )
        {
        case CommandPhase::Combat:          return "Combat";
        case CommandPhase::PreDeparture:    return "Pre-Departure";
        case CommandPhase::Jump:            return "Jumps";
        case CommandPhase::Production:      return "Production";
        case CommandPhase::PostArrival:     return "Post-Arrival";
        case CommandPhase::Strike:          return "Strikes";
        default:
            return "INVALID";
        }
    }

    virtual CommandPhase    GetPhase() override     { return m_Phase; }

    virtual int             GetEUCost() override    { return m_EUCost; }

    virtual String^         Print() override        { return m_Command; }

    CommandPhase    m_Phase;
    String^         m_Command;
    int             m_EUCost;
};

////////////////////////////////////////////////////////////

// Upgrade
public ref class ShipCmdUpgrade : public CmdProdBase<CommandType::Upgrade>
{
public:
    ShipCmdUpgrade(Ship ^ship) : m_Ship(ship) {}

    virtual int     GetEUCost() override;
    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Upgrade"; }

    Ship^   m_Ship;
};

// Recycle ship
public ref class ShipCmdRecycle : public CmdProdBase<CommandType::RecycleShip>
{
public:
    ShipCmdRecycle(Ship ^ship) : m_Ship(ship) {}

    virtual int     GetEUCost() override;
    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Recycle"; }

    Ship^   m_Ship;
};

// Unload
public ref class ShipCmdUnload : public CmdBase<CommandPhase::PreDeparture, CommandType::Unload>
{
public:
    ShipCmdUnload(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Unload"; }

    Ship^   m_Ship;
};

// Land
public ref class ShipCmdLand : public CmdBase<CommandPhase::PreDeparture, CommandType::Land>
{
public:
    ShipCmdLand(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Land"; }

    Ship^   m_Ship;
};

// Deep
public ref class ShipCmdDeep : public CmdBase<CommandPhase::PreDeparture, CommandType::Deep>
{
public:
    ShipCmdDeep(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Deep"; }

    Ship^   m_Ship;
};

// Orbit
public ref class ShipCmdOrbit : public CmdBase<CommandPhase::PreDeparture, CommandType::Orbit>
{
public:
    ShipCmdOrbit(Ship ^ship, Planet ^planet) : m_Ship(ship), m_Planet(planet) {}

    virtual String^ Print() override;
    virtual String^ PrintForUI() override   { return "Orbit " + GetOrbitTarget(); }

    virtual String^ GetOrbitTarget();

    Ship^   m_Ship;
    Planet^ m_Planet;
};

// Scan
public ref class ShipCmdScan : public CmdBase<CommandPhase::PostArrival, CommandType::Scan>
{
public:
    ShipCmdScan(Ship ^ship) : m_Ship(ship) {}

    virtual String^ Print() override;
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

    virtual StarSystem^ GetRefSystem() override { return m_JumpTarget; }

    virtual String^ Print() override;
    virtual String^ PrintForUI() override;

    Ship^       m_Ship;
    StarSystem^ m_JumpTarget;
    int         m_PlanetNum;
};

// Move
public ref class ShipCmdMove
    : public CmdBase<CommandPhase::Jump, CommandType::Move>
{
public:
    ShipCmdMove(Ship ^ship, int x, int y, int z)
        : m_Ship(ship), m_X(x), m_Y(y), m_Z(z)
    {}

    virtual StarSystem^ GetRefSystem() override;

    virtual String^ Print() override;
    virtual String^ PrintForUI() override;

    Ship^       m_Ship;
    int         m_X, m_Y, m_Z;
};

// Wormhole
public ref class ShipCmdWormhole
    : public CmdBase<CommandPhase::Jump, CommandType::Wormhole>
{
public:
    ShipCmdWormhole(Ship ^ship, StarSystem ^target, int planetNum)
        : m_Ship(ship), m_JumpTarget(target), m_PlanetNum(planetNum)
    {}

    virtual StarSystem^ GetRefSystem() override { return m_JumpTarget; }

    virtual String^ Print() override;
    virtual String^ PrintForUI() override;

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
    CmdInstall(int amount, InventoryType unit, Colony ^colony)
        : m_Amount(amount), m_Unit(unit), m_Colony(colony)
    {}

    virtual StarSystem^ GetRefSystem() override;

    virtual String^ Print() override;

    int             m_Amount;
    InventoryType   m_Unit;
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
    : public CmdBase<CommandPhase::PostArrival, CommandType::Message>
{
public:
    CmdMessage(Alien ^alien, String ^text)
        : m_Alien(alien)
        , m_Text(text)
    {}

    virtual String^ Print() override;

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

// Visited
public ref class CmdVisited
    : public CmdBase<CommandPhase::Jump, CommandType::Visited>
{
public:
    CmdVisited(StarSystem ^system)
        : m_System(system)
    {}

    virtual StarSystem^ GetRefSystem() override { return m_System; }

    virtual String^ Print() override;

    StarSystem^     m_System;
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

// Estimate
public ref class ProdCmdEstimate : public CmdProdBase<CommandType::Estimate>
{
public:
    ProdCmdEstimate(Alien ^alien) : m_Alien(alien) {}

    virtual int     GetEUCost() override    { return 25; }
    virtual String^ Print() override;

    Alien^      m_Alien;
};

////////////////////////////////////////////////////////////

// Shipyard
public ref class ProdCmdShipyard : public CmdProdBase<CommandType::Shipyard>
{
public:
    virtual int     GetEUCost() override;
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
    ProdCmdBuildIUAU(int amount, InventoryType unit, Colony ^colony, Ship ^ship)
        : m_Amount(amount), m_PopCost(0), m_Unit(unit), m_Colony(colony), m_Ship(ship)
    {
        if ( unit == INV_CU )
        {
            m_PopCost = amount;
        }
    }

    ProdCmdBuildIUAU^ operator = (ProdCmdBuildIUAU ^cmd)
    {
        Origin      = cmd->Origin;
        m_Amount    = cmd->m_Amount;
        m_PopCost   = cmd->m_PopCost;
        m_Unit      = cmd->m_Unit;
        m_Colony    = cmd->m_Colony;
        m_Ship      = cmd->m_Ship;
        return this;
    }

    virtual int     GetEUCost() override    { return m_Amount; }
    virtual int     GetPopCost() override   { return m_PopCost; }
    virtual int     GetInvMod(InventoryType i) override { return (i == m_Unit && m_Colony == nullptr && m_Ship == nullptr) ? m_Amount : 0; }

    virtual String^ Print() override;

    int             m_Amount;
    int             m_PopCost;
    InventoryType   m_Unit;
    Colony^         m_Colony;
    Ship^           m_Ship;
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

// Develop
public ref class ProdCmdDevelop : public CmdProdBase<CommandType::Develop>
{
public:
    ProdCmdDevelop(int amount, Colony ^colony, Ship ^ship)
        : m_Amount(amount), m_Colony(colony), m_Ship(ship) {}

    ProdCmdDevelop^ operator = (ProdCmdDevelop ^cmd)
    {
        Origin      = cmd->Origin;
        m_Amount    = cmd->m_Amount;
        m_Colony    = cmd->m_Colony;
        m_Ship      = cmd->m_Ship;
        return this;
    }

    virtual int     GetEUCost() override    { return m_Amount; }
    virtual int     GetPopCost() override   { return m_Ship ? m_Amount : ((m_Amount / 2) + (m_Amount % 1)); }

    virtual String^ Print() override;

    int         m_Amount;
    Colony^     m_Colony;
    Ship^       m_Ship;
};

////////////////////////////////////////////////////////////

// Transfer
public ref class CmdTransfer
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Transfer>
{
public:
    CmdTransfer(CommandPhase phase, InventoryType type, int amount, Colony ^from, Colony ^to)
    {
        Init(phase, type, amount, from, nullptr, to, nullptr);
    }

    CmdTransfer(CommandPhase phase, InventoryType type, int amount, Colony ^from, Ship ^to)
    {
        Init(phase, type, amount, from, nullptr, nullptr, to);
    }

    CmdTransfer(CommandPhase phase, InventoryType type, int amount, Ship ^from, Colony ^to)
    {
        Init(phase, type, amount, nullptr, from, to, nullptr);
    }

    CmdTransfer(CommandPhase phase, InventoryType type, int amount, Ship ^from, Ship ^to)
    {
        Init(phase, type, amount, nullptr, from, nullptr, to);
    }

    virtual CommandPhase    GetPhase() override     { return m_Phase; }

    virtual StarSystem^     GetRefSystem() override;

    virtual String^ Print() override;

    CommandPhase    m_Phase;
    InventoryType   m_Type;
    int             m_Amount;
    Colony^         m_FromColony;
    Ship^           m_FromShip;
    Colony^         m_ToColony;
    Ship^           m_ToShip;

protected:
    void    Init(CommandPhase phase, InventoryType type, int amount, Colony ^fromColony, Ship ^fromShip, Colony ^toColony, Ship ^toShip)
    {
        m_Phase = phase;
        m_Type = type;
        m_Amount = amount;
        m_FromColony = fromColony;
        m_FromShip = fromShip;
        m_ToColony = toColony;
        m_ToShip = toShip;
    }
};

////////////////////////////////////////////////////////////

} // end namespace FHUI
