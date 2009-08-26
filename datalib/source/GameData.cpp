#include "StdAfx.h"
#include "GameData.h"

#include "Commands.h"

namespace FHUI
{

// ---------------------------------------------------------

String^ GameData::PrintInventory(array<int> ^inv)
{
    String ^ret = "";

    for( int i = 0; i < INV_MAX; ++i )
    {
        if( inv[i] )
        {
            ret = String::Format("{0}{1}{2} {3}",
                ret,
                String::IsNullOrEmpty(ret) ? "" : ", ",
                inv[i],
                FHStrings::InvToString(static_cast<InventoryType>(i)) );
        }
    }

    return ret;
}

void GameData::SetSpecies(String ^sp)
{
    if ( String::IsNullOrEmpty(m_PlayerName) )
    {
        m_PlayerName = sp;
        m_CurrentTurnData->SetSpecies(sp);
    }
}

// ---------------------------------------------------------

GameData::GameData(void)
    : m_TurnData(gcnew SortedList<int, TurnData^>)
{
    if (AtmReq == nullptr)
    {
        AtmReq = gcnew AtmosphericReq();
    }
}

void GameData::InitTurnFrom(int srcTurn)
{
    // Copy relevant data from one turn to another
    // copy constructors invoked DO NOT perform full copies !!!

    for each ( StarSystem^ srcSystem in m_TurnData[srcTurn]->GetStarSystems() )
    {
        if ( srcSystem->IsVoid == false )
        {
            m_CurrentTurnData->AddStarSystem( gcnew StarSystem(srcSystem) );
        }
    }

    for each ( Alien^ srcAlien in m_TurnData[srcTurn]->GetAliens() )
    {
        if ( srcAlien->HomeSystem == nullptr )
        {
            m_CurrentTurnData->AddAlien( gcnew Alien(nullptr, srcAlien) );
        }
        else
        {
            StarSystem^ home = m_CurrentTurnData->GetStarSystem(
                srcAlien->HomeSystem->X,
                srcAlien->HomeSystem->Y,
                srcAlien->HomeSystem->Z );
            m_CurrentTurnData->AddAlien( gcnew Alien(home, srcAlien) );
        }
    }

    for each ( Colony^ srcColony in m_TurnData[srcTurn]->GetColonies() )
    {
        if ( srcColony->Owner->Name != m_PlayerName )
        {
            StarSystem^ system = m_CurrentTurnData->GetStarSystem(
                srcColony->System->X,
                srcColony->System->Y,
                srcColony->System->Z );
            Alien^ owner = m_CurrentTurnData->GetAlien( srcColony->Owner->Name );
            m_CurrentTurnData->AddColony( gcnew Colony( system, owner, srcColony) );
        }
    }
}

int GameData::GetSystemId(int x, int y, int z)
{
    return (x * MaxGalaxyDiameter + y) * MaxGalaxyDiameter + z;
}

void GameData::Update()
{
    m_CurrentTurnData->Update();
}

void GameData::SetTechLevel(int turn, Alien ^sp, TechType tech, int lev, int levTeach)
{
    m_CurrentTurnData->SetTechLevel(turn, sp, tech, lev, levTeach);
}

void GameData::SetFleetCost(int turn, int cost, int percent)
{
    m_CurrentTurnData->SetFleetCost(turn, cost, percent);
}

Alien^ GameData::AddAlien(int turn, String ^sp)
{
    return m_CurrentTurnData->AddAlien(turn, sp);
}

void GameData::SetAlienRelation(int turn, String ^sp, SPRelType rel)
{
    m_CurrentTurnData->SetAlienRelation(turn, sp, rel);
}

StarSystem^ GameData::AddStarSystem(int x, int y, int z, String ^type, String ^comment)
{
    return m_CurrentTurnData->AddStarSystem(x, y, z, type, comment);
}

void GameData::AddPlanetScan(int turn, StarSystem ^system, Planet ^planet)
{
    m_CurrentTurnData->AddPlanetScan(turn, system, planet);
}

void GameData::SetTurnStartEU(int turn, int eu)
{
    m_CurrentTurnData->SetTurnStartEU(turn, eu);
}

void GameData::AddTurnProducedEU(int turn, int eu)
{
    m_CurrentTurnData->AddTurnProducedEU(turn, eu);
}

Colony^ GameData::AddColony(int turn, Alien ^sp, String ^name, StarSystem ^system, int plNum)
{
    return m_CurrentTurnData->AddColony(turn, sp, name, system, plNum);
}

void GameData::AddPlanetName(int turn, StarSystem ^system, int pl, String ^name)
{
    m_CurrentTurnData->AddPlanetName(turn, system, pl, name);
}

Ship^ GameData::AddShip(int turn, Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    return m_CurrentTurnData->AddShip(turn, sp, type, name, subLight, system);
}

void GameData::SetAutoOrderPreDeparture(int turn, StarSystem^ system, String^ order)
{
    m_CurrentTurnData->SetAutoOrderPreDeparture(turn, system, order);
}

void GameData::SetAutoOrderJumps(int turn, Ship^ ship, String^ order)
{
    m_CurrentTurnData->SetAutoOrderJumps(turn, ship, order);
}

void GameData::SetAutoOrderProduction(int turn, Colony^ colony, String^ order, int cost)
{
    m_CurrentTurnData->SetAutoOrderProduction(turn, colony, order, cost);
}

List<String^>^ GameData::GetAutoOrdersPreDeparture(StarSystem^ system)
{
    return m_CurrentTurnData->GetAutoOrdersPreDeparture(system);
}

List<String^>^ GameData::GetAutoOrdersJumps(Ship^ ship)
{
    return m_CurrentTurnData->GetAutoOrdersJumps(ship);
}

List<Pair<String^, int>^>^ GameData::GetAutoOrdersProduction(Colony^ colony)
{
    return m_CurrentTurnData->GetAutoOrdersProduction(colony);
}

void GameData::SetAtmosphereReq(GasType gas, int reqMin, int reqMax)
{
    if( (AtmReq->GasRequired != GAS_MAX && AtmReq->GasRequired != gas) ||
        AtmReq->Neutral[gas] ||
        AtmReq->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    AtmReq->GasRequired = gas;
    AtmReq->ReqMin = reqMin;
    AtmReq->ReqMax = reqMax;
}

void GameData::SetAtmosphereNeutral(GasType gas)
{
    if( AtmReq->GasRequired == gas ||
        AtmReq->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    AtmReq->Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    if( AtmReq->GasRequired == gas ||
        AtmReq->Neutral[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    AtmReq->Poisonous[gas] = true;
}

} // end namespace FHUI
