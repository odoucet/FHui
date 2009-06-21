#include "StdAfx.h"
#include "Report.h"

using namespace System::Text::RegularExpressions;

Report::Report(GameData ^gd)
    : m_GameData(gd)
    , m_Phase(PHASE_GLOBAL)
    , m_Turn(0)
    , m_bParsingAggregate(false)
    , m_StringAggregate(nullptr)
    , m_AggregateMaxLines(-1)
    , m_ScanX(0)
    , m_ScanY(0)
    , m_ScanZ(0)
    , m_ScanHasPlanets(false)
    , m_ScanHome(nullptr)
    , m_ScanColony(nullptr)
    , m_ScanShip(nullptr)
    , m_EstimateAlien(nullptr)
{
    m_Content           = gcnew String("");
    m_TmpRegexResult    = gcnew array<String^>(1);
}

bool Report::IsValid()
{
    return
        m_GameData->GetSpecies() != nullptr &&
        m_GameData->GetSpeciesName() != nullptr &&
        m_GameData->GetSpecies()->m_AtmReq->IsValid() &&
        m_Phase == PHASE_ORDERS_TEMPLATE;
}

// ------------------------------------------------------------
// Parser

bool Report::Parse(String ^s)
{
    m_Content = String::Concat(m_Content,
        String::Concat(s, "\r\n") );

    s = s->Trim(' ');
    s = s->Trim('\t');

    if( m_Phase == PHASE_FILE_DETECT )
    {
        if( String::IsNullOrEmpty(s) )
            return true;

        if( Regex("^EVENT LOG FOR TURN \\d+").Match(s)->Success )
        {
            m_Phase = PHASE_GLOBAL;
            return true;
        }

        return false;
    }
    if( m_Phase == PHASE_ORDERS_TEMPLATE )
        return true;

    if( m_bParsingAggregate )
    {
        if( !String::IsNullOrEmpty(s) )
        {
            m_StringAggregate = String::Format("{0} {1}",
                m_StringAggregate, s);
            if( m_AggregateMaxLines == -1 ||
                --m_AggregateMaxLines > 0 )
            return true;
        }
    }

    switch( m_Phase )
    {
    case PHASE_GLOBAL:
        // Turn number
        if( MatchWithOutput(s, "^EVENT LOG FOR TURN (\\d+)") )
        {
            m_Turn = GetMatchResultInt(0);
            break;
        }
        if( MatchWithOutput(s, "^START OF TURN (\\d+)") )
        {
            m_Turn = GetMatchResultInt(0);
            break;
        }

        // Species name
        if( MatchWithOutput(s, "^Species name: ([^,;]+)") )
            m_GameData->SetSpecies( GetMatchResult(0) );
        // Atmospheric requirements
        else if( MatchWithOutput(s, "^Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
            m_GameData->SetAtmosphereReq(
                GasFromString( GetMatchResult(2) ), // required gas
                GetMatchResultInt(0),               // min level
                GetMatchResultInt(1) );             // max level
        else if( MatchAggregateList(s, "^Neutral Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmosphereNeutral(
                    GasFromString(GetMatchResult(i)) );
        }
        else if( MatchAggregateList(s, "^Poisonous Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmospherePoisonous(
                    GasFromString(GetMatchResult(i)) );
        }
        // Economy
        else if( MatchWithOutput(s, "^Economic units = (\\d+)") )
            m_GameData->SetTurnStartEU( m_Turn, GetMatchResultInt(0) );
        // Fleet maintenance
        else if( MatchWithOutput(s, "^Fleet maintenance cost = (\\d+) \\((\\d+\\.?\\d+)% of total production\\)") )
            m_GameData->SetFleetCost(m_Turn, GetMatchResultInt(0), GetMatchResultFloat(1));
        // Species...
        else if( Regex("^Species met:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_MET, s, AGGREGATE_LINES_MAX);
        else if( MatchWithOutput(s, "^Scan of home star system for SP\\s+([^,;]+):$") )
            m_ScanHome = m_GameData->AddAlien(m_Turn, GetMatchResult(0));
        else if( Regex("^Allies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ALLIES, s, AGGREGATE_LINES_MAX);
        else if( Regex("^Enemies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ENEMIES, s, AGGREGATE_LINES_MAX);
        else if( MatchWithOutput(s, "^Government name:\\s+(.+)$") )
            m_GameData->GetSpecies()->m_GovName = GetMatchResult(0);
        else if( MatchWithOutput(s, "^Government type:\\s+(.+)$") )
            m_GameData->GetSpecies()->m_GovType = GetMatchResult(0);
        // Tech levels
        else if( Regex("^Tech Levels:").Match(s)->Success )
            m_Phase = PHASE_TECH_LEVELS;
        // System scan
        else if( MatchSystemScanStart(s) )
        {}
        else if( Regex("^\\w+ PLANET: PL.+").Match(s)->Success ||
                 Regex("^\\w+ COLONY: PL.+").Match(s)->Success )
            StartLineAggregate(PHASE_COLONY, s, 1);
        else if( Regex("^Other planets and ships:").Match(s)->Success )
            m_Phase = PHASE_OTHER_PLANETS_SHIPS;
        else if( Regex("^Combat orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_COMBAT;
        // Parsing ends here
        else if( Regex("^ORDER SECTION.").Match(s)->Success )
            m_Phase = PHASE_ORDERS_TEMPLATE;
        break;

    case PHASE_SPECIES_MET:
        if( MatchAggregateList(FinishLineAggregate(true), "^Species met:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->AddAlien(m_Turn, GetMatchResult(i));
            break;
        }
        return false;

    case PHASE_SPECIES_ALLIES:
        if( MatchAggregateList(FinishLineAggregate(true), "^Allies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAlienRelation(m_Turn, GetMatchResult(i), SP_ALLY);
            break;
        }
        return false;

    case PHASE_SPECIES_ENEMIES:
        if( MatchAggregateList(FinishLineAggregate(true), "^Enemies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAlienRelation(m_Turn, GetMatchResult(i), SP_ENEMY);
            break;
        }
        return false;

    case PHASE_ORDERS_COMBAT:
        if( Regex("^Pre-departure orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_PRE_DEP;
        break;

    case PHASE_ORDERS_PRE_DEP:
        if( Regex("^Jump orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_JUMP;
        // System scan
        else if( MatchSystemScanStart(s) )
        {}
        break;

    case PHASE_ORDERS_JUMP:
        if( Regex("^Production orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_PROD;
        break;

    case PHASE_ORDERS_PROD:
        if( Regex("^Post-arrival orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_POST_ARRIVAL;
        else if( Regex("^Estimate of the technology of SP ").Match(s)->Success )
            StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
        break;

    case PHASE_ORDERS_POST_ARRIVAL:
        if( Regex("^Strike orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_STRIKE;
        // System scan
        else if( MatchSystemScanStart(s) )
        {}
        break;

    case PHASE_ORDERS_STRIKE:
        if( Regex("^Other events:").Match(s)->Success )
            m_Phase = PHASE_GLOBAL;
        break;

    case PHASE_TECH_LEVELS:
        if( MatchTech(s, "^Mining", TECH_MI) ||
            MatchTech(s, "^Manufacturing", TECH_MA) ||
            MatchTech(s, "^Military", TECH_ML) ||
            MatchTech(s, "^Gravitics", TECH_GV) ||
            MatchTech(s, "^Life Support", TECH_LS) )
            break;
        else if( MatchTech(s, "Biology", TECH_BI) )
            m_Phase = PHASE_GLOBAL;
        else
            return false;
        break;

    case PHASE_ALIEN_ESTIMATE:
        if( m_bParsingAggregate )
            s = FinishLineAggregate(false);

        if( MatchWithOutput(s, "^Estimate of the technology of SP\\s+([^,;]+)\\s+\\(government name '([^']+)', government type '([^']+)'\\)") )
        {
            m_EstimateAlien = m_GameData->AddAlien(m_Turn, GetMatchResult(0));
            m_EstimateAlien->m_GovName = GetMatchResult(1);
            m_EstimateAlien->m_GovType = GetMatchResult(2);
        }
        else if( m_EstimateAlien != nullptr &&
            MatchWithOutput(s, "MI =\\s+(\\d+), MA =\\s+(\\d+), ML =\\s+(\\d+), GV =\\s+(\\d+), LS =\\s+(\\d+), BI =\\s+(\\d+)\\.") )
        {
            if( m_Turn > m_EstimateAlien->m_TechEstimateTurn )
            {
                m_EstimateAlien->m_TechLevels[TECH_MI] = GetMatchResultInt(0);
                m_EstimateAlien->m_TechLevels[TECH_MA] = GetMatchResultInt(1);
                m_EstimateAlien->m_TechLevels[TECH_ML] = GetMatchResultInt(2);
                m_EstimateAlien->m_TechLevels[TECH_GV] = GetMatchResultInt(3);
                m_EstimateAlien->m_TechLevels[TECH_LS] = GetMatchResultInt(4);
                m_EstimateAlien->m_TechLevels[TECH_BI] = GetMatchResultInt(5);
                m_EstimateAlien->m_TechEstimateTurn = m_Turn;
            }
        }
        else
            m_Phase = PHASE_ORDERS_PROD;
        break;

    case PHASE_SYSTEM_SCAN:
        if( String::IsNullOrEmpty(s) )
        {
            if( m_ScanHasPlanets )
            {
                m_Phase = PHASE_GLOBAL;
                m_ScanHome = nullptr;
            }
        }
        else
            MatchPlanetScan(s);
        break;

    case PHASE_COLONY:
        if( MatchSectionEnd(s) )
        {
            m_Phase = PHASE_GLOBAL;
            m_ScanColony = nullptr;
            m_ScanShip = nullptr;
        }
        else
            MatchColonyScan(s);
        break;

    case PHASE_COLONY_INVENTORY:
        if( String::IsNullOrEmpty(s) )
            m_Phase = PHASE_COLONY;
        else
            MatchColonyInventoryScan(s);
        break;

    case PHASE_COLONY_SHIPS:
        if( String::IsNullOrEmpty(s) )
        {
            m_Phase = PHASE_COLONY;
            m_ScanShip = nullptr;
        }
        else
            MatchColonyShipsScan(s);
        break;

    case PHASE_OTHER_PLANETS_SHIPS:
        if( MatchSectionEnd(s) )
        {
            m_Phase = PHASE_GLOBAL;
            m_ScanColony = nullptr;
            m_ScanShip = nullptr;
        }
        else
            MatchOtherPlanetsShipsScan(s);
    }

    return true;
}

bool Report::MatchSectionEnd(String ^s)
{
    if( Regex("^(\\*\\s)+\\*\\s*$").Match(s)->Success )
        return true;
    return false;
}

bool Report::MatchSystemScanStart(String ^s)
{
    if( MatchWithOutput(s, "^Coordinates:\\s+[Xx]\\s+=\\s+(\\d+)\\s+[Yy]\\s+=\\s+(\\d+)\\s+[Zz]\\s+=\\s+(\\d+)") )
    {
        m_Phase = PHASE_SYSTEM_SCAN;
        m_ScanX = GetMatchResultInt(0);
        m_ScanY = GetMatchResultInt(1);
        m_ScanZ = GetMatchResultInt(2);
        m_ScanHasPlanets = false;

        // Set the home system
        if( m_ScanHome )
            m_ScanHome->m_HomeSystem = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);

        return true;
    }
    return false;
}

void Report::MatchPlanetScan(String ^s)
{
    //                          0:plNum   1:dia    2:gv            3:tc       4:pc    5:mining diff
    if( MatchWithOutput(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+") )
    {
        int plNum = GetMatchResultInt(0);
        Planet ^planet = gcnew Planet(
            plNum,
            GetMatchResultInt(1),
            GetMatchResultFloat(2),
            GetMatchResultInt(3),
            GetMatchResultInt(4),
            GetMatchResultFloat(5) );

        // Try reading LSN
        if( MatchWithOutput(s, "(\\d+)\\s+") )
        {
            planet->m_LSN = GetMatchResultInt(0);

            if( m_ScanHome && planet->m_LSN == 0 )
            {
                m_ScanHome->m_HomePlanet = plNum;
                m_ScanHome->m_AtmReq->m_TempClass  = planet->m_TempClass;
                m_ScanHome->m_AtmReq->m_PressClass = planet->m_PressClass;
            }
        }
        else if( MatchWithOutput(s, "[x?\\-]+\\s+") )
        {
            // just skip from input
        }
        else
            throw gcnew ArgumentException("Report contains invalid planetary scan (LSN)");

        // Scan gases
        if( MatchWithOutput(s, "^No atmosphere\\s*") )
        {
            // just skip from input
        }
        else
        {
            bool bGasMatched = false;
            while( MatchWithOutput(s, ",?(\\w+)\\((\\d+)%\\)") )
            {
                bGasMatched = true;
                int gas = GasFromString(GetMatchResult(0));
                planet->m_Atmosphere[gas] = GetMatchResultInt(1);
            }
            if( !bGasMatched )
                throw gcnew ArgumentException("Report contains invalid planetary scan (atmosphere)");
        }

        if( !String::IsNullOrEmpty(s) )
            planet->m_Comment = s;

        m_GameData->AddPlanetScan( m_Turn, m_ScanX, m_ScanY, m_ScanZ, plNum, planet );
        m_ScanHasPlanets = true;
    }
}

bool Report::MatchTech(String ^s, String ^techName, TechType tech)
{
    String ^e2 = String::Format("{0} = (\\d+)/(\\d+)", techName);
    String ^e1 = String::Format("{0} = (\\d+)", techName);
    if( MatchWithOutput(s, e2) )
    {
        m_GameData->SetTechLevel(
            m_Turn,
            m_GameData->GetSpecies(),
            tech,
            GetMatchResultInt(0),
            GetMatchResultInt(1));
        return true;
    }
    if( MatchWithOutput(s, e1) )
    {
        m_GameData->SetTechLevel(
            m_Turn,
            m_GameData->GetSpecies(),
            tech,
            GetMatchResultInt(0),
            GetMatchResultInt(0));
        return true;
    }
    return false;
}

void Report::MatchColonyScan(String ^s)
{
    if( m_bParsingAggregate )
        s = FinishLineAggregate(false);

    if( m_ScanColony == nullptr )
    {
        // Initial colony data
        PlanetType planetType = PLANET_HOME;
        if( MatchWithOutput(s, "^(HOME|COLONY)\\s+PLANET: PL\\s+") )
        {
            if( GetMatchResult(0)[0] == 'C' )
                planetType = PLANET_COLONY;
        }
        else if( MatchWithOutput(s, "^(MINING|RESORT)\\s+COLONY: PL\\s+") )
        {
            if( GetMatchResult(0)[0] == 'M' )
                planetType = PLANET_COLONY_MINING;
            else
                planetType = PLANET_COLONY_RESORT;
        }
        else
            throw gcnew ArgumentException(
                String::Format("Unknown colony type in report: {0}", s) );

        if( MatchWithOutput(s, "([^,;]+)\\s+Coordinates: x = (\\d+), y = (\\d+), z = (\\d+), planet number (\\d+)") )
        {
            String ^plName = GetMatchResult(0);
            plName = plName->TrimEnd(' ');
            StarSystem ^system = m_GameData->GetStarSystem(
                GetMatchResultInt(1),
                GetMatchResultInt(2),
                GetMatchResultInt(3) );

            m_ScanColony = m_GameData->AddColony(
                m_Turn,
                m_GameData->GetSpecies(),
                plName,
                system,
                GetMatchResultInt(4) );

            if( m_ScanColony == nullptr )
            {
                m_Phase = PHASE_GLOBAL;
                return;
            }

            m_ScanColony->m_PlanetType = planetType;
        }
        else
            throw gcnew ArgumentException(
                String::Format("Unable to parse colony coordinates: {0}", s) );

        return;
    }

    // Remaining entries
    switch( m_ScanColony->m_PlanetType )
    {
    case PLANET_HOME:
    case PLANET_COLONY:
        if( MatchWithOutput(s, "^Total available for spending this turn = (\\d+) - (\\d+) = \\d+") )
        {
            m_ScanColony->m_EUProd  = GetMatchResultInt(0);
            m_ScanColony->m_EUFleet = GetMatchResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->m_EUProd );
        }
        break;

    case PLANET_COLONY_MINING:
        if( MatchWithOutput(s, "^This mining colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->m_EUProd  = GetMatchResultInt(0);
            m_ScanColony->m_EUFleet = GetMatchResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->m_EUProd );
        }
        break;

    case PLANET_COLONY_RESORT:
        if( MatchWithOutput(s, "^This resort colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->m_EUProd  = GetMatchResultInt(0);
            m_ScanColony->m_EUFleet = GetMatchResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->m_EUProd );
        }
        break;
    }

    if( MatchWithOutput(s, "^Economic efficiency = (\\d+)%") )
    {
        m_ScanColony->m_EconomicEff = GetMatchResultInt(0);
    }
    else if( MatchWithOutput(s, "^Production penalty = (\\d+)% \\(") )
    {
        m_ScanColony->m_ProdPenalty = GetMatchResultInt(0);
    }
    else if( MatchWithOutput(s, "^Mining base = (\\d+\\.\\d+) \\(MI = \\d+, MD = (\\d+\\.\\d+)\\)") )
    {
        m_ScanColony->m_MiBase = GetMatchResultFloat(0);
        m_ScanColony->m_MiDiff = GetMatchResultFloat(1);
    }
    else if( MatchWithOutput(s, "^Raw Material Units \\(RM,C1\\) carried over from last turn = (\\d+)") )
    {
        m_ScanColony->m_Inventory[INV_RM] = GetMatchResultInt(0);
    }
    else if( MatchWithOutput(s, "^Manufacturing base = (\\d+\\.\\d+) \\(") )
    {
        m_ScanColony->m_MaBase = GetMatchResultFloat(0);
    }
    else if( MatchWithOutput(s, "^Shipyard capacity = (\\d+)") )
    {
        m_ScanColony->m_Shipyards = GetMatchResultInt(0);
    }
    else if( MatchWithOutput(s, "^Available population units = (\\d+)") )
    {
        m_ScanColony->m_AvailPop = GetMatchResultInt(0);
    }
    else if( Regex("^Planetary inventory:").Match(s)->Success )
        m_Phase = PHASE_COLONY_INVENTORY;
    else if( Regex("^Ships at PL [^,;]+:").Match(s)->Success )
        m_Phase = PHASE_COLONY_SHIPS;
}

void Report::MatchColonyInventoryScan(String ^s)
{
    if( MatchWithOutput(s, "^[\\w\\s]+\\(PD,C3\\) = (\\d+) (warship equivalence = \\d+ tons)") )
        m_ScanColony->m_Inventory[INV_PD] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SU,C20\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SU] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(CU,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_CU] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(IU,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_IU] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(AU,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_AU] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(FS,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_FS] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(FD,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_FD] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(DR,C1\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_DR] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(FM,C5\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_FM] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(FJ,C5\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_FJ] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GW,C100\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GW] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GT,C20\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GT] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(JP,C10\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_JP] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(TP,C100\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_TP] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU1,C5\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU1] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU2,C10\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU2] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU3,C15\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU3] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU4,C20\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU4] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU5,C25\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU5] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU6,C30\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU6] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU7,C35\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU7] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU8,C40\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU8] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(GU9,C45\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_GU9] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG1,C5\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG1] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG2,C10\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG2] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG3,C15\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG3] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG4,C20\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG4] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG5,C25\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG5] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG6,C30\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG6] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG7,C35\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG7] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG8,C40\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG8] = GetMatchResultInt(0);
    else if( MatchWithOutput(s, "^[\\w\\s]+\\(SG9,C45\\) = (\\d+)") )
        m_ScanColony->m_Inventory[INV_SG9] = GetMatchResultInt(0);
}

void Report::MatchColonyShipsScan(String ^s)
{
    MatchShipScan(s, true);
}

void Report::MatchShipScan(String ^s, bool bColony)
{
    ShipType type   = SHIP_MAX;
    int size        = 0;
    bool subLight   = false;

    if( MatchWithOutput(s, "BAS\\s+") )
        type = SHIP_BAS;
    else if( MatchWithOutput(s, "TR(\\d+)([Ss]?)\\s+") ) 
    {
        type = SHIP_TR;
        size = GetMatchResultInt(0);
        subLight = 0 == String::Compare( GetMatchResult(1)->ToLower(), "s" );
    }
    else
    {
        try
        {
            if( MatchWithOutput(s, "([A-Za-z]{2})([Ss]?)\\s+") )
            {
                type = ShipFromString( GetMatchResult(0) );
                subLight = 0 == String::Compare( GetMatchResult(1)->ToLower(), "s" );
            }
            else
                return;
        }
        catch( ArgumentException^ )
        {
            return;
        }
    }

    if( MatchWithOutput(s, "([^,;]+) \\(A(\\d+),([DOLdol])") )
    {
        String ^name    = GetMatchResult(0);
        int age         = GetMatchResultInt(1);
        String ^loc     = GetMatchResult(2)->ToLower();

        m_ScanShip      = m_GameData->AddShip(
            m_Turn, m_GameData->GetSpecies(), type, name, size, subLight);

        if( m_ScanShip == nullptr )
            return;

        if( bColony )
        {
            m_ScanShip->m_X = m_ScanColony->m_System->X;
            m_ScanShip->m_Y = m_ScanColony->m_System->Y;
            m_ScanShip->m_Z = m_ScanColony->m_System->Z;
            m_ScanShip->m_System = m_ScanColony->m_System;
        }
        else
        {
            m_ScanShip->m_X = m_ScanX;
            m_ScanShip->m_Y = m_ScanY;
            m_ScanShip->m_Z = m_ScanZ;
            try
            {
                m_ScanShip->m_System = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);
            }
            catch( ArgumentException^ )
            {
                m_ScanShip->m_System = nullptr;
            }
        }

        m_ScanShip->m_Age = age;

        switch( loc[0] )
        {
        case 'd':
            m_ScanShip->m_Location = SHIP_LOC_DEEP_SPACE;
            break;
        case 'o':
            m_ScanShip->m_Location = SHIP_LOC_ORBIT;
            break;
        case 'l':
            m_ScanShip->m_Location = SHIP_LOC_LANDED;
            break;
        }
        if( m_ScanShip->m_Location != SHIP_LOC_DEEP_SPACE )
        {
            if( MatchWithOutput(s, "(\\d+)") )
                m_ScanShip->m_Planet = GetMatchResultInt(0);
            else
                throw gcnew ArgumentException(
                    String::Format("Unable to parse ship '{0}' location.", name));
        }

        if( type == SHIP_BAS )
        {
            if( MatchWithOutput(s, ",(\\d+) tons") )
                m_ScanShip->m_Size = GetMatchResultInt(0);
            else
                throw gcnew ArgumentException(
                    String::Format("Unable to parse starbase '{0}' size.", name));
        }

        if( bColony )
        {
            if( MatchWithOutput(s, "\\)\\s+(\\d+)\\s*") )
                m_ScanShip->m_Capacity = GetMatchResultInt(0);
            else
                throw gcnew ArgumentException(
                    String::Format("Unable to parse ship '{0}' capacity.", name));
        }
        else
        {
            if( !MatchWithOutput(s, "\\)\\s*") )
                throw gcnew ArgumentException(
                    String::Format("Unable to parse ship '{0}'.", name));
            m_ScanShip->CalculateCapacity();
        }

        while( !String::IsNullOrEmpty(s) )
        {
            if( MatchWithOutput(s, ",?\\s*(\\d+)\\s+(\\w+)\\s*") )
            {
                int amount = GetMatchResultInt(0);
                InventoryType inv = InvFromString( GetMatchResult(1) );
                m_ScanShip->m_Cargo[inv] = amount;
            }
            else
                throw gcnew ArgumentException(
                    String::Format("Unable to parse ship '{0}' inventory.", name));
        }
    }
}

void Report::MatchOtherPlanetsShipsScan(String ^s)
{
    if( MatchWithOutput(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+#(\\d+)\\s+PL\\s+(.+)$") )
    {
        m_ScanX = GetMatchResultInt(0);
        m_ScanY = GetMatchResultInt(1);
        m_ScanZ = GetMatchResultInt(2);

        m_GameData->AddPlanetName(
            m_Turn, m_ScanX, m_ScanY, m_ScanZ,
            GetMatchResultInt(3),
            GetMatchResult(4) );
    }

    if( MatchWithOutput(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+") )
    {
        m_ScanX = GetMatchResultInt(0);
        m_ScanY = GetMatchResultInt(1);
        m_ScanZ = GetMatchResultInt(2);
    }
    MatchShipScan(s, false);
}

void Report::StartLineAggregate(PhaseType phase, String ^s, int maxLines)
{
    m_bParsingAggregate = true;
    m_StringAggregate = s;
    m_AggregateMaxLines = maxLines;
    m_Phase = phase;
}

String^ Report::FinishLineAggregate(bool resetPhase)
{
    m_bParsingAggregate = false;
    String ^ret = m_StringAggregate;
    m_StringAggregate = nullptr;
    if( resetPhase )
        m_Phase = PHASE_GLOBAL;
    return ret;
}

int Report::GetMatchResultInt(int arg)
{
    return int::Parse(m_TmpRegexResult[arg]);
}

float Report::GetMatchResultFloat(int arg)
{
    return Single::Parse(
        m_TmpRegexResult[arg],
        Globalization::CultureInfo::InvariantCulture );
}

bool Report::MatchWithOutput(String ^%s, String ^exp)
{
    Array::Clear(m_TmpRegexResult, 0, m_TmpRegexResult->Length);

    Match ^m = Regex(exp).Match(s);
    if( m->Success )
    {
        String ^g = m->Groups[0]->ToString();
        s = s->Substring( s->IndexOf(g) + g->Length );

        int cnt = m->Groups->Count - 1;
        Array::Resize(m_TmpRegexResult, cnt);
        for( int i = 0; i < cnt; ++i )
        {
            m_TmpRegexResult[i] = m->Groups[i + 1]->ToString();
        }
        return true;
    }

    return false;
}

bool Report::MatchAggregateList(String ^s, String ^prefix, String ^exp)
{
    Array::Clear(m_TmpRegexResult, 0, m_TmpRegexResult->Length);
    int cnt = 0;

    String ^exprInit = String::Format("{0}\\s*{1}", prefix, exp);
    String ^exprCont = String::Format("^\\s*,\\s*{0}", exp);
    Match ^m = Regex(exprInit).Match(s);
    while( m->Success )
    {
        Array::Resize(m_TmpRegexResult, cnt + 1);
        m_TmpRegexResult[cnt++] = m->Groups[1]->ToString();

        int l = m->Groups[0]->ToString()->Length;
        if( s->Length == l )
            break;
        s = s->Substring(l);

        m = Regex(exprCont).Match(s);
    }

    return cnt > 0;
}
