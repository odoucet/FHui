#include "StdAfx.h"
#include "TurnData.h"
#include "GameData.h"

#include "Commands.h"

namespace FHUI
{

TurnData::TurnData(int turn)
    : m_Turn(turn)
    , m_TurnEUStart(0)
    , m_TurnEUProduced(0)
    , m_FleetCost(0)
    , m_FleetCostPercent(0)
    , m_Aliens(gcnew SortedList<String^, Alien^>)
    , m_Systems(gcnew SortedList<int, StarSystem^>)
    , m_Colonies(gcnew SortedList<String^, Colony^>)
    , m_PlanetNames(gcnew SortedList<String^, PlanetName^>)
    , m_Ships(gcnew SortedList<String^, Ship^>)
{
}

String^ TurnData::GetSummary()
{
    return String::Format(
        "{0}"
        "---------------------------\r\n"
        "{1}"
        "---------------------------\r\n"
        "{2}"
        "---------------------------\r\n"
        "{3}"
        "---------------------------\r\n"
        "{4}"
        "---------------------------\r\n"
        "{5}"
        "---------------------------",
        GetSpeciesSummary(),
        GetAllTechsSummary(),
        GetEconomicSummary(),
        GetAliensSummary(),
        GetShipsSummary(),
        GetPlanetsSummary() );
}

String^ TurnData::GetSpeciesSummary()
{
    String ^toxicGases = "";
    for( int gas = 0; gas < GAS_MAX; ++gas )
    {
        if( GameData::AtmReq->Poisonous[gas] )
            toxicGases = toxicGases + String::Format(",{0}", FHStrings::GasToString(static_cast<GasType>(gas)));
    }

    return String::Format(
        "Species: {0}\r\n"
        "Home: [{1} {2} {3} {4}]\r\n"
        "Temp:{5} Press:{6}  Atm:{7} {8}-{9}%\r\n"
        "Poisons:{10}\r\n",
        GameData::Player->Name,
        GameData::Player->HomeSystem->X,
        GameData::Player->HomeSystem->Y,
        GameData::Player->HomeSystem->Z,
        GameData::Player->HomePlanet,
        GameData::AtmReq->TempClass == -1 ? "??" : GameData::AtmReq->TempClass.ToString(),
        GameData::AtmReq->PressClass == -1 ? "??" : GameData::AtmReq->PressClass.ToString(),
        FHStrings::GasToString( GameData::AtmReq->GasRequired ),
        GameData::AtmReq->ReqMin,
        GameData::AtmReq->ReqMax,
        toxicGases->Substring(1));
}

String^ TurnData::GetAllTechsSummary()
{
    return String::Format(
        "  MI: {0} | GV: {3}\r\n"
        "  MA: {1} | LS: {4}\r\n"
        "  ML: {2} | BI: {5}\r\n",
        GetTechSummary(TECH_MI),
        GetTechSummary(TECH_MA),
        GetTechSummary(TECH_ML),
        GetTechSummary(TECH_GV),
        GetTechSummary(TECH_LS),
        GetTechSummary(TECH_BI) );
}

String^ TurnData::GetEconomicSummary()
{
    return String::Format(
        "EUs  carried: {0,7}\r\n"
        "    produced: {1,7}\r\n"
        "Fleet maint.: {2} ({3}.{4}%)\r\n"
        "     Budget : {5,7}\r\n",
        m_TurnEUStart,
        m_TurnEUProduced,
        m_FleetCost, m_FleetCostPercent/100, m_FleetCostPercent%100,
        m_TurnEUProduced + m_TurnEUStart - m_FleetCost);
}

String^ TurnData::GetTechSummary(TechType tech)
{
    int lev = GameData::Player->TechLevels[tech];
    int levTeach = GameData::Player->TechLevelsTeach[tech];

    if( lev != levTeach )
    {
        return String::Format("{0,2}/{1,2}", lev, levTeach);
    }

    return String::Format("{0,2}   ", lev);
}

String^ TurnData::GetAliensSummary()
{
    String ^ret = "";

    if( GetAliens()->Count == 1 )
        return "No alien races met\r\n";

    List<Alien^> ^aliens = gcnew List<Alien^>;
    for each( Alien ^alien in GetAliens() )
    {
        if( alien != GameData::Player &&
            alien->Relation != SP_PIRATE )
        {
            aliens->Add(alien);
        }
    }

    aliens->Sort( gcnew Alien::RelationComparer );

    for each( Alien ^alien in aliens )
    {
        String ^prefix = "N";
        if( alien->Relation == SP_ALLY )
            prefix = "A";
        else if( alien->Relation == SP_ENEMY )
            prefix = "E";

        ret = String::Format("{0}{1}: SP {2}\r\n",
            ret, prefix, alien->Name );
    }

    return ret;
}

String^ TurnData::GetPlanetsSummary()
{
    String ^ret = "";

    for each( StarSystem ^system in GetStarSystems() )
    {
        for each( Planet ^planet in system->Planets->Values )
        {
            if( String::IsNullOrEmpty(planet->Name) )
                continue;

            ret += String::Format("PL {0} @ {1}\r\n",
                planet->Name,
                planet->PrintLocation() );
        }
    }

    return ret;
}

String^ TurnData::GetShipsSummary()
{
    List<Ship^> ^ships = GetShips(GameData::Player);
    if( ships->Count == 0 )
        return "You have no ships\r\n";

    ships->Sort( gcnew Ship::LocationComparer );

    String ^ret = "";
    for each( Ship ^ship in ships )
        ret += String::Format("{0} {1} @{2}\r\n",
            ship->PrintClass(),
            ship->Name,
            ship->PrintLocation() );

    return ret;
}

int TurnData::GetFleetCost()
{
    return m_FleetCost;
}

int TurnData::GetFleetPercentCost()
{
    return m_FleetCostPercent;
}

Alien^ TurnData::GetAlien(String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
        return m_Aliens[spKey];

    throw gcnew FHUIDataIntegrityException(
        String::Format("Species not found: {0}", sp));
}

Ship^ TurnData::GetShip(String ^name)
{
    return m_Ships[name->ToLower()];
}

List<Ship^>^ TurnData::GetShips(StarSystem ^sys, Alien ^sp)
{
    List<Ship^> ^ret = gcnew List<Ship^>; 

    for each( Ship ^ship in GetShips() )
    {
        if( sp && ship->Owner != sp )
            continue;
        if( sys && ship->System != sys )
            continue;
        ret->Add(ship);
    }

    return ret;
}

StarSystem^ TurnData::GetStarSystem(String ^name)
{
    if ( m_Colonies->ContainsKey( name->ToLower() ) )
    {
        return m_Colonies[name->ToLower()]->System;
    }
    else if ( m_PlanetNames->ContainsKey( name->ToLower() ) )
    {
        return m_PlanetNames[name->ToLower()]->System;
    }
    else
    {
        return nullptr;
    }
}

Colony^ TurnData::GetColony(String ^name)
{
    return m_Colonies[name->ToLower()];
}

List<Colony^>^ TurnData::GetColonies(StarSystem ^sys, Alien ^sp)
{
    List<Colony^> ^ret = gcnew List<Colony^>; 

    for each( Colony ^colony in GetColonies() )
    {
        if( sp && colony->Owner != sp )
            continue;
        if( sys && colony->System != sys )
            continue;
        ret->Add(colony);
    }

    return ret;
}

void TurnData::SetSpecies(String ^sp)
{
    if( GameData::Player == nullptr )
    {
        AddAlien(sp)->Relation = SP_PLAYER;
    }
    else if( String::Compare(GameData::Player->Name, sp) != 0 )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Reports for different species: {0} and {1}", GameData::Player->Name, sp) );
    }
}

Alien^ TurnData::AddAlien(String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        return m_Aliens[spKey];
    }
    Alien ^alien = gcnew Alien(sp, m_Turn);
    m_Aliens->Add(spKey, alien);
    return alien;
}

void TurnData::SetAlienRelation(String ^sp, SPRelType rel)
{
    AddAlien(sp)->Relation = rel;
}

void TurnData::SetTechLevel(Alien ^sp, TechType tech, int lev, int levTeach)
{
    sp->TechEstimateTurn        = m_Turn;
    sp->TechLevels[tech]        = lev;
    sp->TechLevelsAssumed[tech] = lev;
    sp->TechLevelsTeach[tech]   = levTeach;
}

void TurnData::SetFleetCost(int cost, int percent)
{
    m_FleetCost = cost;
    m_FleetCostPercent = percent;
}

StarSystem^ TurnData::GetStarSystem(int x, int y, int z)
{
    if ( m_Systems->ContainsKey( GameData::GetSystemId(x,y,z) ) )
    {
        return m_Systems[ GameData::GetSystemId(x,y,z) ];
    }
    else
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Trying to access unknown star system: {0} {1} {2}", x, y, z) );
    }
}

void TurnData::AddStarSystem(StarSystem^ system)
{
    m_Systems->Add( GameData::GetSystemId(system->X, system->Y, system->Z), system );
}

void TurnData::AddAlien(Alien^ alien)
{
    m_Aliens->Add( alien->Name->ToLower(), alien );
}

void TurnData::AddColony(Colony^ colony)
{
    m_Colonies->Add( colony->Name, colony );
}

StarSystem^ TurnData::AddStarSystem(int x, int y, int z, String ^type, String ^comment)
{
    int systemId = GameData::GetSystemId(x,y,z);
    if ( m_Systems->ContainsKey( systemId ) )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Star system already exists: [{0} {1} {2}]", x, y, z) );
    }

    StarSystem ^system = gcnew StarSystem(x, y, z, type);
    system->Comment = comment;
    system->IsVoid = false;
    m_Systems->Add( systemId, system );

    return system;
}

void TurnData::AddPlanetScan(StarSystem ^system, Planet ^planet)
{
    if( system->Planets->ContainsKey(planet->Number) )
    {
        Planet ^plExisting = system->Planets[planet->Number];
        if( !String::IsNullOrEmpty(plExisting->Comment) )
        {
            if( String::IsNullOrEmpty(planet->Comment) )
                planet->Comment = plExisting->Comment;
            else
                planet->Comment += "; " + plExisting->Comment;
        }
        //TODO: WTF? WHY DOES IT CRASH???
        //planet->SuspectedColonies = plExisting->SuspectedColonies;

        for each( Colony ^colony in m_Colonies->Values )
            if( colony->Planet == plExisting )
                colony->Planet = planet;
    }
    else
    {
        system->Planets->Add(planet->Number, planet);
    }
    system->TurnScanned = m_Turn;
}

void TurnData::SetTurnStartEU(int eu)
{
    m_TurnEUStart = eu;
}

void TurnData::AddTurnProducedEU(int eu)
{
    m_TurnEUProduced += eu;
}

Colony^ TurnData::AddColony(Alien ^sp, String ^name, StarSystem ^system, int plNum)
{
    if( m_Colonies->ContainsKey( name->ToLower() ) )
    {
        return m_Colonies[name->ToLower()];
    }
    else
    {
        Colony ^colony = gcnew Colony(sp, name, system, plNum);
        m_Colonies->Add(name->ToLower(), colony);

        if( ! system->Planets->ContainsKey(plNum) )
        {
            // System is not yet known, initialize with defaults
            system->Planets->Add(plNum, Planet::Default(system, plNum) );
        }
        colony->Planet = system->Planets[plNum];
        //TODO: WTF? WHY DOES IT CRASH???
        //if( colony->Planet->SuspectedColonies->ContainsKey(sp) )
        //    colony->Planet->SuspectedColonies->Remove(sp);
        if( sp == GameData::Player )
        {
            DeleteAlienColonies(system);
        }
        return colony;
    }
}

void TurnData::AddPlanetName(StarSystem ^system, int pl, String ^name)
{
    m_PlanetNames[name->ToLower()] = gcnew PlanetName(system, pl, name);
}

void TurnData::Update()
{
    // Don't update turn data for "turn 0"
    // because Player and related structures in GameData
    // are not yet initialized
    if( m_Turn > 0 )
    {
        UpdateShips();
        UpdateAliens();
        UpdateSystems();
        LinkPlanetNames();
        UpdateHomeWorlds();
        UpdateColonies();
    }
}

void TurnData::UpdateShips()
{
    m_ShipsByTonnage = gcnew List<Ship^>;
    m_ShipsByTonnage->AddRange(m_Ships->Values);
    m_ShipsByTonnage->Sort( gcnew Ship::WarTonnageComparer );
}

void TurnData::UpdateAliens()
{
    for each( Alien ^alien in GetAliens() )
    {
        alien->RelationOriginal = alien->Relation;

        alien->Colonies = GetColonies(nullptr, alien);
        alien->Ships    = GetShips(nullptr, alien);
    }
}

void TurnData::UpdateSystems()
{
    for each( Colony ^colony in GetColonies() )
    {
        colony->System->AddColony(colony);
    }

    for each( Ship ^ship in GetShips() )
    {
        ship->System->AddShip(ship);
    }

    for each( StarSystem ^system in GetStarSystems() )
    {
        // Calculate LSN
        int minLSN = 99999;
        int minLSNAvail = 99999;
        for each( Planet ^planet in system->Planets->Values )
        {
            planet->LSN = planet->CalculateLSN();
            planet->NumColonies = 0;
            planet->NumColoniesOwned = 0;

            // Update min LSN and num colonies
            minLSN = Math::Min(planet->LSN, minLSN);
            bool available = true;
            for each( Colony^ colony in system->Colonies )
            {
                if( colony->PlanetNum == planet->Number )
                {
                    ++planet->NumColonies;
                    if( colony->Owner == GameData::Player )
                        ++planet->NumColoniesOwned;
                    available = false;

                    // Update planet info with colony info, if there is any difference
                    // Helps with MD changed via mining or when there is no system scan
                    if ( colony->Owner == GameData::Player &&
                         colony->EconomicBase > 0 )
                    {
                        colony->CalculateBalance(false);

                        planet->MiDiff = colony->MiDiff;
                        planet->LSN = colony->LSN;
                    }

                    break;
                }
            }
            if( available )
                minLSNAvail = Math::Min(planet->LSN, minLSNAvail);
        }

        system->MinLSN      = minLSN;
        system->MinLSNAvail = minLSNAvail;

        system->UpdateMaster();
    }
}

void TurnData::LinkPlanetNames()
{
    for each( Colony ^colony in GetColonies() )
    {
        StarSystem ^system = colony->System;
        colony->Planet = system->Planets[colony->PlanetNum];

        if( colony->Owner == GameData::Player )
        {
            if( colony->Planet == nullptr )
                throw gcnew FHUIDataIntegrityException(
                    String::Format("Colony PL {0} at [{1} {2} {3} {4}]: Planet not scanned!",
                        colony->Name,
                        system->X, system->Y, system->Z,
                        colony->PlanetNum ) );

            colony->Planet->Name = colony->Name;
        }
    }

    for each( PlanetName ^plName in m_PlanetNames->Values )
    {
        if( plName->System->Planets->ContainsKey( plName->PlanetNum ) )
        {
            plName->System->Planets[plName->PlanetNum]->Name = plName->Name;
        }
    }
}

void TurnData::UpdateHomeWorlds()
{
    for each( Colony ^colony in GetColonies() )
    {
        if( colony->PlanetType == PLANET_HOME )
        {
            if( GameData::AtmReq->IsValid() )
                continue;

            if( colony->System->Planets->ContainsKey( colony->PlanetNum ) )
            {
                Planet ^planet = colony->System->Planets[colony->PlanetNum];
                GameData::AtmReq->TempClass = planet->TempClass;
                GameData::AtmReq->PressClass = planet->PressClass;
                for( int i = 0; i < GAS_MAX; ++i )
                {
                    if( planet->Atmosphere[i] )
                        GameData::AtmReq->Neutral[i] = true;
                }
            }
        }
    }
}

void TurnData::UpdateColonies()
{
    int order = 0;

    // Mining and resort colonies first
    for each( Colony ^colony in GameData::Player->Colonies )
        if( colony->PlanetType == PLANET_COLONY_MINING ||
            colony->PlanetType == PLANET_COLONY_RESORT )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = false;
        }

    // Colony planets
    for each( Colony ^colony in GameData::Player->Colonies )
        if( colony->PlanetType == PLANET_COLONY )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
        }

    // Home planet
    for each( Colony ^colony in GameData::Player->Colonies )
        if( colony->PlanetType == PLANET_HOME )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
        }

    GameData::Player->SortColoniesByProdOrder();
}

Ship^ TurnData::AddShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    Ship ^ship = gcnew Ship(sp, type, name, subLight);
    ship->System = system;
    m_Ships[name->ToLower()] = ship;

    if( sp == GameData::Player )
    {
        DeleteAlienColonies(system);
    }
    return ship;
}

void TurnData::DeleteAlienColonies(StarSystem^ system)
{
    // Remove alien colonies from this system.
    // They'll be restored by 'Aliens at...' parsing. If not - it means that alien
    // colony was destroyed, assimilated or it is hiding. Add a note.
    bool bRemoved = false;
    do
    {
        bRemoved = false;
        for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
        {
            Colony ^c = iter->Value;
            if( c->Owner != GameData::Player &&
                c->System == system )
            {
                //TODO: WTF? WHY DOES IT CRASH???
                //c->Planet->SuspectedColonies[c->Owner] = c->LastSeen;

                m_Colonies->Remove(iter->Key);
                bRemoved = true;
                break; // for each
            }
        }
    } while( bRemoved );
}

} // end namespace FHUI
