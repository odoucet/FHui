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
    Teach
};

public interface class ICommand
{
    CommandPhase    GetPhase();
    CommandType     GetType();
    void            Print(List<String^>^);
};

////////////////////////////////////////////////////////////

public ref class CmdDisband : public ICommand
{
public:
    CmdDisband(String ^name)
        : m_Name(name)
    {}

    virtual CommandPhase    GetPhase()  { return CommandPhase::PreDeparture; }
    virtual CommandType     GetType()   { return CommandType::Disband; }
    virtual void    Print(List<String^> ^orders);

    String^         m_Name;
};

////////////////////////////////////////////////////////////

public ref class CmdPlanetName : public ICommand
{
public:
    CmdPlanetName(StarSystem ^system, int plNum, String ^name)
        : m_System(system)
        , m_PlanetNum(plNum)
        , m_Name(name)
    {}

    virtual CommandPhase    GetPhase()  { return CommandPhase::PreDeparture; }
    virtual CommandType     GetType()   { return CommandType::Name; }
    virtual void    Print(List<String^> ^orders);

    StarSystem^     m_System;
    int             m_PlanetNum;
    String^         m_Name;
};

////////////////////////////////////////////////////////////

public ref class CmdAlienRelation : public ICommand
{
public:
    CmdAlienRelation(Alien ^alien, SPRelType rel)
        : m_Alien(alien)
        , m_Relation(rel)
    {}

    virtual CommandPhase    GetPhase()  { return CommandPhase::Combat; }
    virtual CommandType     GetType()   { return CommandType::AlienRelation; }
    virtual void    Print(List<String^> ^orders);

    Alien^          m_Alien;
    SPRelType       m_Relation;
};

////////////////////////////////////////////////////////////

public ref class CmdTeach : public ICommand
{
public:
    CmdTeach(Alien ^alien, TechType tech, int level)
        : m_Alien(alien)
        , m_Tech(tech)
        , m_Level(level)
    {}

    virtual CommandPhase    GetPhase()  { return CommandPhase::PostArrival; }
    virtual CommandType     GetType()   { return CommandType::Teach; }
    virtual void    Print(List<String^> ^orders);

    Alien^          m_Alien;
    TechType        m_Tech;
    int             m_Level;
};

} // end namespace FHUI
