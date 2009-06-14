#pragma once

#include "enums.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;

ref class StarSystem;

public ref class AtmosphericReq
{
public:
    AtmosphericReq()
        : m_ReqGas(GAS_MAX)
        , m_ReqMin(0)
        , m_Neutral(gcnew array<bool>(GAS_MAX) )
        , m_Poisonous(gcnew array<bool>(GAS_MAX) )
        , m_TempClass(-1)
        , m_PressClass(-1)
    {}

    bool IsValid() { return m_ReqGas != GAS_MAX && m_TempClass != -1 && m_PressClass != -1; }

    GasType             m_ReqGas;
    int                 m_ReqMin;
    int                 m_ReqMax;
    array<bool>        ^m_Neutral;
    array<bool>        ^m_Poisonous;

    int                 m_TempClass;
    int                 m_PressClass;
};

public ref class Alien
{
public:
    Alien(String ^name, int turn)
        : m_Name(name)
        , m_Relation(SP_NEUTRAL)
        , m_TurnMet(turn)
        , m_AtmReq(gcnew AtmosphericReq)
        , m_HomeSystem(nullptr)
        , m_HomePlanet(-1)
        , m_TechEstimateTurn(-1)
    {
        m_TechLevels        = gcnew array<int>(TECH_MAX){0};
        m_TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
    }

    String         ^m_Name;
    SPRelType       m_Relation;
    int             m_TurnMet;
    AtmosphericReq ^m_AtmReq;

    StarSystem     ^m_HomeSystem;
    int             m_HomePlanet;

    int             m_TechEstimateTurn;
    array<int>     ^m_TechLevels;
    array<int>     ^m_TechLevelsTeach;
};

public ref class Planet
{
public:
    Planet(int dia, float gv, int tc, int pc, float md)
        : m_Diameter(dia)
        , m_Grav(gv)
        , m_TempClass(tc)
        , m_PressClass(pc)
        , m_MiningDiff(md)
        , m_LSN(-1)
        , m_Atmosphere(gcnew array<int>(GAS_MAX))
        , m_Name(nullptr)
        , m_Comment(nullptr)
    {}

    int CalculateLSN(AtmosphericReq^, int tc, int pc);

    int         m_Diameter;
    float       m_Grav;
    int         m_TempClass;
    int         m_PressClass;
    float       m_MiningDiff;
    int         m_LSN;
    array<int> ^m_Atmosphere;

    String     ^m_Name;
    String     ^m_Comment;
};

public ref class StarSystem
{
public:
    StarSystem(int x, int y, int z, String ^type)
        : m_Planets(gcnew array<Planet^>(0))
        , m_TurnScanned(-1)
    {
        X = x;
        Y = y;
        Z = z;
        Type = type;
    }

    double CalcDistance(StarSystem ^s)                  { return CalcDistance(s->X, s->Y, s->Z); }
    double CalcMishap(StarSystem ^s, int gv, int age)   { return CalcMishap(s->X, s->Y, s->Z, gv, age); }
    double CalcDistance(int x, int y, int z);
    double CalcMishap(int x, int y, int z, int gv, int age);

    property int        X;
    property int        Y;
    property int        Z;
    property String^    Type;
    property String^    NumPlanets
    {
        String^ get()
        {
            if( m_TurnScanned == -1 )
                return "?";
            int minLSN = 99999;
            for each( Planet ^pl in m_Planets )
                minLSN = Math::Min(minLSN, pl->m_LSN == -1 ? 99999 : pl->m_LSN);
            return String::Format("{0} ({1})", m_Planets->Length, minLSN);
        }
    }
    property String^    ScanTurn
    {
        String^ get()
        {
            if( m_TurnScanned == -1 )       return "Not scanned";
            else if( m_TurnScanned == 0 )   return "Received";
            else                            return String::Format("Scaned, {0}", m_TurnScanned);
        }
    }

    array<Planet^>     ^m_Planets;
    int                 m_TurnScanned;
};

/*
public ref class PlanetView
{
public:
    PlanetView(int x, int y, int z, int plNum, Planet ^planet)
        : m_Planet(planet)
    {
        X = x;
        Y = y;
        Z = z;
        PlNum = plNum;
        TempClass = planet->m_TempClass;
        PressClass = planet->m_PressClass;
        LSN = planet->m_LSN;
        Comment = planet->m_Comment;
    }

    property int        X;
    property int        Y;
    property int        Z;
    property int        PlNum;
    property String^    Name;
    property int        TempClass;
    property int        PressClass;
    property String^    MiningDiff { String^ get() { return String::Format("{0:F2}", m_Planet->m_MiningDiff); } }
    property int        LSN;
    property String^    Distance
    {
        String^ get() { return String::Format("{0:F2}", m_Distance); }
    }
    property String^    Mishap
    {
        String^ get() { return String::Format("{0:F2}% ({1:F2}%)", m_Mishap, m_Mishap * m_Mishap / 100.0); }
    }
    property String^    Comment;

    Planet             ^m_Planet;
    double              m_Distance;
    double              m_Mishap;
};
*/

public ref class GameData
{
public:
    GameData();


    String^         GetSummary();

    Alien^          GetSpecies()                { return m_Species; }
    String^         GetSpeciesName()            { return m_Species->m_Name; }
    void            GetFleetCost(int%, float%);
    Alien^          GetAlien(String ^sp);
    SortedList^     GetAliens()                 { return m_Aliens; }
    StarSystem^     GetStarSystem(int x, int y, int z);
    array<StarSystem^>^ GetStarSystems()        { return m_Systems; }

    // ------------------------------------------
    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);
    void            SetTechLevel(int turn, Alien ^sp, TechType, int, int);
    void            SetFleetCost(int turn, int, float);
    Alien^          AddAlien(int turn, String ^sp);
    void            SetAlienRelation(int turn, String ^sp, SPRelType);
    void            AddStarSystem(int x, int y, int z, String ^type);
    void            AddPlanetScan(int turn, int x, int y, int z, int plNum, Planet ^planet);

    // ------------------------------------------
protected:
    int             TurnAlign(int turn);

    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetFleetSummary();
    String^         GetAliensSummary();

    // ------------------------------------------
    Alien              ^m_Species;
    int                 m_FleetCost;
    float               m_FleetCostPercent;
    SortedList         ^m_Aliens;
    array<StarSystem^> ^m_Systems;

    int                 m_TurnMax;
};
