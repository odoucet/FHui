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
    Production      // All production commands
};

public interface class ICommand : public IComparable
{
    CommandPhase    GetPhase();
    CommandType     GetType();

    String^         Print();
};

public interface class ICommandProd : public ICommand
{
    int             GetEUCost();
    int             GetCUCost();
};

////////////////////////////////////////////////////////////

template<CommandPhase Phase, CommandType Type>
public ref class CmdBase abstract : public ICommand
{
public:
    virtual CommandPhase    GetPhase()  { return Phase; }
    virtual CommandType     GetType()   { return Type; }

    virtual String^         Print() abstract;

    virtual int CompareTo(Object ^obj)
    {
        ICommand ^cmd = dynamic_cast<ICommand^>(obj);
        if( cmd == nullptr )
            return -1;

        int n = (int)GetPhase() - (int)cmd->GetPhase();
        if( n == 0 )
            n = (int)GetType() - (int)cmd->GetType();
        if( n == 0 )
        {
            String ^o1 = Print();
            String ^o2 = cmd->Print();
            n = o1->CompareTo(o2);
        }
        return n;
    }
};

public ref class CmdProdBase abstract
    : public ICommandProd
    , public CmdBase<CommandPhase::Production, CommandType::Production>
{
public:
    virtual int             GetEUCost()         { return 0; }
    virtual int             GetCUCost()         { return 0; }
};

////////////////////////////////////////////////////////////

public ref class CmdDisband
    : public CmdBase<CommandPhase::PreDeparture, CommandType::Disband>
{
public:
    CmdDisband(String ^name) : m_Name(name) {}

    virtual String^ Print() override { return "Disband PL " + m_Name; }

    String^         m_Name;
};

////////////////////////////////////////////////////////////

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

public ref class IProdCmdShipyard : public CmdProdBase
{
public:
    virtual int     GetEUCost() override    { return Calculators::ShipyardCost(GameData::Player->TechLevels[TECH_MA]); }
    virtual String^ Print() override        { return "Shipyard"; } 
};

////////////////////////////////////////////////////////////

} // end namespace FHUI
