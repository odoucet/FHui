#include "StdAfx.h"
#include "GameData.h"

// ---------------------------------------------------------

String^ PrintInventory(array<int> ^inv)
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
                InvToString(static_cast<InventoryType>(i)) );
        }
    }

    return ret;
}

// ---------------------------------------------------------

String^ Alien::PrintHome()
{
    if( HomeSystem == nullptr )
        return "???";
    String ^plName = nullptr;
    if( HomePlanet != -1 &&
        HomePlanet <= HomeSystem->PlanetsCount )
    {
        String ^n = HomeSystem->GetPlanets()[HomePlanet - 1]->Name;
        if( String::IsNullOrEmpty(n) )
            plName = "-";
        else
            plName = String::Format("PL {0}", n);
    }
    if( plName == nullptr )
    {
        plName = "?";
    }
    return String::Format( "{0} {1} ({2})",
        HomeSystem->PrintLocation(), HomePlanet, plName );
}

String^ Alien::PrintTechLevels()
{
    if( TechEstimateTurn == -1 )
        return "No estimates.";

    return String::Format(
        "Turn {6}: MI:{0} MA:{1} ML:{2} GV:{3} LS:{4} BI:{5}",
        TechLevels[TECH_MI],
        TechLevels[TECH_MA],
        TechLevels[TECH_ML],
        TechLevels[TECH_GV],
        TechLevels[TECH_LS],
        TechLevels[TECH_BI],
        TechEstimateTurn );
}

// ---------------------------------------------------------

int Planet::CalculateLSN(AtmosphericReq ^atmReq)
{
    int lsn =
        3 * Math::Abs(PressClass - atmReq->PressClass) +
        3 * Math::Abs(TempClass - atmReq->TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( m_Atmosphere[i] > 0 && atmReq->Poisonous[i] )
            lsn += 3;
    }

    if( m_Atmosphere[atmReq->GasRequired] < atmReq->ReqMin ||
        m_Atmosphere[atmReq->GasRequired] > atmReq->ReqMax )
    {
        lsn += 3;
    }

    return lsn;
}

// ---------------------------------------------------------

int StarSystem::GetMinLSN()
{
    int minLSN = 99999;
    for each( Planet ^pl in m_Planets )
        minLSN = Math::Min(minLSN, pl->LSN == -1 ? 99999 : pl->LSN);

    return minLSN;
}

Planet^ StarSystem::GetPlanet(int plNum)
{
    if( plNum < 1 )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Invalid planet number: {0}!", plNum) );

    if( plNum <= m_Planets->Length )
        return m_Planets[plNum - 1];

    return nullptr;
}

double StarSystem::CalcDistance(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo)
{
    int dx = Math::Abs(xFrom - xTo);
    int dy = Math::Abs(yFrom - yTo);
    int dz = Math::Abs(zFrom - zTo);
    return Math::Sqrt(dx * dx + dy * dy + dz * dz);
}

double StarSystem::CalcMishap(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo, int gv, int age)
{
    if( gv == 0 )
        throw gcnew FHUIDataIntegrityException("GV must not be 0 for mishap calculation!");

    double dist = CalcDistance(xFrom, yFrom, zFrom, xTo, yTo, zTo);
    if( dist == 0 )
        return 0.0;
    return (dist * dist) / gv + (age * 2.0);
}

String^ StarSystem::GenerateScan()
{
    String ^scan = String::Format(
        "Coordinates: x = {0}  y = {1}  z = {2}  stellar type = {3}  {4} planets.\r\n\r\n"
        "               Temp  Press Mining\r\n"
        "  #  Dia  Grav Class Class  Diff  LSN  Atmosphere\r\n"
        " ---------------------------------------------------------------------\r\n",
        X, Y, Z, Type,
        m_Planets->Length == 0 ? "?" : m_Planets->Length.ToString() );

    for( int i = 0; i < m_Planets->Length; ++i )
    {
        Planet ^planet = m_Planets[i];
        String ^plStr = String::Format(
            "{0,3}{1,5}{2,7:F2}{3,4}{4,6}{5,8:F2}{6,5}  ",
            i + 1,
            planet->Diameter,
            planet->Grav,
            planet->TempClass,
            planet->PressClass,
            planet->MiningDiff,
            planet->LSN );
        bool anyGas = false;
        for( int gas = 0; gas < GAS_MAX; ++gas )
        {
            if( planet->Atmosphere[gas] )
            {
                plStr = String::Concat( plStr,
                    String::Format(
                        "{0}{1}({2}%)",
                        anyGas ? "," : "",
                        GasToString(static_cast<GasType>(gas)),
                        planet->Atmosphere[gas] ) );
                anyGas = true;
            }
        }
        if( !anyGas )
            plStr = String::Concat(plStr, "No atmosphere");

        plStr = String::Concat(plStr, "\r\n");
        scan  = String::Concat(scan, plStr);
    }

    return scan;
}

String^ StarSystem::PrintScanTurn()
{
    if( TurnScanned == -1 )       return "Not scanned";
    else if( TurnScanned == 0 )   return "Received";
    else                            return String::Format("Scanned, {0}", TurnScanned);
}

void StarSystem::Planets::set(int plNum, Planet ^pl)
{
    Array::Resize(m_Planets, Math::Max(plNum + 1, m_Planets->Length));
    m_Planets[plNum] = pl;
}

// ---------------------------------------------------------

String^ Colony::PrintInventoryShort()
{
    return PrintInventory(m_Inventory);
}

// ---------------------------------------------------------

String^ Ship::PrintClass()
{
    return String::Format(
        "{0}{1}{2}",
        ShipToString( Type ),
        Type == SHIP_TR ? Size.ToString() : "",
        SubLight ? "s" : "" );
}

String^ Ship::PrintLocation()
{
    String ^xyz;

    if( System )
    {
        if( PlanetNum != -1 )
        {
            Planet ^planet = System->Planets[PlanetNum - 1];
            if( planet != nullptr &&
                !String::IsNullOrEmpty(planet->Name) )
            {
                xyz = String::Format("PL {0}", planet->Name);
            }
            else
                xyz = String::Format("[{0} {1}]", System->PrintLocation(), PlanetNum);
        }
        else
           xyz = String::Format("[{0}]", System->PrintLocation());
    }
    else
    {
        if( PlanetNum != -1 )
            xyz = String::Format("[{0,2} {1,2} {2,2} {3}]", X, Y, Z, PlanetNum);
        else
            xyz = String::Format("[{0,2} {1,2} {2,2}]", X, Y, Z);
    }

    switch( Location )
    {
    case SHIP_LOC_DEEP_SPACE:   return String::Format("{0}, DeepSp", xyz);
    case SHIP_LOC_ORBIT:        return String::Format("{0}, Orbit", xyz);
    case SHIP_LOC_LANDED:       return String::Format("{0}, Landed", xyz);
    }

    int l = Location;
    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid ship location! ({0})", l) );
}

String^ Ship::PrintCargo()
{
    return PrintInventory(m_Cargo);
}

void Ship::CalculateCapacity()
{
    switch( Type )
    {
    case SHIP_BAS:  Capacity = Size / 1000; break;
    case SHIP_TR:   Capacity = (10 + (Size / 2)) * Size; break;
    case SHIP_PB:   Capacity = 1;       break;
    case SHIP_CT:   Capacity = 2;       break;
    case SHIP_ES:   Capacity = 5;       break;
    case SHIP_FF:   Capacity = 10;      break;
    case SHIP_DD:   Capacity = 15;      break;
    case SHIP_CL:   Capacity = 20;      break;
    case SHIP_CS:   Capacity = 25;      break;
    case SHIP_CA:   Capacity = 30;      break;
    case SHIP_CC:   Capacity = 35;      break;
    case SHIP_BC:   Capacity = 40;      break;
    case SHIP_BS:   Capacity = 45;      break;
    case SHIP_DN:   Capacity = 50;      break;
    case SHIP_SD:   Capacity = 55;      break;
    case SHIP_BM:   Capacity = 60;      break;
    case SHIP_BW:   Capacity = 65;      break;
    case SHIP_BR:   Capacity = 70;      break;
    }
}

// ---------------------------------------------------------

GameData::GameData(void)
    : m_Species(nullptr)
    , m_TurnEUStart(0)
    , m_TurnEUProduced(0)
    , m_FleetCost(0)
    , m_FleetCostPercent(0.0)
    , m_Aliens(gcnew SortedList<String^, Alien^>)
    , Systems(gcnew array<StarSystem^>(0))
    , m_Colonies(gcnew SortedList<String^, Colony^>)
    , m_PlanetNames(gcnew SortedList<String^, PlanetName^>)
    , m_Ships(gcnew SortedList<String^, Ship^>)
    , m_TurnMax(0)
{
}

// ---------------------------------------------------------

String^ GameData::GetSummary()
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
        "---------------------------\r\n",
        GetSpeciesSummary(),
        GetAllTechsSummary(),
        GetEconomicSummary(),
        GetAliensSummary(),
        GetPlanetsSummary(),
        GetShipsSummary() );
}

String^ GameData::GetSpeciesSummary()
{
    AtmosphericReq ^atm = m_Species->AtmReq;

    return String::Format(
        "Species: {0}\r\n"
        "Home: [{1} {2} {3} {4}]\r\n"
        "Temp:{5} Press:{6}  Atm:{7} {8}-{9}%\r\n",
        GetSpeciesName(),
        m_Species->HomeSystem->X,
        m_Species->HomeSystem->Y,
        m_Species->HomeSystem->Z,
        m_Species->HomePlanet,
        atm->TempClass == -1 ? "??" : atm->TempClass.ToString(),
        atm->PressClass == -1 ? "??" : atm->PressClass.ToString(),
        GasToString( atm->GasRequired ),
        atm->ReqMin,
        atm->ReqMax );
}

String^ GameData::GetAllTechsSummary()
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

String^ GameData::GetEconomicSummary()
{
    return String::Format(
        "EUs  carried: {0,7}\r\n"
        "    produced: {1,7}\r\n"
        "Fleet maint.: {2} ({3:F2}%)\r\n"
        "Production  : {4,7}\r\n",
        m_TurnEUStart,
        m_TurnEUProduced,
        m_FleetCost, m_FleetCostPercent,
        m_TurnEUProduced + m_TurnEUStart - m_FleetCost);
}

String^ GameData::GetTechSummary(TechType tech)
{
    int lev = m_Species->TechLevels[tech];
    int levTeach = m_Species->TechLevelsTeach[tech];

    if( lev != levTeach )
    {
        return String::Format("{0,2}/{1,2}", lev, levTeach);
    }

    return String::Format("{0,2}   ", lev);
}

String^ GameData::GetAliensSummary()
{
    String ^ret = "";

    for each( Alien ^alien in GetAliens() )
    {
        if( alien == m_Species ||
            alien->Relation == SP_PIRATE )
            continue;

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

String^ GameData::GetPlanetsSummary()
{
    return "Planets summary\r\n - TODO...\r\n";
    /*
    String ^ret = "";

    for each( DictionaryEntry ^entry in m_PlanetNames )
    {
        PlanetName ^plName = safe_cast<PlanetName^>(entry->Value);
        String ^prefix = "  ";
        //TODO: prefix: home, colony, alien, etc...
        ret = String::Format("{0}{1} PL {2} {3} {4} {5} {6}\r\n",
            ret, prefix, plName->Name,
            plName->X, plName->Y, plName->Z, plName->Num);
    }

    return ret;
    */
}

String^ GameData::GetShipsSummary()
{
    String ^ret = "";

    for each( Ship ^ship in GetShips() )
    {
        if( ship->Owner != m_Species )
            continue;

        ret = String::Format("{0}{1} {2} @{3}\r\n",
            ret,
            ship->PrintClass(),
            ship->Name,
            ship->PrintLocation() );
    }

    if( String::IsNullOrEmpty(ret) )
        ret = "You have no ships\r\n";

    return ret;
}

void GameData::GetFleetCost(int %cost, float %percent)
{
    cost = m_FleetCost;
    percent = m_FleetCostPercent;
}

Alien^ GameData::GetAlien(String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
        return m_Aliens[spKey];

    throw gcnew FHUIDataIntegrityException(
        String::Format("Species not found: {0}", sp));
}

Ship^ GameData::GetShip(String ^name)
{
    return safe_cast<Ship^>(m_Ships[name->ToLower()]);
}

Colony^ GameData::GetColony(String ^name)
{
    return safe_cast<Colony^>(m_Colonies[name->ToLower()]);
}

List<Colony^>^ GameData::GetColonies(StarSystem ^sys, Alien ^sp)
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

// ---------------------------------------------------------

bool GameData::TurnCheck(int turn)
{
    if( turn > m_TurnMax )
    {
        m_TurnMax = turn;

        // New turn cleanup
        for each( Alien ^alien in GetAliens() )
        {
            if( alien != m_Species && alien->Relation != SP_PIRATE )
                alien->Relation = SP_NEUTRAL; // FIXME: default relation may be different...
        }

        m_TurnEUStart       = 0;
        m_TurnEUProduced    = 0;
        m_FleetCost         = 0;
        m_FleetCostPercent  = 0.0;

        // Remove all player's colonies in case one of them no longer exists
        bool removed = false;
        do {
            removed = false;
            for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
            {
                if( iter->Value->Owner == m_Species )
                {
                    m_Colonies->Remove(iter->Key);
                    removed = true;
                    break;
                }
            }
        } while( removed );

        // remove planet names
        m_PlanetNames->Clear();
        // remove ships
        m_Ships->Clear();
    }
    return turn == m_TurnMax;
}

int GameData::TurnAlign(int turn)
{
    if( turn == 0 )
    {
        return TURN_NOTES;
    }
    return turn;
}

void GameData::SetSpecies(String ^sp)
{
    if( m_Species == nullptr )
    {
        m_Species = AddAlien(0, sp);
        m_Species->Relation = SP_PLAYER;
    }

    if( String::Compare(m_Species->Name, sp) != 0 )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Reports for different species: {0} and {1}",
                m_Species->Name, sp) );
}

Alien^ GameData::AddAlien(int turn, String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        Alien ^alien = m_Aliens[spKey];
        alien->TurnMet = Math::Min(TurnAlign(turn), alien->TurnMet);

        return alien;
    }

    Alien ^alien = gcnew Alien(sp, turn);
    m_Aliens->Add(spKey, alien);

    return alien;
}

void GameData::SetAlienRelation(int turn, String ^sp, SPRelType rel)
{
    if( TurnCheck(turn) )
    {
        Alien ^alien = AddAlien(turn, sp);
        alien->Relation = rel;
    }
}

void GameData::SetTechLevel(int turn, Alien ^sp, TechType tech, int lev, int levTeach)
{
    if( TurnCheck(turn) || sp->TechEstimateTurn == -1 )
    {
        m_TurnMax = turn;

        sp->TechEstimateTurn      = turn;
        sp->TechLevels[tech]      = lev;
        sp->TechLevelsTeach[tech] = levTeach;
    }
}

void GameData::SetFleetCost(int turn, int cost, float percent)
{
    if( TurnCheck(turn) )
    {
        m_FleetCost = cost;
        m_FleetCostPercent = percent;
    }
}

void GameData::SetAtmosphereReq(GasType gas, int reqMin, int reqMax)
{
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( (atm->GasRequired != GAS_MAX && atm->GasRequired != gas) ||
        atm->Neutral[gas] ||
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
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Neutral[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Poisonous[gas] = true;
}

StarSystem^ GameData::GetStarSystem(int x, int y, int z)
{
    for each( StarSystem ^system in Systems )
    {
        if( system->X == x &&
            system->Y == y &&
            system->Z == z )
        {
            return system;
        }
    }

    throw gcnew FHUIDataIntegrityException(
        String::Format("Trying to get not existing star system: {0} {1} {2}", x, y, z) );
}

void GameData::AddStarSystem(int x, int y, int z, String ^type, String ^comment)
{
    for each( StarSystem ^system in Systems )
    {
        if( system->X == x &&
            system->Y == y &&
            system->Z == z )
        {
            throw gcnew FHUIDataIntegrityException(
                String::Format("Star system already added: [{0} {1} {2}]", x, y, z) );
        }
    }

    StarSystem ^system = gcnew StarSystem(x, y, z, type);
    system->Comment = comment;
    Array::Resize(Systems, Systems->Length + 1);
    Systems[Systems->Length - 1] = system;
}

void GameData::AddPlanetScan(int turn, int x, int y, int z, Planet ^planet)
{
    StarSystem ^system = GetStarSystem(x, y, z);
    if( system == nullptr )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Star system [{0} {1} {2}] not found in galaxy list!", x, y, z) );

    int plNum = planet->Number - 1;
    if( system->PlanetsCount < planet->Number ||
        system->Planets[plNum] == nullptr ||
        turn == m_TurnMax )
    {
        if( system->PlanetsCount >= planet->Number &&
            system->Planets[plNum] != nullptr &&
            String::IsNullOrEmpty(planet->Comment) )
        {
            planet->Comment = system->Planets[plNum]->Comment;
        }
        system->Planets[plNum] = planet;
        system->TurnScanned   = Math::Max(system->TurnScanned, turn);
    }
}

void GameData::SetTurnStartEU(int turn, int eu)
{
    if( TurnCheck(turn) )
    {
        m_TurnEUStart = eu;
    }
}

void GameData::AddTurnProducedEU(int turn, int eu)
{
    if( TurnCheck(turn) )
    {
        m_TurnEUProduced += eu;
    }
}

Colony^ GameData::AddColony(int turn, Alien ^sp, String ^name, StarSystem ^system, int plNum)
{
    bool createColony = false;

    if( TurnCheck(turn) )
    {
        createColony = true;
    }

    if( createColony )
    {
        Colony ^colony = gcnew Colony(sp, name, system, plNum);
        m_Colonies[name->ToLower()] = colony;

        // If this is species' own colony, remove alien colonies from this system.
        // They'll be restored by 'Aliens at...' parsing. If not - it means that alien
        // colony was destroyed or assymilated.
        if( sp == m_Species )
        {
            bool bRemoved = false;
            do
            {
                bRemoved = false;
                for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
                {
                    Colony ^c = iter->Value;
                    if( c->Owner != m_Species &&
                        c->System == system &&
                        c->PlanetNum == plNum )
                    {
                        m_Colonies->Remove(iter->Key);
                        bRemoved = true;
                        break; // for each
                    }
                }
            } while( bRemoved );
        }

        return colony;
    }

    return nullptr;
}

void GameData::AddPlanetName(int turn, int x, int y, int z, int pl, String ^name)
{
    if( TurnCheck(turn) )
    {
        m_PlanetNames[name->ToLower()] = gcnew PlanetName(x, y, z, pl, name);
    }
}

void GameData::UpdatePlanets()
{
    CalculateLSN();
    LinkPlanetNames();
    UpdateHomeWorlds();
}

void GameData::CalculateLSN()
{
    for each( StarSystem ^system in Systems )
    {
        for each( Planet ^planet in system->GetPlanets() )
        {
            planet->LSN = planet->CalculateLSN(m_Species->AtmReq);
        }
    }
}

void GameData::LinkPlanetNames()
{
    for each( Colony ^colony in GetColonies() )
    {
        if( colony->Owner == m_Species )
        {
            StarSystem ^system = colony->System;

            if( system->GetPlanets()->Length < colony->PlanetNum )
                throw gcnew FHUIDataIntegrityException(
                    String::Format("Colony PL {0} at [{1} {2} {3} {4}]: Planet not scanned!",
                        colony->Name,
                        system->X, system->Y, system->Z,
                        colony->PlanetNum ) );

            colony->Planet = system->GetPlanets()[ colony->PlanetNum - 1 ];
            colony->Planet->Name = colony->Name;
        }
    }

    for each( PlanetName ^plName in m_PlanetNames->Values )
    {
        StarSystem ^system = GetStarSystem( plName->X, plName->Y, plName->Z );
        Planet ^planet = system->GetPlanet( plName->Num );
        if( planet )
            planet->Name = plName->Name;
    }
}

void GameData::UpdateHomeWorlds()
{
    for each( Colony ^colony in GetColonies() )
    {
        if( colony->PlanetType == PLANET_HOME )
        {
            Alien ^sp = colony->Owner;
            if( sp->AtmReq->IsValid() )
                continue;

            Planet ^planet = colony->System->GetPlanet(colony->PlanetNum);
            if( planet )
            {
                sp->AtmReq->TempClass = planet->TempClass;
                sp->AtmReq->PressClass = planet->PressClass;
                for( int i = 0; i < GAS_MAX; ++i )
                {
                    if( planet->Atmosphere[i] )
                        sp->AtmReq->Neutral[i] = true;
                }
            }
        }
    }
}

Ship^ GameData::AddShip(int turn, Alien ^sp, ShipType type, String ^name, int size, bool subLight)
{
    bool createShip = false;

    if( TurnCheck(turn) )
    {
        createShip = true;
    }
    else if( sp != m_Species )
    {   // Turn N-1 report scanned after Turn N report
        // and seeing an alien colony that wasn't seen next turn.
        /*
        //tbd: later, when history of seen ships is added this code may come in handy
        if( m_Colonies->ContainsKey(name->ToLower()) == false )
            createColony = true;
        */
    }

    if( createShip )
    {
        Ship ^ship = gcnew Ship(sp, type, name, size, subLight);
        m_Ships[name->ToLower()] = ship;
        return ship;
    }

    return nullptr;
}
