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

    String^         PrintRelation() { return SpRelToString(m_Relation); }
    String^         PrintHome();

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
    Planet(int nr, int dia, float gv, int tc, int pc, float md)
        : m_Number(nr)
        , m_Diameter(dia)
        , m_Grav(gv)
        , m_TempClass(tc)
        , m_PressClass(pc)
        , m_MiningDiff(md)
        , m_LSN(-1)
        , m_Atmosphere(gcnew array<int>(GAS_MAX))
        , m_Name(nullptr)
        , m_Comment(nullptr)
    {}

    int         CalculateLSN(AtmosphericReq^);

    int         m_Number;
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

    double      CalcDistance(StarSystem ^s)                  { return CalcDistance(s->X, s->Y, s->Z); }
    double      CalcMishap(StarSystem ^s, int gv, int age)   { return CalcMishap(s->X, s->Y, s->Z, gv, age); }
    double      CalcDistance(int x, int y, int z);
    double      CalcMishap(int x, int y, int z, int gv, int age);

    String^     GenerateScan();
    String^     PrintLocation() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^     PrintScanTurn();

    property int        X;
    property int        Y;
    property int        Z;
    property String^    Type;

    array<Planet^>     ^m_Planets;
    int                 m_TurnScanned;
};

public ref class Colony
{
public:
    Colony(Alien ^owner, String ^name, StarSystem ^system, int planetNum)
        : m_Owner(owner)
        , m_Name(name)
        , m_PlanetType(PLANET_HOME)
        , m_System(system)
        , m_Planet(nullptr)
        , m_PlanetNum(planetNum)
        , m_AvailPop(0)
        , m_EconomicEff(0)
        , m_ProdPenalty(0)
        , m_EUProd(0)
        , m_EUFleet(0)
        , m_MiBase(0)
        , m_MiDiff(0)
        , m_MaBase(0)
        , m_RMCarried(0)
        , m_Shipyards(0)
    {
        m_Inventory = gcnew array<int>(INV_MAX){0};
    }

    String^     PrintLocation() { return String::Format("{0} {1}", m_System->PrintLocation(), m_PlanetNum); }
    String^     PrintInventoryShort();

    Alien          ^m_Owner;
    String         ^m_Name;
    PlanetType      m_PlanetType;
    StarSystem     ^m_System;
    Planet         ^m_Planet;
    int             m_PlanetNum;
    int             m_AvailPop;
    int             m_EconomicEff;
    int             m_ProdPenalty;
    int             m_EUProd;
    int             m_EUFleet;
    double          m_MiBase;
    double          m_MiDiff;
    double          m_MaBase;
    int             m_RMCarried;
    int             m_Shipyards;
    array<int>     ^m_Inventory;
};

public ref class PlanetName
{
public:
    PlanetName(int x, int y, int z, int num, String ^name)
    {
        X = x;
        Y = y;
        Z = z;
        Num = num;
        Name = name;
    }

    property int X;
    property int Y;
    property int Z;
    property int Num;
    property String^ Name;
};

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
    SortedList^     GetColonies()               { return m_Colonies; }
    SortedList^     GetPlanetNames()            { return m_PlanetNames; }

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
    void            SetTurnStartEU(int turn, int eu);
    void            AddTurnProducedEU(int turn, int eu);
    Colony^         AddColony(int turn, Alien^, String^, StarSystem^, int);
    void            AddPlanetName(int turn, int x, int y, int z, int pl, String ^name);
    void            UpdatePlanets();

    // ------------------------------------------
protected:
    bool            TurnCheck(int turn);
    int             TurnAlign(int turn);

    void            CalculateLSN();
    void            LinkPlanetNames();

    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetEconomicSummary();
    String^         GetAliensSummary();
    String^         GetPlanetsSummary();
    String^         GetShipsSummary();

    // ------------------------------------------
    Alien              ^m_Species;
    int                 m_TurnEUStart;
    int                 m_TurnEUProduced;
    int                 m_FleetCost;
    float               m_FleetCostPercent;
    SortedList         ^m_Aliens;
    array<StarSystem^> ^m_Systems;
    SortedList         ^m_Colonies;
    SortedList         ^m_PlanetNames;

    int                 m_TurnMax;
};
