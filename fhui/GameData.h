#pragma once

#include "enums.h"

using namespace System;
using namespace System::Collections::Generic;

ref class StarSystem;
ref class Ship;
ref class Colony;

public ref class AtmosphericReq
{
public:
    AtmosphericReq()
    {
        GasRequired = GAS_MAX;
        ReqMin = 0;
        ReqMax = 0;
        TempClass = -1;
        PressClass = -1;

        m_Neutral = gcnew array<bool>(GAS_MAX){false};
        m_Poisonous = gcnew array<bool>(GAS_MAX){false};
    }

    bool IsValid() { return GasRequired != GAS_MAX && TempClass != -1 && PressClass != -1; }

    property GasType    GasRequired;
    property int        ReqMin;
    property int        ReqMax;
    property int        TempClass;
    property int        PressClass;
    property bool       Neutral [int] {
        bool get(int gas)           { return m_Neutral[gas]; }
        void set(int gas, bool val) { m_Neutral[gas] = val; }
    }
    property bool       Poisonous [int] {
        bool get(int gas)           { return m_Poisonous[gas]; }
        void set(int gas, bool val) { m_Poisonous[gas] = val; }
    }

protected:
    array<bool>        ^m_Neutral;
    array<bool>        ^m_Poisonous;
};

public ref class Alien
{
public:
    Alien(String ^name, int turn)
    {
        Name = name;
        GovName = nullptr;
        GovType = nullptr;
        Relation = SP_NEUTRAL;
        TurnMet = turn;
        Email = nullptr;
        HomeSystem = nullptr;
        HomePlanet = -1;
        AtmReq = gcnew AtmosphericReq;
        TechEstimateTurn = -1;

        m_TechLevels        = gcnew array<int>(TECH_MAX){0};
        m_TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
    }

    String^         PrintRelation() { return SpRelToString(Relation); }
    String^         PrintHome();
    String^         PrintTechLevels();

    property String^            Name;
    property String^            GovName;
    property String^            GovType;
    property SPRelType          Relation;
    property int                TurnMet;
    property String^            Email;

    property StarSystem^        HomeSystem;
    property int                HomePlanet;
    property AtmosphericReq^    AtmReq;

    property List<Ship^>^       Ships;
    property List<Colony^>^     Colonies;

    property int                TechEstimateTurn;
    property int                TechLevels [int] {
        int  get(int tech)          { return m_TechLevels[tech]; }
        void set(int tech, int val) { m_TechLevels[tech] = val; }
    }
    property int                TechLevelsTeach [int] {
        int  get(int tech)          { return m_TechLevelsTeach[tech]; }
        void set(int tech, int val) { m_TechLevelsTeach[tech] = val; }
    }

protected:
    array<int>     ^m_TechLevels;
    array<int>     ^m_TechLevelsTeach;
};

public ref class Planet
{
public:
    Planet(StarSystem ^s, int nr, int dia, float gv, int tc, int pc, float md)
    {
        System = s;
        Number = nr;
        Name = nullptr;
        Comment = nullptr;
        Diameter = dia;
        Grav = gv;
        TempClass = tc;
        PressClass = pc;
        MiningDiff = md;
        LSN = -1;
        m_Atmosphere = gcnew array<int>(GAS_MAX) {false};
    }

    int         CalculateLSN(AtmosphericReq^);

    String^     PrintLocation();

    property StarSystem^ System;
    property int         Number;
    property String^     Name;
    property String^     Comment;
    property int         Diameter;
    property float       Grav;
    property int         TempClass;
    property int         PressClass;
    property float       MiningDiff;
    property int         LSN;
    property int         Atmosphere [int] {
        int  get(int gas)           { return m_Atmosphere[gas]; }
        void set(int gas, int val)  { m_Atmosphere[gas] = val; }
    }

protected:
    array<int> ^m_Atmosphere;
};

public ref class StarSystem
{
public:
    StarSystem(int x, int y, int z, String ^type)
        : m_Planets(gcnew array<Planet^>(0))
    {
        X = x;
        Y = y;
        Z = z;
        Type = type;
        TurnScanned = -1;
        LastVisited = -1;
        Master = nullptr;
    }

    static double   CalcDistance(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo);
    static double   CalcMishap(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo, int gv, int age);

    double      CalcDistance(StarSystem ^s)                         { return CalcDistance(X, Y, Z, s->X, s->Y, s->Z); }
    double      CalcMishap(StarSystem ^s, int gv, int age)          { return CalcMishap(X, Y, Z, s->X, s->Y, s->Z, gv, age); }
    double      CalcDistance(int x, int y, int z)                   { return CalcDistance(X, Y, Z, x, y, z); }
    double      CalcMishap(int x, int y, int z, int gv, int age)    { return CalcMishap(X, Y, Z, x, y, z, gv, age); }

    Planet^     GetPlanet(int plNum);

    bool        IsExplored() { return TurnScanned != -1; }
    int         GetMinLSN();

    String^     GenerateScan();
    String^     PrintLocation() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^     PrintScanTurn();
    String^     PrintColonies();
    //String^     PrintShips();

    property int        X;
    property int        Y;
    property int        Z;
    property String^    Type;
    property String^    Comment;
    property int        TurnScanned;
    property int        LastVisited;

    property List<Ship^>^   Ships;
    property List<Colony^>^ Colonies;
    property Alien^         Master;

    property int        PlanetsCount { int get() { return m_Planets->Length; } }
    property Planet^    Planets [int] {
        Planet^ get(int i) { return m_Planets[i]; }
        void    set(int i, Planet^);
    }

    array<Planet^>^     GetPlanets()        { return m_Planets; }

protected:
    array<Planet^>     ^m_Planets;
};

public ref class Colony
{
public:
    Colony(Alien ^owner, String ^name, StarSystem ^system, int planetNum)
    {
        Owner = owner;
        Name = name;
        PlanetType = PLANET_HOME;
        System = system;
        Planet = nullptr;
        PlanetNum = planetNum;
        AvailPop = 0;
        EconomicEff = 0;
        ProdPenalty = 0;
        EUProd = 0;
        EUFleet = 0;
        MiBase = 0;
        MiDiff = 0;
        MaBase = 0;
        Shipyards = 0;
        LastSeen = -1;
        m_Inventory = gcnew array<int>(INV_MAX){0};
    }

    String^     PrintLocation() { return String::Format("{0} {1}", System->PrintLocation(), PlanetNum); }
    String^     PrintInventoryShort();

    Alien^          Owner;
    String^         Name;
    PlanetType      PlanetType;
    StarSystem^     System;
    Planet^         Planet;
    int             PlanetNum;
    int             AvailPop;
    int             EconomicEff;
    int             ProdPenalty;
    int             EUProd;
    int             EUFleet;
    double          MiBase;
    double          MiDiff;
    double          MaBase;
    int             Shipyards;
    int             LastSeen;

    property int            Inventory [int] {
        int  get(int inv)           { return m_Inventory[inv]; }
        void set(int inv, int val)  { m_Inventory[inv] = val; }
    }

protected:
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

public ref class Ship
{
public:
    Ship(Alien ^owner, ShipType type, String ^name, int size, bool subLight)
    {
        Owner = owner;
        Type = type;
        Name = name;
        SubLight = subLight;
        Age = 0;
        Location = SHIP_LOC_MAX;
        X = -1;
        Y = -1;
        Z = -1;
        PlanetNum = -1;
        System = nullptr;
        Capacity = 0;
        IsPirate = false;

        m_Size = size;
        Size = m_Size;

        m_Cargo = gcnew array<int>(INV_MAX){0};
    }

    String^         PrintClass();
    String^         PrintLocation();
    String^         PrintCargo();

    int             GetMaintenanceCost();
    int             GetUpgradeCost()        { return Age * OriginalCost / 40; }
    int             GetRecycleValue()       { return ((3 * OriginalCost) / 4) * ((60 - Age) / 50); }

    void            CalculateCapacity();

    property Alien^         Owner;
    property ShipType       Type;
    property String^        Name;
    property bool           SubLight;
    property int            Age;
    property ShipLocType    Location;
    property int            X;
    property int            Y;
    property int            Z;
    property int            PlanetNum;
    property StarSystem^    System;
    property int            Capacity;
    property bool           IsPirate;
    property int            Tonnage;
    property int            OriginalCost;

    property int            Size {
        int get() { return m_Size; }
        void set(int val) { m_Size = val; SetupTonnage(); }
    }

    property int            Cargo [int] {
        int  get(int inv)           { return m_Cargo[inv]; }
        void set(int inv, int val)  { m_Cargo[inv] = val; }
    }

protected:
    void            SetupTonnage();

    int             m_Size;
    array<int>     ^m_Cargo;
};

public ref class GameData
{
public:
    GameData();

    String^         GetSummary();
    int             GetLastTurn()               { return m_TurnMax; }

    Alien^          GetSpecies()                { return m_Species; }
    String^         GetSpeciesName()            { return m_Species->Name; }
    void            GetFleetCost(int%, float%);
    Alien^          GetAlien(String ^sp);
    StarSystem^     GetStarSystem(int x, int y, int z);
    Colony^         GetColony(String ^name);
    Ship^           GetShip(String ^name);

    IList<Alien^>^          GetAliens()                         { return m_Aliens->Values; }
    array<StarSystem^>^     GetStarSystems()                    { return m_Systems; }
    IList<PlanetName^>^     GetPlanetNames()                    { return m_PlanetNames->Values; }
    IList<Ship^>^           GetShips()                          { return m_Ships->Values; }
    IList<Colony^>^         GetColonies()                       { return m_Colonies->Values; }

    // ------------------------------------------
    void            Update();
    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);
    void            SetTechLevel(int turn, Alien ^sp, TechType, int, int);
    void            SetFleetCost(int turn, int, float);
    Alien^          AddAlien(int turn, String ^sp);
    void            SetAlienRelation(int turn, String ^sp, SPRelType);
    void            AddStarSystem(int x, int y, int z, String ^type, String ^comment);
    void            AddPlanetScan(int turn, int x, int y, int z, Planet ^planet);
    void            SetTurnStartEU(int turn, int eu);
    void            AddTurnProducedEU(int turn, int eu);
    Colony^         AddColony(int turn, Alien^, String^, StarSystem^, int);
    void            AddPlanetName(int turn, int x, int y, int z, int pl, String ^name);
    Ship^           AddShip(int turn, Alien ^sp, ShipType type, String ^name, int size, bool subLight);

    // ------------------------------------------
protected:
    bool            TurnCheck(int turn);
    int             TurnAlign(int turn);

    void            UpdateAliens();
    void            UpdateSystems();
    void            LinkPlanetNames();
    void            UpdateHomeWorlds();

    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetEconomicSummary();
    String^         GetAliensSummary();
    String^         GetPlanetsSummary();
    String^         GetShipsSummary();

    List<Ship^>^            GetShips(Alien ^sp)                 { return GetShips(nullptr, sp); }
    List<Ship^>^            GetShips(StarSystem ^sys)           { return GetShips(sys, nullptr); }
    List<Ship^>^            GetShips(StarSystem^, Alien^);
    List<Colony^>^          GetColonies(Alien ^sp)              { return GetColonies(nullptr, sp); }
    List<Colony^>^          GetColonies(StarSystem ^sys)        { return GetColonies(sys, nullptr); }
    List<Colony^>^          GetColonies(StarSystem^, Alien^);

    // ------------------------------------------
    Alien              ^m_Species;
    int                 m_TurnEUStart;
    int                 m_TurnEUProduced;
    int                 m_FleetCost;
    float               m_FleetCostPercent;
    array<StarSystem^>                 ^m_Systems;
    SortedList<String^, Alien^>        ^m_Aliens;
    SortedList<String^, Colony^>       ^m_Colonies;
    SortedList<String^, PlanetName^>   ^m_PlanetNames;
    SortedList<String^, Ship^>         ^m_Ships;

    int                 m_TurnMax;
};
