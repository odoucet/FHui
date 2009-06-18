#include "StdAfx.h"
#include "GameData.h"

// ---------------------------------------------------------

String^ Alien::PrintHome()
{
    if( m_HomeSystem == nullptr )
        return "???";
    String ^plName = nullptr;
    if( m_HomePlanet != -1 &&
        m_HomePlanet <= m_HomeSystem->m_Planets->Length )
        plName = m_HomeSystem->m_Planets[m_HomePlanet]->m_Name;
    if( plName == nullptr )
        plName = "?";
    return String::Format( "{0} {1} ({2})",
        m_HomeSystem->PrintLocation(), m_HomePlanet, plName );
}

// ---------------------------------------------------------

int Planet::CalculateLSN(AtmosphericReq ^atmReq)
{
    int lsn =
        3 * Math::Abs(m_PressClass - atmReq->m_PressClass) +
        3 * Math::Abs(m_TempClass - atmReq->m_TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( m_Atmosphere[i] > 0 && atmReq->m_Poisonous[i] )
            lsn += 3;
    }

    if( m_Atmosphere[atmReq->m_ReqGas] < atmReq->m_ReqMin ||
        m_Atmosphere[atmReq->m_ReqGas] > atmReq->m_ReqMax )
    {
        lsn += 3;
    }

    return lsn;
}

// ---------------------------------------------------------

double StarSystem::CalcDistance(int x, int y, int z)
{
    int dx = Math::Abs(X - x);
    int dy = Math::Abs(Y - y);
    int dz = Math::Abs(Z - z);
    return Math::Sqrt(dx * dx + dy * dy + dz * dz);
}

double StarSystem::CalcMishap(int x, int y, int z, int gv, int age)
{
    if( gv == 0 )
        throw gcnew ArgumentException("GV must not be 0 for mishap calculation!");

    double dist = CalcDistance(x, y, z);
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
            planet->m_Diameter,
            planet->m_Grav,
            planet->m_TempClass,
            planet->m_PressClass,
            planet->m_MiningDiff,
            planet->m_LSN );
        bool anyGas = false;
        for( int gas = 0; gas < GAS_MAX; ++gas )
        {
            if( planet->m_Atmosphere[gas] )
            {
                plStr = String::Concat( plStr,
                    String::Format(
                        "{0}{1}({2}%)",
                        anyGas ? "," : "",
                        GasToString(static_cast<GasType>(gas)),
                        planet->m_Atmosphere[gas] ) );
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

String^ StarSystem::PrintNumPlanets()
{
    if( m_TurnScanned == -1 )
        return "?";
    int minLSN = 99999;
    for each( Planet ^pl in m_Planets )
        minLSN = Math::Min(minLSN, pl->m_LSN == -1 ? 99999 : pl->m_LSN);
    return String::Format("{0} ({1})", m_Planets->Length, minLSN);
}

String^ StarSystem::PrintScanTurn()
{
    if( m_TurnScanned == -1 )       return "Not scanned";
    else if( m_TurnScanned == 0 )   return "Received";
    else                            return String::Format("Scanned, {0}", m_TurnScanned);
}

// ---------------------------------------------------------

String^ Colony::PrintInventoryShort()
{
    String ^ret = "";

    for( int i = 0; i < INV_MAX; ++i )
    {
        if( m_Inventory[i] )
        {
            ret = String::Format("{0}{1}{2}{3}",
                ret,
                String::IsNullOrEmpty(ret) ? "" : ",",
                m_Inventory[i],
                InvToString(static_cast<InventoryType>(i)) );
        }
    }

    return ret;
}

// ---------------------------------------------------------

GameData::GameData(void)
    : m_Species(nullptr)
    , m_TurnEUStart(0)
    , m_TurnEUProduced(0)
    , m_FleetCost(0)
    , m_FleetCostPercent(0.0)
    , m_Aliens(gcnew SortedList)
    , m_Systems(gcnew array<StarSystem^>(0))
    , m_Colonies(gcnew SortedList)
    , m_PlanetNames(gcnew SortedList)
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
    AtmosphericReq ^atm = m_Species->m_AtmReq;

    return String::Format(
        "Species: {0}\r\n"
        "Home: [{1} {2} {3} {4}]\r\n"
        "TC:{5} PC:{6}  Atm:{7} {8}-{9}%\r\n",
        GetSpeciesName(),
        m_Species->m_HomeSystem->X,
        m_Species->m_HomeSystem->Y,
        m_Species->m_HomeSystem->Z,
        m_Species->m_HomePlanet,
        atm->m_TempClass == -1 ? "??" : atm->m_TempClass.ToString(),
        atm->m_PressClass == -1 ? "??" : atm->m_PressClass.ToString(),
        GasToString( atm->m_ReqGas ),
        atm->m_ReqMin,
        atm->m_ReqMax );
}

String^ GameData::GetAllTechsSummary()
{
    return String::Format(
        "  MI={0} | GV={3}\r\n"
        "  MA={1} | LS={4}\r\n"
        "  ML={2} | BI={5}\r\n",
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
    int lev = m_Species->m_TechLevels[tech];
    int levTeach = m_Species->m_TechLevelsTeach[tech];

    if( lev != levTeach )
    {
        return String::Format("{0,2}/{1,2}", lev, levTeach);
    }

    return String::Format("{0,2}   ", lev);
}

String^ GameData::GetAliensSummary()
{
    String ^ret = "";

    for each( DictionaryEntry ^entry in m_Aliens )
    {
        Alien ^alien = safe_cast<Alien^>(entry->Value);
        if( alien == m_Species )
            continue;

        String ^prefix = "N";
        if( alien->m_Relation == SP_ALLY )
            prefix = "A";
        else if( alien->m_Relation == SP_ENEMY )
            prefix = "E";

        ret = String::Format("{0}{1}: SP {2}\r\n",
            ret, prefix, alien->m_Name );
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
    return "Ships summary\r\n - TODO...\r\n";
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
    {
        return safe_cast<Alien^>
            ( m_Aliens->GetByIndex( m_Aliens->IndexOfKey(spKey) ) );
    }

    throw gcnew NullReferenceException(
        String::Format("Species not found: {0}", sp));
}

// ---------------------------------------------------------

bool GameData::TurnCheck(int turn)
{
    if( turn > m_TurnMax )
    {
        m_TurnMax = turn;

        // New turn cleanup
        for each( DictionaryEntry ^entry in m_Aliens )
        {
            Alien ^alien = safe_cast<Alien^>(entry->Value);
            if( alien != m_Species )
                alien->m_Relation = SP_NEUTRAL; // FIXME: default relation may be different...
        }

        m_TurnEUStart       = 0;
        m_TurnEUProduced    = 0;
        m_FleetCost         = 0;
        m_FleetCostPercent  = 0.0;

        // Remove all player's colonies in case one of them no longer exists
        bool removed = false;
        do {
            removed = false;
            for each( DictionaryEntry ^entry in m_Colonies )
            {
                Colony ^colony = safe_cast<Colony^>(entry->Value);
                if( colony->m_Owner == m_Species )
                {
                    m_Colonies->Remove(entry->Key);
                    removed = true;
                    break;
                }
            }
        } while( removed );

        // remove planet names
        m_PlanetNames->Clear();
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
        m_Species->m_Relation = SP_PLAYER;
    }

    if( String::Compare(m_Species->m_Name, sp) != 0 )
        throw gcnew ArgumentException(
            String::Format("Reports for different species: {0} and {1}",
                m_Species->m_Name, sp) );
}

Alien^ GameData::AddAlien(int turn, String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        Alien ^alien = safe_cast<Alien^>
            ( m_Aliens->GetByIndex( m_Aliens->IndexOfKey(spKey) ) );

        alien->m_TurnMet = Math::Min(TurnAlign(turn), alien->m_TurnMet);

        // Reset relation if this is latest turn,
        // it will self tune while parsing Allies and Enemies.
        if( turn == m_TurnMax )
            alien->m_Relation = SP_NEUTRAL;

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
        alien->m_Relation = rel;
    }
}

void GameData::SetTechLevel(int turn, Alien ^sp, TechType tech, int lev, int levTeach)
{
    if( TurnCheck(turn) || sp->m_TechEstimateTurn == -1 )
    {
        m_TurnMax = turn;

        sp->m_TechEstimateTurn      = turn;
        sp->m_TechLevels[tech]      = lev;
        sp->m_TechLevelsTeach[tech] = levTeach;
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
    AtmosphericReq ^atm = GetSpecies()->m_AtmReq;

    if( (atm->m_ReqGas != GAS_MAX && atm->m_ReqGas != gas) ||
        atm->m_Neutral[gas] ||
        atm->m_Poisonous[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    atm->m_ReqGas = gas;
    atm->m_ReqMin = reqMin;
    atm->m_ReqMax = reqMax;
}

void GameData::SetAtmosphereNeutral(GasType gas)
{
    AtmosphericReq ^atm = GetSpecies()->m_AtmReq;

    if( atm->m_ReqGas == gas ||
        atm->m_Poisonous[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    atm->m_Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    AtmosphericReq ^atm = GetSpecies()->m_AtmReq;

    if( atm->m_ReqGas == gas ||
        atm->m_Neutral[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    atm->m_Poisonous[gas] = true;
}

StarSystem^ GameData::GetStarSystem(int x, int y, int z)
{
    for each( StarSystem ^system in m_Systems )
    {
        if( system->X == x &&
            system->Y == y &&
            system->Z == z )
        {
            return system;
        }
    }

    return nullptr;
}

void GameData::AddStarSystem(int x, int y, int z, String ^type)
{
    StarSystem ^system = GetStarSystem(x, y, z);
    if( system != nullptr )
        throw gcnew ArgumentException(
            String::Format("Star system already added: [{0} {1} {2}]", x, y, z) );

    system = gcnew StarSystem(x, y, z, type);
    Array::Resize(m_Systems, m_Systems->Length + 1);
    m_Systems[m_Systems->Length - 1] = system;
}

void GameData::AddPlanetScan(int turn, int x, int y, int z, int plNum, Planet ^planet)
{
    StarSystem ^system = GetStarSystem(x, y, z);
    if( system == nullptr )
        throw gcnew ArgumentException(
            String::Format("Star system [{0} {1} {2}] not found in galaxy list!", x, y, z) );

    Array::Resize(system->m_Planets, Math::Max(plNum, system->m_Planets->Length));
    --plNum;
    if( system->m_Planets[plNum] == nullptr ||
        turn == m_TurnMax )
    {
        if( system->m_Planets[plNum] != nullptr &&
            String::IsNullOrEmpty(planet->m_Comment) )
        {
            planet->m_Comment = system->m_Planets[plNum]->m_Comment;
        }
        system->m_Planets[plNum] = planet;
        system->m_TurnScanned = Math::Max(system->m_TurnScanned, turn);
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
    else if( sp != m_Species )
    {   // Turn N-1 report scanned after Turn N report
        // and seeing an alien colony that wasn't seen next turn.
        if( m_Colonies->ContainsKey(name->ToLower()) == false )
            createColony = true;
    }

    if( createColony )
    {
        Colony ^colony = gcnew Colony(sp, name, system, plNum);
        m_Colonies[name->ToLower()] = colony;
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
}

void GameData::CalculateLSN()
{
    for each( StarSystem ^system in m_Systems )
    {
        for each( Planet ^planet in system->m_Planets )
        {
            planet->m_LSN = planet->CalculateLSN(m_Species->m_AtmReq);
        }
    }
}

void GameData::LinkPlanetNames()
{
    for each( DictionaryEntry ^entry in m_Colonies )
    {
        Colony ^colony = safe_cast<Colony^>(entry->Value);
        if( colony->m_Owner == m_Species )
        {
            StarSystem ^system = colony->m_System;

            if( system->m_Planets->Length < colony->m_PlanetNum )
                throw gcnew ArgumentException(
                    String::Format("Colony PL {0} at [{1} {2} {3} {4}]: Planet not scanned!",
                        colony->m_Name,
                        system->X, system->Y, system->Z,
                        colony->m_PlanetNum ) );

            colony->m_Planet = system->m_Planets[ colony->m_PlanetNum - 1 ];
            colony->m_Planet->m_Name = colony->m_Name;
        }
    }

    for each( DictionaryEntry ^entry in m_PlanetNames )
    {
        PlanetName ^plName = safe_cast<PlanetName^>(entry->Value);
        StarSystem ^system = GetStarSystem( plName->X, plName->Y, plName->Z );
        if( system->m_Planets->Length >= plName->Num )
            system->m_Planets[ plName->Num - 1 ]->m_Name = plName->Name;
    }
}
