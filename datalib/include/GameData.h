#pragma once

#include "TurnData.h"
#include "GameData.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

interface class ICommand;

// ---------------------------------------------------
// Game data
public ref class GameData
{
public:
    GameData();

    static String^         GetSummary()                        { return m_CurrentTurnData->GetSummary(); }
    static int             GetCarriedEU()                      { return m_CurrentTurnData->GetCarriedEU(); }

    static int             GetFleetCost()                      { return m_CurrentTurnData->GetFleetCost(); }
    static int             GetFleetPercentCost()               { return m_CurrentTurnData->GetFleetPercentCost(); }
    static Alien^          GetAlien(String ^sp)                { return m_CurrentTurnData->GetAlien(sp); }
    static StarSystem^     GetStarSystem(int id)               { return m_CurrentTurnData->GetStarSystem(id); }
    static StarSystem^     GetStarSystem(int x, int y, int z, bool allowVoid)  { return m_CurrentTurnData->GetStarSystem(x, y, z, allowVoid); }
    static StarSystem^     GetStarSystem(String ^name)         { return m_CurrentTurnData->GetStarSystem(name); }
    static Colony^         GetColony(String ^name)             { return m_CurrentTurnData->GetColony(name); }
    static Ship^           GetShip(String ^name)               { return m_CurrentTurnData->GetShip(name); }
    static Planet^         GetPlanetByName(String ^name)       { return m_CurrentTurnData->GetPlanetByName(name); }

    static IList<Alien^>^          GetAliens()          { return m_CurrentTurnData->GetAliens(); }
    static IList<StarSystem^>^     GetStarSystems()     { return m_CurrentTurnData->GetStarSystems(); }
    static IList<PlanetName^>^     GetPlanetNames()     { return m_CurrentTurnData->GetPlanetNames(); }
    static IList<Ship^>^           GetShips()           { return m_CurrentTurnData->GetShips(); }
    static IList<Colony^>^         GetColonies()        { return m_CurrentTurnData->GetColonies(); }

    void            Update();
    void            SetTechLevel(Alien ^sp, TechType, int, int);
    void            SetFleetCost(int, int);
    Alien^          AddAlien(String ^sp);
    void            SetAlienRelation(String ^sp, SPRelType);
    StarSystem^     AddStarSystem(int x, int y, int z, String ^type, String ^comment);
    void            AddPlanetScan(StarSystem ^system, Planet ^planet);
    void            SetTurnStartEU(int eu);
    void            AddTurnProducedEU(int eu);
    Colony^         AddColony(Alien ^sp, String ^name, StarSystem ^system, int plNum);
    void            AddPlanetName(StarSystem ^system, int pl, String ^name);
    Ship^           AddShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system);
    void            AddWormholeJump(String ^shipName, int fromSystemId);

    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);

    static int      GetSystemId(int, int, int);
    static property int CurrentTurn { int get() { return m_CurrentTurn; } }
    static property Alien^ Player
    {
        Alien^ get ()
        {
            // In "turn 0" player may be uninitialized
            if( m_CurrentTurn == 0 &&
                String::IsNullOrEmpty(m_PlayerName) )
            {
                return nullptr;
            }

            try
            {
                return m_CurrentTurnData->GetAlien( m_PlayerName );
            }
            catch ( FHUIDataIntegrityException^ )
            {
                return nullptr;
            }
        }
    }
    static const int MaxGalaxyDiameter = 100;
    static int GalaxyDiameter = MaxGalaxyDiameter;

    static String^ PrintInventory(array<int> ^inv);

    bool SelectTurn(int turn);

protected:
    void        InitTurn();

    static String^              m_PlayerName;
    static int                  m_CurrentTurn;
    static int                  m_PrevTurn;
    static TurnData^            m_CurrentTurnData;
    SortedList<int, TurnData^>^ m_TurnData;
};

} // end namespace FHUI
