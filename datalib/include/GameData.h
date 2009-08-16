#pragma once

#include "enums.h"
#include "IGridData.h"
#include "FHStrings.h"
#include "Calculators.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

ref class StarSystem;
ref class Ship;
ref class Colony;
interface class ICommand;

// ---------------------------------------------------
// -- base IGridDataSrc impl --
public ref class GridDataSrcBase : public IGridDataSrc
{
public:
    virtual Alien^      GetAlienForBgColor()        { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual String^     GetTooltipText()            { return "<TODO...>"; }

    virtual StarSystem^ GetFilterSystem()           { return nullptr; }
    virtual StarSystem^ GetFilterLocation(int %pl)  { return nullptr; }
    virtual Alien^      GetFilterOwner()            { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual int         GetFilterLSN()              { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual int         GetFilterNumColonies()      { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual SPRelType   GetFilterRelType()          { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual ShipType    GetFilterShipType()         { throw gcnew FHUIDataImplException("Not implemented!"); }
};

// ---------------------------------------------------
// Atmospheric requirements
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

// ---------------------------------------------------
// Alien species
public ref class Alien : public GridDataSrcBase
{
public:
    Alien(String ^name, int turn)
    {
        Name = name;
        GovName = nullptr;
        GovType = nullptr;
        Relation = SP_NEUTRAL;
        RelationOriginal = SP_NEUTRAL;
        TurnMet = turn;
        Email = nullptr;
        HomeSystem = nullptr;
        HomePlanet = -1;
        AtmReq = gcnew AtmosphericReq;
        TechEstimateTurn = -1;
        Ships = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;

        TechLevels        = gcnew array<int>(TECH_MAX){0};
        TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
        TechLevelsAssumed = gcnew array<int>(TECH_MAX){0};
        TeachOrders = 0;
    }

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return this; }
    virtual SPRelType   GetFilterRelType() override     { return Relation; }
    // -------- IComparers ------------------------------
    ref class RelationComparer : public IComparer<Alien^>
    {
    public:
        virtual int Compare(Alien ^a1, Alien ^a2)
        {
            if( a1->Relation == a2->Relation )
                return String::Compare(a1->Name, a2->Name);
            return a1->Relation - a2->Relation;
        }
    };
    // --------------------------------------------------

    void            SortColoniesByProdOrder();

    String^         PrintRelation() { return FHStrings::SpRelToString(Relation); }
    String^         PrintHome();
    String^         PrintTechLevels();

    property String^            Name;
    property String^            GovName;
    property String^            GovType;
    property SPRelType          Relation;
    property SPRelType          RelationOriginal;
    property int                TurnMet;
    property String^            Email;

    property StarSystem^        HomeSystem;
    property int                HomePlanet;
    property AtmosphericReq^    AtmReq;

    property List<Ship^>^       Ships;
    property List<Colony^>^     Colonies;

    property int                TechEstimateTurn;
    property array<int>^        TechLevels;
    property array<int>^        TechLevelsTeach;
    property array<int>^        TechLevelsAssumed;
    property int                TeachOrders;
};

// ---------------------------------------------------
// Planet in star system
public ref class Planet : public GridDataSrcBase
{
public:
    Planet(StarSystem ^s, int nr, int dia, float gv, int tc, int pc, int md)
    {
        System = s;
        Number = nr;
        Name = nullptr;
        NameIsNew = false;
        NameIsDisband = false;
        Comment = nullptr;
        Diameter = dia;
        Grav = gv;
        TempClass = tc;
        PressClass = pc;
        MiDiff = md;
        LSN = -1;
        Master = nullptr;
        NumColonies = 0;
        Atmosphere = gcnew array<int>(GAS_MAX) {0};
        SuspectedColonies = gcnew SortedList<Alien^, int>;
    }

    static Planet^ Default(StarSystem ^system, int nr)
    {
        return gcnew Planet(system, nr, 99, 99.99F, 99, 99, 999);
    }

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = Number; return System; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return LSN; }
    virtual int         GetFilterNumColonies() override { return NumColonies; }
    // --------------------------------------------------

    int         CalculateLSN(AtmosphericReq^);

    String^     GetNameWithOrders();
    void        AddName(String ^name);
    void        DelName();

    String^     PrintLocation();
    String^     PrintLocationAligned();
    String^     PrintComment();

    property StarSystem^    System;
    property int            Number;
    property String^        Name;
    property bool           NameIsNew;
    property bool           NameIsDisband;
    property String^        Comment;
    property int            Diameter;
    property float          Grav;
    property int            TempClass;
    property int            PressClass;
    property int            MiDiff; // * 100
    property int            LSN;
    property Alien^         Master;
    property int            NumColonies;
    property int            NumColoniesOwned;
    property array<int>^    Atmosphere;
    property SortedList<Alien^, int>^ SuspectedColonies;
};

// ---------------------------------------------------
// Star system
public ref class StarSystem : public GridDataSrcBase, public IComparable
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
        MinLSN = 99999;
        MinLSNAvail = 99999;
        Master = nullptr;
        Ships = gcnew List<Ship^>;
        ShipsOwned = gcnew List<Ship^>;
        ShipsAlien = gcnew List<Ship^>;
        Colonies = gcnew List<Colony^>;
        ColoniesOwned = gcnew List<Colony^>;
        ColoniesAlien = gcnew List<Colony^>;
        IsVoid = true;
    }

    // -------- IComparable ----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Master; }
    virtual String^     GetTooltipText() override       { return GenerateScan(); }

    virtual StarSystem^ GetFilterSystem() override      { return this; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = -1; return this; }
    virtual Alien^      GetFilterOwner() override       { return Master; }
    virtual int         GetFilterLSN() override         { return MinLSN; }
    virtual int         GetFilterNumColonies() override { return Colonies->Count; }
    // --------------------------------------------------

    double      CalcDistance(StarSystem ^s)                         { return Calculators::Distance(X, Y, Z, s->X, s->Y, s->Z); }
    double      CalcMishap(StarSystem ^s, int gv, int age)          { return Calculators::Mishap(X, Y, Z, s->X, s->Y, s->Z, gv, age); }
    double      CalcDistance(int x, int y, int z)                   { return Calculators::Distance(X, Y, Z, x, y, z); }
    double      CalcMishap(int x, int y, int z, int gv, int age)    { return Calculators::Mishap(X, Y, Z, x, y, z, gv, age); }

    int         CompareLocation(StarSystem ^sys);

    int         GetId();
    Planet^     GetPlanet(int plNum);

    bool        IsExplored() { return TurnScanned != -1; }

    String^     GenerateScan();
    void        UpdateMaster();

    String^         PrintLocation() { return String::Format("{0} {1} {2}", X, Y, Z); }
    String^         PrintLocationAligned() { return String::Format("{0,2} {1,2} {2,2}", X, Y, Z); }
    String^         PrintScanStatus();
    String^         PrintColonies(int planetNum, Alien ^player);   // -1 for all colonies in system
    List<String^>^  PrintAliens();

    property int        X;
    property int        Y;
    property int        Z;
    property String^    Type;
    property String^    Comment;
    property int        TurnScanned;
    property int        LastVisited;
    property int        MinLSN;
    property int        MinLSNAvail;
    property bool       IsVoid;

    property Alien^     HomeSpecies;

    property List<Ship^>^   Ships;
    property List<Ship^>^   ShipsOwned;
    property List<Ship^>^   ShipsAlien;
    property List<Colony^>^ Colonies;
    property List<Colony^>^ ColoniesOwned;
    property List<Colony^>^ ColoniesAlien;
    property Alien^         Master;

    property int        PlanetsCount { int get() { return m_Planets->Length; } }
    property Planet^    Planets [int] {
        Planet^ get(int i) { return i < m_Planets->Length ? m_Planets[i] : nullptr; }
        void    set(int i, Planet^);
    }

    array<Planet^>^     GetPlanets()        { return m_Planets; }

protected:
    array<Planet^>     ^m_Planets;

    initonly static String^ s_ScanNone = "Not scanned";
    initonly static String^ s_ScanDipl = "Received";
    initonly static String^ s_ScanSelf = "Scanned";
};

// ---------------------------------------------------
// Colony
public ref class Colony : public GridDataSrcBase, public IComparable
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
        MiBase = -1;
        NeedIU = 0;
        NeedAU = 0;
        RecoveryIU = 0;
        RecoveryAU = 0;
        MiDiff = 0;
        MaBase = -1;
        LSN = 99;
        Shipyards = -1;
        LastSeen = -1;
        Inventory = gcnew array<int>(INV_MAX){0};
        OrderBuildShipyard = false;
        Hidden = false;
        UnderSiege = false;
        Shared = false;
    }

    // -------- IComparable -----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Owner; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = PlanetNum; return System; }
    virtual Alien^      GetFilterOwner() override       { return Owner; }
    virtual int         GetFilterLSN() override         { return Planet ? Planet->LSN : 99999; }
    // -------- IComparers ------------------------------
    ref class ProdOrderComparer : public IComparer<Colony^>
    {
    public:
        virtual int Compare(Colony ^c1, Colony ^c2)
        {
            return c1->ProductionOrder - c2->ProductionOrder;
        }
    };
    // --------------------------------------------------

    String^         PrintLocation() { return String::Format("{0} {1}", System->PrintLocation(), PlanetNum); }
    String^         PrintInventory();
    String^         PrintRefListEntry();
    String^         PrintBalance();

    void            CalculateBalance(bool MiMaBalanced);
    int             GetMaxProductionBudget();

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
    int             MaBase; // *10
    int             MiBase; // *10
    int             NeedIU;
    int             NeedAU;
    int             RecoveryIU;
    int             RecoveryAU;
    int             MiDiff; // *100
    int             LSN;
    int             Shipyards;
    int             LastSeen;
    int             ProductionOrder;
    bool            CanProduce;
    bool            OrderBuildShipyard;
    bool            Hidden;
    bool            UnderSiege;
    bool            Shared;

    property int EconomicBase { int get() { return Math::Max(-1, MiBase + MaBase); } }
    property int EUAvail { int get() { return EUProd - EUFleet; } }

    property array<int>^    Inventory;
    property bool           HasInventory 
    {
        bool get()
        {
            for (int i=0; i<INV_MAX; i++)
                if (Inventory[i] > 0) return true;
            return false;
        }
    }
};

// ---------------------------------------------------
// Named planet info
public ref class PlanetName
{
public:
    PlanetName(StarSystem ^system, int num, String ^name)
    {
        System      = system;
        PlanetNum   = num;
        Name        = name;
    }

    property StarSystem^    System;
    property int            PlanetNum;
    property String^        Name;
};

// ---------------------------------------------------
// Ship or starbase
public ref class Ship : public GridDataSrcBase, public IComparable
{
public:
    Ship(Alien ^owner, ShipType type, String ^name, bool subLight)
    {
        Owner = owner;
        Type = type;
        Name = name;
        SubLight = subLight;
        CanJump = !subLight && type != SHIP_BAS;
        Age = 0;
        Size = 0;
        EUToComplete = 0;
        Location = SHIP_LOC_MAX;
        PlanetNum = -1;
        System = nullptr;
        Capacity = 0;
        IsPirate = false;
        m_Cargo = gcnew array<int>(INV_MAX){0};
    }

    // -------- IComparable -----------------------------
    virtual Int32 CompareTo( Object^ obj );

    // -------- IGridDataSrc ----------------------------
    virtual Alien^      GetAlienForBgColor() override   { return Owner; }

    virtual StarSystem^ GetFilterSystem() override      { return System; }
    virtual StarSystem^ GetFilterLocation(int %pl) override { pl = PlanetNum; return System; }
    virtual Alien^      GetFilterOwner() override       { return Owner; }
    virtual SPRelType   GetFilterRelType() override     { return Owner->GetFilterRelType(); }
    virtual ShipType    GetFilterShipType() override    { return Type; }
    // -------- IComparers ------------------------------
    ref class TypeTonnageLocationComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            if( s1->Type != s2->Type ) return ((int)s1->Type - (int)s2->Type);
            if( s1->Tonnage != s2->Tonnage ) return s1->Tonnage - s2->Tonnage;
            if( s1->System->X != s2->System->X ) return s1->System->X - s2->System->X;
            if( s1->System->Y != s2->System->Y ) return s1->System->Y - s2->System->Y;
            if( s1->System->Z != s2->System->Z ) return s1->System->Z - s2->System->Z;
            if( s1->Age != s2->Age) return s1->Age - s2->Age;
            return s1->Location - s2->Location;
        }
    };

    ref class LocationComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            if( s1->System->X != s2->System->X ) return s1->System->X - s2->System->X;
            if( s1->System->Y != s2->System->Y ) return s1->System->Y - s2->System->Y;
            if( s1->System->Z != s2->System->Z ) return s1->System->Z - s2->System->Z;
            return s1->Location - s2->Location;
        }
    };

    ref class WarTonnageComparer : public IComparer<Ship^>
    {
    public:
        virtual int Compare(Ship ^s1, Ship ^s2)
        {
            // Newer, Bigger ships first
            if( s2->WarTonnage == s1->WarTonnage )
                return s1->Age - s2->Age;
            return s2->WarTonnage - s1->WarTonnage;
        }
    };
    // --------------------------------------------------

    String^         PrintClass();
    String^         PrintClassWithName()                { return PrintClass() + " " + Name; }
    String^         PrintLocation();
    String^         PrintLocationShort();
    String^         PrintAgeLocation();
    String^         PrintCargo();
    String^         PrintRefListEntry();

    int             GetMaintenanceCost();
    int             GetUpgradeCost();
    int             GetRecycleValue();

    void            CalculateCapacity();

    property Alien^         Owner;
    property ShipType       Type;
    property String^        Name;
    property bool           SubLight;
    property bool           CanJump;
    property int            Age;
    property ShipLocType    Location;
    property int            PlanetNum;
    property StarSystem^    System;
    property int            Capacity;
    property bool           IsPirate;
    property int            Tonnage;
    property int            WarTonnage;
    property int            OriginalCost;
    property int            EUToComplete;

    property int            Size {
        int get() { return m_Size; }
        void set(int val) { m_Size = val; SetupTonnage(); }
    }

    property int            Cargo [int] {
        int  get(int inv)           { return m_Cargo[inv]; }
        void set(int inv, int val)  { m_Cargo[inv] = val; }
    }

    // ---- Ship orders ----
    enum class OrderType
    {
        Jump, Upgrade, Recycle
    };

    ref class Order
    {
    public:
        Order(OrderType type)
            : Type(type)
            , JumpTarget(nullptr)
            , PlanetNum(-1)
        {}
        Order(OrderType type, StarSystem ^system)
            : Type(type)
            , JumpTarget(system)
            , PlanetNum(-1)
        {}
        Order(OrderType type, StarSystem ^system, int plNum)
            : Type(type)
            , JumpTarget(system)
            , PlanetNum(plNum)
        {}

        String^     Print();
        String^     PrintNumeric();
        String^     PrintJumpDestination();

        OrderType   Type;
        StarSystem^ JumpTarget;
        int         PlanetNum;
    };

    property Order^ Command;

protected:
    void            SetupTonnage();

    int             m_Size;
    array<int>     ^m_Cargo;
};

// ---------------------------------------------------
// Game data
public ref class GameData
{
public:
    GameData();

    String^         GetSummary();
    int             GetLastTurn()               { return m_TurnMax; }
    int             GetCarriedEU()              { return m_TurnEUStart; }

    int             GetFleetCost();
    int             GetFleetPercentCost();
    Alien^          GetAlien(String ^sp);
    StarSystem^     GetStarSystem(int x, int y, int z);
    StarSystem^     GetStarSystem(String ^name);
    Colony^         GetColony(String ^name);
    Ship^           GetShip(String ^name);

    IList<Alien^>^          GetAliens()                         { return m_Aliens->Values; }
    IList<StarSystem^>^     GetStarSystems()                    { return m_Systems->Values; }
    IList<PlanetName^>^     GetPlanetNames()                    { return m_PlanetNames->Values; }
    IList<Ship^>^           GetShips()                          { return m_ShipsByTonnage; }
    IList<Colony^>^         GetColonies()                       { return m_Colonies->Values; }

    List<ICommand^>^    GetCommands()                       { return m_Commands; }
    void                AddCommand(ICommand ^cmd)           { m_Commands->Add(cmd); }
    void                DelCommand(ICommand ^cmd)           { m_Commands->Remove(cmd); }
    void                SortCommands();

    // ------------------------------------------
    void            Update();
    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);
    void            SetTechLevel(int turn, Alien ^sp, TechType, int, int);
    void            SetFleetCost(int turn, int, int);
    Alien^          AddAlien(int turn, String ^sp);
    void            SetAlienRelation(int turn, String ^sp, SPRelType);
    StarSystem^     AddStarSystem(int x, int y, int z, String ^type, String ^comment);
    void            AddPlanetScan(int turn, int x, int y, int z, Planet ^planet);
    void            SetTurnStartEU(int turn, int eu);
    void            AddTurnProducedEU(int turn, int eu);
    Colony^         AddColony(int turn, Alien ^sp, String ^name, StarSystem ^system, int plNum);
    void            AddPlanetName(int turn, StarSystem ^system, int pl, String ^name);
    Ship^           AddShip(int turn, Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system);

    property bool   AutoEnabled;

    void            SetAutoEnabled(int turn);
    void            SetAutoOrderPreDeparture(int turn, StarSystem^, String^);
    void            SetAutoOrderJumps(int turn, Ship^, String^);
    void            SetAutoOrderProduction(int turn, Colony^, String^, int);

    List<String^>^  GetAutoOrdersPreDeparture(StarSystem^);
    List<String^>^  GetAutoOrdersJumps(Ship^);
    List<Pair<String^, int>^>^  GetAutoOrdersProduction(Colony^);

    static int      GetSystemId(int, int, int);
    // ------------------------------------------

    static property Alien^  Player;
    static const int MaxGalaxyDiameter = 100;
    static int GalaxyDiameter = MaxGalaxyDiameter;

protected:
    bool            TurnCheck(int turn);
    int             TurnAlign(int turn);

    void            UpdateShips();
    void            UpdateAliens();
    void            UpdateSystems();
    void            LinkPlanetNames();
    void            UpdateHomeWorlds();
    void            UpdateColonies();

    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetEconomicSummary();
    String^         GetAliensSummary();
    String^         GetPlanetsSummary();
    String^         GetShipsSummary();

    List<Ship^>^        GetShips(Alien ^sp)                 { return GetShips(nullptr, sp); }
    List<Ship^>^        GetShips(StarSystem ^sys)           { return GetShips(sys, nullptr); }
    List<Ship^>^        GetShips(StarSystem^, Alien^);
    List<Colony^>^      GetColonies(Alien ^sp)              { return GetColonies(nullptr, sp); }
    List<Colony^>^      GetColonies(StarSystem ^sys)        { return GetColonies(sys, nullptr); }
    List<Colony^>^      GetColonies(StarSystem^, Alien^);

    // ------------------------------------------
    int                 m_TurnEUStart;
    int                 m_TurnEUProduced;
    int                 m_FleetCost;
    int                 m_FleetCostPercent; // * 100
    SortedList<int, StarSystem^>       ^m_Systems;
    SortedList<String^, Alien^>        ^m_Aliens;
    SortedList<String^, Colony^>       ^m_Colonies;
    SortedList<String^, PlanetName^>   ^m_PlanetNames;
    SortedList<String^, Ship^>         ^m_Ships;
    List<Ship^>                        ^m_ShipsByTonnage;

    List<ICommand^>^    m_Commands;

    SortedList<StarSystem^, List<String^>^> ^m_AutoOrdersPreDeparture;
    SortedList<Ship^, List<String^>^>       ^m_AutoOrdersJumps;
    SortedList<Colony^, List<Pair<String^, int>^>^> ^m_AutoOrdersProduction;

    int                 m_TurnMax;
};

// ---------------------------------------------------

} // end namespace FHUI
