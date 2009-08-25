#pragma once

#include "TurnData.h"
#include "GameData.h"

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

    String^         GetSummary()                        { return m_CurrentTurnData->GetSummary(); }
    int             GetCarriedEU()                      { return m_CurrentTurnData->GetCarriedEU(); }

    int             GetFleetCost()                      { return m_CurrentTurnData->GetFleetCost(); }
    int             GetFleetPercentCost()               { return m_CurrentTurnData->GetFleetPercentCost(); }
    Alien^          GetAlien(String ^sp)                { return m_CurrentTurnData->GetAlien(sp); }
    StarSystem^     GetStarSystem(int x, int y, int z)  { return m_CurrentTurnData->GetStarSystem(x,y,z); }
    StarSystem^     GetStarSystem(String ^name)         { return m_CurrentTurnData->GetStarSystem(name); }
    Colony^         GetColony(String ^name)             { return m_CurrentTurnData->GetColony(name); }
    Ship^           GetShip(String ^name)               { return m_CurrentTurnData->GetShip(name); }

    IList<Alien^>^          GetAliens()                 { return m_CurrentTurnData->GetAliens(); }
    IList<StarSystem^>^     GetStarSystems()            { return m_CurrentTurnData->GetStarSystems(); }
    IList<PlanetName^>^     GetPlanetNames()            { return m_CurrentTurnData->GetPlanetNames(); }
    IList<Ship^>^           GetShips()                  { return m_CurrentTurnData->GetShips(); }
    IList<Colony^>^         GetColonies()               { return m_CurrentTurnData->GetColonies(); }

    List<ICommand^>^        GetCommands()               { return m_CurrentTurnData->GetCommands(); }
    void                    AddCommand(ICommand ^cmd)   { m_CurrentTurnData->AddCommand(cmd); }
    void                    DelCommand(ICommand ^cmd)   { m_CurrentTurnData->DelCommand(cmd); }
    void                    SortCommands()              { m_CurrentTurnData->SortCommands(); }

    void            Update();
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

    property bool   AutoEnabled { bool get() { return m_CurrentTurnData->AutoEnabled; } }
    void            SetAutoEnabled(int turn) { return m_CurrentTurnData->SetAutoEnabled(turn); }
    void            SetAutoOrderPreDeparture(int turn, StarSystem^, String^);
    void            SetAutoOrderJumps(int turn, Ship^, String^);
    void            SetAutoOrderProduction(int turn, Colony^, String^, int);

    List<String^>^  GetAutoOrdersPreDeparture(StarSystem^);
    List<String^>^  GetAutoOrdersJumps(Ship^);
    List<Pair<String^, int>^>^  GetAutoOrdersProduction(Colony^);

    static void     SetSpecies(String ^sp);
    static void     SetAtmosphereReq(GasType gas, int, int);
    static void     SetAtmosphereNeutral(GasType gas);
    static void     SetAtmospherePoisonous(GasType gas);

    static int      GetSystemId(int, int, int);
    static property int CurrentTurn { int get() { return m_CurrentTurn; } }
    static property AtmosphericReq^    AtmReq;
    static property Alien^ Player
    {
        Alien^ get ()
        {
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

    bool SelectTurn(int turn)
    {
        m_CurrentTurn = turn;
        if ( m_TurnData->ContainsKey(turn) )
        {
            m_CurrentTurnData = m_TurnData[turn];
            return true;
        }
        else
        {
            m_TurnData[turn] = gcnew TurnData;
            m_CurrentTurnData = m_TurnData[turn];
            return false;
        }
    }

protected:
    static String^              m_PlayerName;
    static int                  m_CurrentTurn;
    static TurnData^            m_CurrentTurnData;
    SortedList<int, TurnData^>^ m_TurnData;
};

} // end namespace FHUI
