#pragma once

#include "Alien.h"
#include "Colony.h"
#include "Planet.h"
#include "Ship.h"
#include "StarSystem.h"

#include "Calculators.h"
#include "FHStrings.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

interface class ICommand;

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
    void            AddPlanetScan(int turn, StarSystem ^system, Planet ^planet);
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
    static property AtmosphericReq^    AtmReq;
    static property Alien^  Player;
    static const int MaxGalaxyDiameter = 100;
    static int GalaxyDiameter = MaxGalaxyDiameter;

    static String^ PrintInventory(array<int> ^inv);

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

} // end namespace FHUI
