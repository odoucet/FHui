#include "StdAfx.h"
#include "GameData.h"

// ---------------------------------------------------------

int Planet::CalculateLSN(AtmosphericReq ^atmReq, int tc, int pc)
{
    int lsn =
        3 * Math::Abs(m_PressClass - pc) +
        3 * Math::Abs(m_TempClass - tc);

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

// ---------------------------------------------------------

GameData::GameData(void)
    : m_Species(nullptr)
    , m_TurnEUStart(0)
    , m_TurnEUProduced(0)
    , m_FleetCost(0)
    , m_FleetCostPercent(0.0)
    , m_Aliens(gcnew SortedList)
    , m_Systems(gcnew array<StarSystem^>(0))
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
        "---------------------------\r\n",
        GetSpeciesSummary(),
        GetAllTechsSummary(),
        GetEconomicSummary(),
        GetAliensSummary() );
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
        return String::Format("{0,3}/{1,3}", lev, levTeach);
    }

    return String::Format("{0,3}    ", lev);
}

String^ GameData::GetAliensSummary()
{
    String ^ret = gcnew String("");

    for( int i = 0; i < m_Aliens->Count; ++i )
    {
        Alien ^alien = safe_cast<Alien^>(m_Aliens->GetByIndex(i));
        if( alien->m_Relation == SP_ALLY )
        {
            String ^tmp = String::Format("{0}Ally: SP {1}\r\n",
                ret, alien->m_Name);
            ret = tmp;
        }
    }

    for( int i = 0; i < m_Aliens->Count; ++i )
    {
        Alien ^alien = safe_cast<Alien^>(m_Aliens->GetByIndex(i));
        if( alien->m_Relation == SP_ENEMY )
        {
            String ^tmp = String::Format("{0}Enemy: SP {1}\r\n",
                ret, alien->m_Name);
            ret = tmp;
        }
    }

    if( ret->Length == 0 )
        ret = "No Allies/Enemies";

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
            safe_cast<Alien^>(entry->Value)->m_Relation = SP_NEUTRAL; // FIXME: default relation may be different...

        m_TurnEUStart       = 0;
        m_TurnEUProduced    = 0;
        m_FleetCost         = 0;
        m_FleetCostPercent  = 0.0;
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
