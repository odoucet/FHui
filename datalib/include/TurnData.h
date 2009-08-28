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
// Game data
public ref class TurnData
{
public:
    TurnData(int turn);

    String^         GetSummary();
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

    // ------------------------------------------
    void            Update();
    void            SetSpecies(String ^sp);
    void            SetTechLevel(Alien ^sp, TechType, int, int);
    void            SetFleetCost(int, int);
    void            AddAlien(Alien^);
    Alien^          AddAlien(String ^sp);
    void            SetAlienRelation(String ^sp, SPRelType);
    void            AddStarSystem(StarSystem^ system);
    StarSystem^     AddStarSystem(int x, int y, int z, String ^type, String ^comment);
    void            AddPlanetScan(StarSystem ^system, Planet ^planet);
    void            SetTurnStartEU(int eu);
    void            AddTurnProducedEU(int eu);
    void            AddColony(Colony^);
    Colony^         AddColony(Alien ^sp, String ^name, StarSystem ^system, int plNum);
    void            AddPlanetName(StarSystem ^system, int pl, String ^name);
    Ship^           AddShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system);

protected:

    void            DeleteAlienColonies(StarSystem^ system);

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

    initonly int        m_Turn;
};

} // end namespace FHUI
