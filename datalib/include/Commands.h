#pragma once

#include "Enums.h"

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
    Teach,
    // Production commands:
    Hide,
    Shipyard,
    Research,
};

public interface class ICommand : public IComparable
{
    CommandPhase    GetPhase();
    CommandType     GetCmdType();

    // EU/CU/Inventory modifiers to budget / colony inventory
    // POSITIVE value mean:
    //  - EU SPENT (negative modifier to budget)
    //  + CU TRANSFERRED TO colony from transport ship or other colony
    //  + Inventory moved TRANSFERRED TO colony from transport ship or other colony
    int             GetEUCost();
    int             GetCUMod();
    int             GetInvMod(InventoryType);

    String^         Print();
};

////////////////////////////////////////////////////////////

template<CommandPhase Phase, CommandType CmdType>
public ref class CmdBase abstract : public ICommand
{
public:
    virtual CommandPhase    GetPhase()                  { return Phase; }
    virtual CommandType     GetCmdType()                { return CmdType; }
    virtual int             GetEUCost()                 { return 0; }
    virtual int             GetCUMod()                  { return 0; }
    virtual int             GetInvMod(InventoryType)    { return 0; }

    virtual String^         Print() abstract;

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
};

template<CommandType CmdType>
public ref class CmdProdBase abstract
    : public CmdBase<CommandPhase::Production, CmdType>
{
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
public ref class IProdCmdHide : public CmdProdBase<CommandType::Hide>
{
public:
    IProdCmdHide(Colony ^colony) : m_Colony(colony) {}

    virtual int     GetEUCost() override    { return Calculators::ColonyHideCost(m_Colony); }
    virtual String^ Print() override        { return "Hide"; } 

    Colony^     m_Colony;
};

////////////////////////////////////////////////////////////

// Shipyard
public ref class IProdCmdShipyard : public CmdProdBase<CommandType::Shipyard>
{
public:
    virtual int     GetEUCost() override    { return Calculators::ShipyardCost(GameData::Player->TechLevels[TECH_MA]); }
    virtual String^ Print() override        { return "Shipyard"; } 
};

////////////////////////////////////////////////////////////

// Research
public ref class IProdCmdResearch : public CmdProdBase<CommandType::Research>
{
public:
    IProdCmdResearch(TechType tech, int amount)
        : m_Tech(tech), m_Amount(amount) {}

    virtual int     GetEUCost() override    { return m_Amount; }
    virtual String^ Print() override        { return String::Format("Research {0} {1}",
                                                    m_Amount,
                                                    FHStrings::TechToString(m_Tech) ); }

    TechType    m_Tech;
    int         m_Amount;
};

////////////////////////////////////////////////////////////

} // end namespace FHUI
