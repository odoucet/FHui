#pragma once

#include "Enums.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

////////////////////////////////////////////////////////////

private enum class CommandPhase
{
    Combat,
    PreDeparture,
    Jump,
    Production,
    PostArrival,
    Strike
};

private enum class CommandType
{
    Name,
    AlienRelation,  // Enemy/Neutral/Ally
    Teach
};

interface class ICommand
{
    CommandPhase    GetPhase();
    CommandType     GetType();
    void            Print(List<String^>^);
};

typedef List<ICommand^>     CommandListT;

////////////////////////////////////////////////////////////

private ref class CmdPlanetName : public ICommand
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

private ref class CmdAlienRelation : public ICommand
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

private ref class CmdTeach : public ICommand
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
