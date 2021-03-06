#include "StdAfx.h"
#include "GameData.h"

#include "Commands.h"

namespace FHUI
{

// ---------------------------------------------------------

void GameData::EvalPreDepartureInventory(StarSystem ^system, ICommand ^cmdEnd, bool transfersOnly)
{
    IList<Ship^> ^ships         = system->ShipsOwned;
    IList<Colony^> ^colonies    = system->ColoniesOwned;

    // Copy initial data at the beginning of Post-Arrival phase
    for each( Colony ^colony in colonies )
        colony->Inventory->CopyTo(colony->InventoryPreDeparture, 0);
    for each( Ship ^ship in ships )
        ship->CargoOriginal->CopyTo(ship->CargoPreDeparture, 0);

    // Transfers
    for each( ICommand ^cmd in system->Transfers )
    {
        if( cmd->GetPhase() != CommandPhase::PreDeparture )
            continue;
        if( cmd == cmdEnd )
            return;

        CmdTransfer ^cmdTr = safe_cast<CmdTransfer^>(cmd);
        array<int> ^invFrom = cmdTr->GetFromInventory(true);
        array<int> ^invTo = cmdTr->GetToInventory(true);

        int inv = (int)cmdTr->m_Type;
        int amount = Math::Min(cmdTr->m_Amount, invFrom[inv]);
        invFrom[inv] -= amount;
        invTo[inv] += amount;
    }

    if( transfersOnly )
        return;

    // Ships
    for each( Ship ^ship in ships )
    {
        for each( ICommand ^cmd in ship->Commands )
        {
            if( cmd->GetPhase() != CommandPhase::PreDeparture )
                continue;
            if( cmd == cmdEnd )
                return;

            array<int> ^inv = ship->CargoPreDeparture;

            switch( cmd->GetCmdType() )
            {
            case CommandType::Unload:
                inv[INV_CU] = 0;
                inv[INV_AU] = 0;
                inv[INV_IU] = 0;
                break;

            // TODO: Base, Repair when implemented
            }

            //for( int i = 0; i < INV_MAX; ++i )
            //{
            //    int amount = cmd->GetInvMod(static_cast<InventoryType>(i));
            //    if( amount < 0 )
            //        amount = -Math::Min(-amount, inv[i]);
            //    inv[i] += amount;
            //}
        }
    }

    // Colonies
    for each( Colony ^colony in colonies )
    {
        for each( ICommand ^cmd in colony->Commands )
        {
            if( cmd->GetPhase() != CommandPhase::PreDeparture )
                continue;
            if( cmd == cmdEnd )
                return;

            array<int> ^inv = colony->InventoryPreDeparture;
            for( int i = 0; i < INV_MAX; ++i )
            {
                int amount = cmd->GetInvMod(static_cast<InventoryType>(i));
                if( amount < 0 )
                    amount = -Math::Min(-amount, inv[i]);
                inv[i] += amount;
            }
        }
    }
}

void GameData::EvalPostArrivalInventory(StarSystem ^system, ICommand ^cmdEnd)
{
    // Copy initial data at the beginning of Post-Arrival phase
    for each( Colony ^colony in GameData::Player->Colonies )
        colony->Res->Inventory->CopyTo(colony->InventoryPostArrival, 0);
    for each( Ship ^ship in GameData::Player->Ships )
        ship->Cargo->CopyTo(ship->CargoPostArrival, 0);

    for each( ICommand ^cmd in system->Transfers )
    {
        if( cmd->GetPhase() != CommandPhase::PostArrival )
            continue;
        if( cmd == cmdEnd )
            break;

        CmdTransfer ^cmdTr = safe_cast<CmdTransfer^>(cmd);
        array<int> ^invFrom = cmdTr->GetFromInventory(false);
        array<int> ^invTo = cmdTr->GetToInventory(false);

        int inv = (int)cmdTr->m_Type;
        int amount = Math::Min(cmdTr->m_Amount, invFrom[inv]);
        invFrom[inv] -= amount;
        invTo[inv] += amount;
    }
}

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
    m_PrevTurn = -1;
    m_CurrentTurn = -1;
    m_CurrentTurnData = nullptr;
    m_PlayerName = nullptr;
}

bool GameData::SelectTurn(int turn)
{
    m_PrevTurn = m_CurrentTurn;
    m_CurrentTurn = turn;

    if ( m_TurnData->ContainsKey(turn) )
    {
        m_CurrentTurnData = m_TurnData[turn];
        return true;
    }
    else
    {
        m_TurnData[turn] = gcnew TurnData(turn);
        m_CurrentTurnData = m_TurnData[turn];

        if( m_PrevTurn != -1 )
            InitTurn();

        return false;
    }
}

void GameData::InitTurn()
{
    // Copy relevant data from one turn to another
    // copy constructors invoked DO NOT perform full copies !!!
    TurnData ^srcTurn = m_TurnData[m_PrevTurn];

    for each ( StarSystem^ srcSystem in srcTurn->GetStarSystems() )
    {
        if ( srcSystem->IsVoid == false )
        {
            m_CurrentTurnData->AddStarSystem( gcnew StarSystem(srcSystem) );
        }
    }

    for each ( Alien^ srcAlien in srcTurn->GetAliens() )
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
                srcAlien->HomeSystem->Z,
                false );
            m_CurrentTurnData->AddAlien( gcnew Alien(home, srcAlien) );
        }
    }

    for each ( StarSystem^ srcSystem in srcTurn->GetStarSystems() )
    {
        for each ( Colony^ srcColony in srcSystem->ColoniesAlien )
        {
            StarSystem^ system = m_CurrentTurnData->GetStarSystem(
                srcColony->System->X,
                srcColony->System->Y,
                srcColony->System->Z,
                false );
            Alien^ owner = m_CurrentTurnData->GetAlien( srcColony->Owner->Name );
            m_CurrentTurnData->AddColony( gcnew Colony( system, owner, srcColony) );
        }
    }
}

void GameData::PrintDebugStats()
{
    m_CurrentTurnData->PrintDebugStats();
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

Colony^ GameData::CreateColony(Alien ^sp, String ^name, StarSystem ^system, int plNum, bool isObserver)
{
    return m_CurrentTurnData->CreateColony(sp, name, system, plNum, isObserver);
}

void GameData::RemoveColony(String ^name)
{
    return m_CurrentTurnData->RemoveColony(name);
}

Ship^ GameData::CreateShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    return m_CurrentTurnData->CreateShip(sp, type, name, subLight, system);
}

void GameData::AddWormholeJump(String ^shipName, int fromSystemId)
{
    m_CurrentTurnData->AddWormholeJump(shipName, fromSystemId);
}

void GameData::AddMishap(String ^shipName)
{
    m_CurrentTurnData->AddMishap(shipName);
}

void GameData::SetAtmosphereReq(GasType gas, int reqMin, int reqMax)
{
    AtmosphericReq ^atm = Player->AtmReq;

    if( (atm->GasRequired != GAS_MAX && atm->GasRequired != gas) ||
        atm->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->GasRequired = gas;
    atm->ReqMin = reqMin;
    atm->ReqMax = reqMax;
}

void GameData::SetAtmosphereNeutral(GasType gas)
{
    AtmosphericReq ^atm = Player->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    AtmosphericReq ^atm = Player->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Neutral[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Poisonous[gas] = true;
}

} // end namespace FHUI
