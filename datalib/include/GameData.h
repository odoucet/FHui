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

    static IList<Alien^>^          GetAliens()          { return m_CurrentTurnData->GetAliens(); }
    static IList<StarSystem^>^     GetStarSystems()     { return m_CurrentTurnData->GetStarSystems(); }

    static void         SetParsingUserContent(bool state)      { return m_CurrentTurnData->SetParsingUserContent(state); }

    static void         Update();
    static void         SetTechLevel(Alien ^sp, TechType, int, int);
    static void         SetFleetCost(int, int);
    static Alien^       AddAlien(String ^sp);
    static void         SetAlienRelation(String ^sp, SPRelType);
    static StarSystem^  AddStarSystem(int x, int y, int z, String ^type, String ^comment);
    static void         AddPlanetScan(StarSystem ^system, Planet ^planet);
    static void         SetTurnStartEU(int eu);
    static void         AddTurnProducedEU(int eu);
    static Colony^      CreateColony(Alien ^sp, String ^name, StarSystem ^system, int plNum, bool);
    static void         RemoveColony(String ^name);
    static Ship^        CreateShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system);
    static void         RemoveShip(Ship ^ship)          { m_CurrentTurnData->RemoveShip(ship); }
    static void         AddWormholeJump(String ^shipName, int fromSystemId);
    static void         AddMishap(String ^shipName);

    static void         SetSpecies(String ^sp);
    static void         SetAtmosphereReq(GasType gas, int, int);
    static void         SetAtmosphereNeutral(GasType gas);
    static void         SetAtmospherePoisonous(GasType gas);

    static int          GetSystemId(int x, int y, int z);
    static property int CurrentTurn { int get() { return m_CurrentTurn; } }
    static property Alien^ Player
    {
        Alien^ get ()
        {
            // In "turn 0" or while initializing turn 1
            // player may be uninitialized
            if( String::IsNullOrEmpty(m_PlayerName) )
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

    static void     EvalPostArrivalInventory(StarSystem ^system, ICommand ^cmdEnd);
    static String^  PrintInventory(array<int> ^inv);

    bool SelectTurn(int turn);

    void PrintDebugStats();

protected:
    void        InitTurn();

    static String^              m_PlayerName;
    static int                  m_CurrentTurn;
    static int                  m_PrevTurn;
    static TurnData^            m_CurrentTurnData;
    SortedList<int, TurnData^>^ m_TurnData;
};

} // end namespace FHUI
