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

void GameData::SetTechLevel(Alien ^sp, TechType tech, int lev, int levTeach)
{
    m_CurrentTurnData->SetTechLevel(sp, tech, lev, levTeach);
}

void GameData::SetFleetCost(int cost, int percent)
{
    m_CurrentTurnData->SetFleetCost(cost, percent);
}

Alien^ GameData::AddAlien(String ^sp)
{
    return m_CurrentTurnData->AddAlien(sp);
}

void GameData::SetAlienRelation(String ^sp, SPRelType rel)
{
    m_CurrentTurnData->SetAlienRelation(sp, rel);
}

StarSystem^ GameData::AddStarSystem(int x, int y, int z, String ^type, String ^comment)
{
    return m_CurrentTurnData->AddStarSystem(x, y, z, type, comment);
}

void GameData::AddPlanetScan(StarSystem ^system, Planet ^planet)
{
    m_CurrentTurnData->AddPlanetScan(system, planet);
}

void GameData::SetTurnStartEU(int eu)
{
    m_CurrentTurnData->SetTurnStartEU(eu);
}

void GameData::AddTurnProducedEU(int eu)
{
    m_CurrentTurnData->AddTurnProducedEU(eu);
}

Colony^ GameData::AddColony(Alien ^sp, String ^name, StarSystem ^system, int plNum)
{
    return m_CurrentTurnData->AddColony(sp, name, system, plNum);
}

void GameData::AddPlanetName(StarSystem ^system, int pl, String ^name)
{
    m_CurrentTurnData->AddPlanetName(system, pl, name);
}

Ship^ GameData::AddShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    return m_CurrentTurnData->AddShip(sp, type, name, subLight, system);
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
