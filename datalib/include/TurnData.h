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
    StarSystem^     GetStarSystem(int id);
    StarSystem^     GetStarSystem(int x, int y, int z, bool allowVoid);
    StarSystem^     GetStarSystem(String ^name);

    IList<Alien^>^          GetAliens()                         { return m_Aliens->Values; }
    IList<StarSystem^>^     GetStarSystems()                    { return m_Systems->Values; }

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
    Colony^         CreateColony(Alien ^sp, String ^name, StarSystem ^system, int plNum, bool);
    void            AddColony(Colony ^colony);
    void            RemoveColony(String ^name);
    Ship^           CreateShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system);
    void            RemoveShip(Ship ^ship);
    void            AddWormholeJump(String ^shipName, int fromSystemId);
    void            AddMishap(String ^shipName);

    void            PrintDebugStats();

    void            SetParsingUserContent(bool state) { m_bParsingUserContent = state; };

protected:
    void            UpdateShips();
    void            UpdateAliens();
    void            UpdateSystems();
    void            UpdateHomeWorlds();
    void            UpdateColonies();

    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetEconomicSummary();
    String^         GetAliensSummary();
    String^         GetPlanetsSummary();
    String^         GetShipsSummary();

    int                 m_TurnEUStart;
    int                 m_TurnEUProduced;
    int                 m_FleetCost;
    int                 m_FleetCostPercent; // * 100
    SortedList<int, StarSystem^>^       m_Systems;
    SortedList<String^, Alien^>^        m_Aliens;

    typedef Pair<String^, int> WormholeJump;
    List<WormholeJump^>^                m_WormholeJumps;
    List<String^>^                      m_Misjumps;

    initonly int        m_Turn;
    bool                m_bParsingFinished;
    bool                m_bParsingUserContent;
};

} // end namespace FHUI
