#include "StdAfx.h"
#include "TurnData.h"
#include "GameData.h"

#include "Commands.h"

using namespace System::Diagnostics;

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
    //, m_Colonies(gcnew SortedList<String^, Colony^>)
    //, m_Ships(gcnew SortedList<String^, Ship^>)
    , m_WormholeJumps(gcnew List<WormholeJump^>)
    , m_Misjumps(gcnew List<String^>)
    , m_bParsingFinished(false)
    //, m_ShipsByTonnage(gcnew List<Ship^>)
{
}

void TurnData::PrintDebugStats()
{
    List<String^>^ stats = gcnew List<String^>;

#define STAT(descr, num) \
    stats->Add( String::Format( "{0,-20} {1}", descr, num ) );

    int knownSystems = 0;
    int knownPlanets = 0;
    int playerColonies = 0;
    int alienColonies = 0;
    int wormholes = 0;
    int ships = 0;

    for each (StarSystem^ system in GetStarSystems() )
    {
        knownSystems += system->IsExplored() ? 1 : 0;
        wormholes += system->HasWormhole ? 1 : 0;
        knownPlanets += system->Planets->Count;
        playerColonies += system->ColoniesOwned->Count;
        alienColonies += system->ColoniesAlien->Count;
        ships += system->Ships->Count;
    }

    STAT( "Aliens", m_Aliens->Count );
    STAT( "Systems", m_Systems->Count );
    STAT( "  Known", knownSystems );
    STAT( "  Planets", knownPlanets );
    STAT( "  With Wormhole", wormholes );
    STAT( "Colonies", playerColonies + alienColonies );
    STAT( "  Player", playerColonies );
    STAT( "  Alien", alienColonies );
    STAT( "Ships", ships );

    for each (String^ line in stats)
    {
        Debug::WriteLine( line );
    }
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
    Alien ^player = GameData::Player;

    String ^toxicGases = "";
    for( int gas = 0; gas < GAS_MAX; ++gas )
    {
        if( player->AtmReq->Poisonous[gas] )
            toxicGases = toxicGases + String::Format(",{0}", FHStrings::GasToString(static_cast<GasType>(gas)));
    }

    return String::Format(
        "Species: {0}\r\n"
        "Home: [{1} {2} {3} {4}]\r\n"
        "Temp:{5} Press:{6}  Atm:{7} {8}-{9}%\r\n"
        "Poisons:{10}\r\n",
        player->Name,
        player->HomeSystem->X,
        player->HomeSystem->Y,
        player->HomeSystem->Z,
        player->HomePlanet,
        player->AtmReq->TempClass == -1 ? "??" : player->AtmReq->TempClass.ToString(),
        player->AtmReq->PressClass == -1 ? "??" : player->AtmReq->PressClass.ToString(),
        FHStrings::GasToString( player->AtmReq->GasRequired ),
        player->AtmReq->ReqMin,
        player->AtmReq->ReqMax,
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

    for each( Colony ^colony in GameData::Player->Colonies )
    {
        ret += String::Format("PL {0} @ {1}\r\n",
            colony->Name,
            colony->Planet->PrintLocation() );
    }

    return ret;
}

String^ TurnData::GetShipsSummary()
{
    if( GameData::Player->Ships->Count == 0 )
        return "You have no ships\r\n";

    GameData::Player->Ships->Sort( gcnew Ship::LocationComparer );

    String ^ret = "";
    for each( Ship ^ship in GameData::Player->Ships )
        ret += String::Format("{0} {1} @{2}\r\n",
            ship->PrintClass(),
            ship->Name,
            ship->PrintLocation() );

    GameData::Player->Ships->Sort( gcnew Ship::WarTonnageComparer );

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

StarSystem^ TurnData::GetStarSystem(String ^name)
{
    //if ( m_Colonies->ContainsKey( name->ToLower() ) )
    //{
    //    return m_Colonies[name->ToLower()]->System;
    //}
    //else
    {
        return nullptr;
    }
}

void TurnData::SetSpecies(String ^sp)
{
    Alien ^alien = GameData::Player;
    if( String::Compare(GameData::Player->Name, sp) != 0 )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Reports for different species: {0} and {1}", GameData::Player->Name, sp) );
    }
    else
    {
        alien->Relation = SP_PLAYER;
    }
}

Alien^ TurnData::AddAlien(String ^sp)
{
    String ^spKey = sp->ToLower();
    Alien ^alien;
    if( m_Aliens->ContainsKey(spKey) )
    {
        alien = m_Aliens[spKey];
    }
    else
    {
        alien = gcnew Alien(sp);
        m_Aliens->Add(spKey, alien);
    }

    if( m_bParsingUserContent == false && alien->TurnMet == 0 )
        alien->TurnMet = m_Turn;

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

StarSystem^ TurnData::GetStarSystem(int id)
{
    if ( m_Systems->ContainsKey( id ) )
    {
        return m_Systems[ id ];
    }

    throw gcnew FHUIDataIntegrityException(
        "Trying to access unknown star system: id=" + id.ToString() );
}

StarSystem^ TurnData::GetStarSystem(int x, int y, int z, bool allowVoid)
{
    try
    {
        if( allowVoid )
        {
            int id = GameData::GetSystemId(x, y, z);
            if ( m_Systems->ContainsKey( id ) )
                return m_Systems[ id ];


            StarSystem ^system = AddStarSystem(x, y, z, "void", "");
            system->IsVoid = true;
            return system;
        }
        else
            return GetStarSystem( GameData::GetSystemId(x, y, z) );
    }
    catch( FHUIDataIntegrityException^ )
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
    colony->Owner->Colonies->Add( colony );
    colony->System->AddColony( colony );
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
        system->Planets[planet->Number] = planet;

        if( !String::IsNullOrEmpty(plExisting->Comment) )
        {
            if( String::IsNullOrEmpty(planet->Comment) )
                planet->Comment = plExisting->Comment;
            else
                planet->Comment += "; " + plExisting->Comment;
        }
        if( !String::IsNullOrEmpty(plExisting->AlienName) &&
            String::IsNullOrEmpty(planet->AlienName) )
            planet->AlienName = plExisting->AlienName;

        for each( Colony ^colony in system->Colonies )
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

Colony^ TurnData::CreateColony(Alien ^sp, String ^name, StarSystem ^system, int plNum, bool isObserver)
{
    for each( Colony ^colony in sp->Colonies )
    {
        if( colony->System == system &&
            colony->PlanetNum == plNum )
        {
            return colony;
        }
    }

    Colony ^colony = gcnew Colony(sp, name, system, plNum);

    if( !system->Planets->ContainsKey(plNum) )
    {   // System is not yet known, initialize with defaults
        system->Planets->Add(plNum, Planet::Default(system, plNum) );
    }
    colony->Planet = system->Planets[plNum];
    if( GameData::Player && ( sp == GameData::Player ) )
    {
        colony->Planet->Name = name;
        colony->PlanetType = PLANET_COLONY;
        // for player colonies these are known and should never be -1
        colony->MiBase = 0;
        colony->MaBase = 0;
        colony->Shipyards = 0;
    }
    else
    {
        colony->Planet->AlienName = name;
    }

    // Add colony after setting all properties
    AddColony(colony);

    if( isObserver && (sp == GameData::Player ) )
    {
        system->LastVisited = m_Turn;
        system->DeleteAlienColonies();
    }
    return colony;
}

void TurnData::RemoveColony(String ^name)
{
    // Assume only player's colony may be deleted
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        if( colony->Name == name )
        {
            GameData::Player->Colonies->Remove(colony);
            colony->System->RemoveColony( colony );
            return;
        }
    }

    throw gcnew FHUIDataIntegrityException("Trying to delete colony, but player doesn't have one: " + name);
}

void TurnData::Update()
{
    // Don't update turn data for "turn 0"
    // because Player and related structures in GameData
    // are not yet initialized
    if( m_Turn > 0 )
    {
        m_bParsingFinished = true;

        UpdateShips();
        UpdateAliens();
        UpdateSystems();
        UpdateHomeWorlds();
        UpdateColonies();
    }
}

void TurnData::UpdateShips()
{
    // Back up original cargo
    for each( Ship ^ship in GameData::Player->Ships )
    {
        ship->Cargo->CopyTo(ship->CargoOriginal, 0);
    }

    // Update wormhole targets
    for each( WormholeJump ^jump in m_WormholeJumps )
    {   // The ship could have been intercepted
        Ship ^ship = GameData::Player->FindShip(jump->A, true);
        if( ship )
        {
            StarSystem ^from = GetStarSystem(jump->B);
            // Add wormhole link
            from->SetWormhole( ship->System->GetId() );
        }
    }

    // Update misjumped ships
    for each( String ^misjump in m_Misjumps )
    {   // The ship could have been intercepted
        Ship ^ship = GameData::Player->FindShip(misjump, true);
        if( ship )
            ship->HadMishap = true;
    }
}

void TurnData::UpdateAliens()
{
    for each( Alien ^alien in GetAliens() )
    {
        if( alien->TurnMet == 0 )
            alien->Relation = GameData::Player->RelationDefault;
    }
}

void TurnData::UpdateSystems()
{
    for each( Alien^ alien in GetAliens() )
    {
        if (alien->HomeSystem)
        {
            alien->HomeSystem->HomeSpecies = alien;
        }
    }

    for each( StarSystem ^system in GetStarSystems() )
    {
        // Update tooltips
        system->UpdateTooltip();

        // Calculate LSN
        int minLSN = 99999;
        int minLSNAvail = 99999;
        for each( Planet ^planet in system->Planets->Values )
        {
            planet->LSN = Calculators::LSN(planet, GameData::Player);
            planet->NumColonies = 0;
            planet->NumColoniesOwned = 0;

            bool available = true;
            for each( Colony^ colony in system->Colonies )
            {
                if( colony->PlanetNum == planet->Number )
                {
                    ++planet->NumColonies;
                    if( colony->Owner == GameData::Player )
                        ++planet->NumColoniesOwned;

                    if( colony->EconomicBase > 0 )
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
                }
            }

            // Update min LSN and num colonies
            minLSN = Math::Min(planet->LSN, minLSN);

            if( available )
                minLSNAvail = Math::Min(planet->LSN, minLSNAvail);
        }

        system->MinLSN      = minLSN;
        system->MinLSNAvail = minLSNAvail;
    }
}

void TurnData::UpdateHomeWorlds()
{
    for each( Alien ^alien in GetAliens() )
    {
        if( alien->HomeSystem )
        {
            alien->HomeSystem->CommentHome = String::Format("HOME of SP {0} [#{1}]",
                alien->Name,
                alien->HomePlanet);
        }
    }

    for each( StarSystem ^system in GetStarSystems() )
    {
        for each( Colony ^colony in system->Colonies )
        {
            if( colony->PlanetType == PLANET_HOME && 
                system->Planets->ContainsKey( colony->PlanetNum ) )
            {
                Planet ^planet = system->Planets[colony->PlanetNum];

                // Find species born here
                for each( Alien ^alien in GetAliens() )
                {
                    if( alien->HomeSystem &&
                        alien->HomeSystem == system &&
                        alien->HomePlanet == colony->PlanetNum )
                    {
                        // Update species' living requirements, if unknown
                        if ( ! alien->AtmReq->IsValid() )
                        {
                            alien->AtmReq->TempClass = planet->TempClass;
                            alien->AtmReq->PressClass = planet->PressClass;
                            for( int i = 0; i < GAS_MAX; ++i )
                            {
                                if( planet->Atmosphere[i] )
                                    alien->AtmReq->Neutral[i] = true;
                            }
                        }
                    }
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
        if( ( colony->PlanetType == PLANET_COLONY_MINING ||
              colony->PlanetType == PLANET_COLONY_RESORT ) &&
            ( colony->EconomicBase > 0 ) )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = false;
            colony->ProductionReset();
        }

    // Colony planets
    for each( Colony ^colony in GameData::Player->Colonies )
        if( colony->PlanetType == PLANET_COLONY &&
            colony->EconomicBase > 0 )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
            colony->ProductionReset();
        }

    // Home planet
    for each( Colony ^colony in GameData::Player->Colonies )
        if( colony->PlanetType == PLANET_HOME &&
            colony->EconomicBase > 0 )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
            colony->ProductionReset();
            break;
        }

    GameData::Player->SortColoniesByProdOrder();
}

Ship^ TurnData::CreateShip(Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    Ship ^ship = gcnew Ship(sp, type, name, subLight);
    ship->System = system;

    system->AddShip( ship );
    sp->Ships->Add( ship );

    if( ( sp == GameData::Player ) &&
        ( m_bParsingFinished == false ) )
    {
        system->DeleteAlienColonies();
    }

    return ship;
}

void TurnData::RemoveShip(Ship ^ship)
{
    ship->System->RemoveShip( ship );
    GameData::Player->Ships->Remove( ship );
}

void TurnData::AddWormholeJump(String ^shipName, int fromSystemId)
{
    m_WormholeJumps->Add( gcnew WormholeJump(shipName, fromSystemId) );
}

void TurnData::AddMishap(String ^shipName)
{
    m_Misjumps->Add( shipName );
}

} // end namespace FHUI
