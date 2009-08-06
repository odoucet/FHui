#include "StdAfx.h"
#include "Report.h"

namespace FHUI
{

//////////////////////////////////////////////////////////

RegexMatcher::RegexMatcher()
{
    m_Results = gcnew array<String^>(1);

    ExpCmdColony    = gcnew Regex("^COLONY ([^,;]+)$");
    ExpCmdShipJump  = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Jump to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([0-9-]+)$");
    ExpCmdShipUpg   = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Upgrade$");
    ExpCmdShipRec   = gcnew Regex("^SHIP ([A-Z0-9]+) ([^,;]+) Recycle$");
    ExpCmdPLName    = gcnew Regex("^Name\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+) PL ([^,;]+)$");
    ExpCmdPLDisband = gcnew Regex("^Disband PL ([^,;]+)$");
    ExpCmdSPNeutral = gcnew Regex("^Neutral SP ([^,;]+)$");
    ExpCmdSPAlly    = gcnew Regex("^Ally SP ([^,;]+)$");
    ExpCmdSPEnemy   = gcnew Regex("^Enemy SP ([^,;]+)$");
    ExpCmdSPTeach   = gcnew Regex("^Teach ([A-Z]{2}) (\\d+) SP ([^,;]+)$");
    ExpCmdBuiShipyard = gcnew Regex("^build shipyard$");
}

int RegexMatcher::GetResultInt(int arg)
{
    return int::Parse(m_Results[arg]);
}

float RegexMatcher::GetResultFloat(int arg)
{
    return Single::Parse(
        m_Results[arg],
        Globalization::CultureInfo::InvariantCulture );
}

bool RegexMatcher::Match(String ^%s, String ^exp)
{
    return Match(s, gcnew Regex(exp));
}

bool RegexMatcher::Match(String ^%s, Regex ^exp)
{
    Array::Clear(m_Results, 0, m_Results->Length);

    System::Text::RegularExpressions::Match ^m = exp->Match(s);
    if( m->Success )
    {
        String ^g = m->Groups[0]->ToString();
        s = s->Substring( s->IndexOf(g) + g->Length );

        int cnt = m->Groups->Count - 1;
        Array::Resize(m_Results, cnt);
        for( int i = 0; i < cnt; ++i )
        {
            m_Results[i] = m->Groups[i + 1]->ToString();
        }
        return true;
    }

    return false;
}

bool RegexMatcher::MatchList(String ^s, String ^prefix, String ^exp)
{
    Array::Clear(m_Results, 0, m_Results->Length);
    int cnt = 0;

    String ^exprInit = String::Format("{0}\\s*{1}", prefix, exp);
    String ^exprCont = String::Format("^\\s*,\\s*{0}", exp);
    System::Text::RegularExpressions::Match ^m = Regex(exprInit).Match(s);
    while( m->Success )
    {
        Array::Resize(m_Results, cnt + 1);
        m_Results[cnt++] = m->Groups[1]->ToString();

        int l = m->Groups[0]->ToString()->Length;
        if( s->Length == l )
            break;
        s = s->Substring(l);

        m = Regex(exprCont).Match(s);
    }

    return cnt > 0;
}

//////////////////////////////////////////////////////////

Report::Report(GameData ^gd, RegexMatcher ^rm)
    : m_GameData(gd)
    , m_RM(rm)
    , m_Phase(gd == nullptr ? PHASE_FILE_DETECT : PHASE_GLOBAL)
    , m_PhasePreAggregate(PHASE_GLOBAL)
    , m_Turn(-1)
    , m_LineCnt(0)
    , m_bParsingAggregate(false)
    , m_StringAggregate(nullptr)
    , m_AggregateMaxLines(-1)
    , m_ScanX(0)
    , m_ScanY(0)
    , m_ScanZ(0)
    , m_ScanHasPlanets(false)
    , m_ScanAlien(nullptr)
    , m_ScanColony(nullptr)
    , m_ScanShip(nullptr)
    , m_PirateShipsCnt(0)
    , m_EstimateAlien(nullptr)
{
    m_Content           = gcnew String("");
}

bool Report::IsValid()
{
    return
        m_GameData->GetSpecies() != nullptr &&
        m_GameData->GetSpeciesName() != nullptr &&
        m_GameData->GetSpecies()->AtmReq->IsValid() &&
        m_Phase == PHASE_ORDERS_TEMPLATE;
}

// ------------------------------------------------------------
// Parser

bool Report::Parse(String ^s)
{
    ++m_LineCnt;

    if( m_GameData != nullptr )
    {
        m_Content = String::Concat(m_Content,
            String::Concat(s, "\r\n") );
    }

    s = s->Trim();

    if( m_Phase == PHASE_FILE_DETECT )
    {   // Turn scan mode only
        if( String::IsNullOrEmpty(s) )
            return true;

        if( m_RM->Match(s, "^EVENT LOG FOR TURN (\\d+)") )
        {
            int turn = m_RM->GetResultInt(0);
            if( turn == 0 )
            {   // only turn 0 may be recognized by "event log..."
                // because it can't contain meaningfull 'start of turn'
                m_Turn = 0;
            }
        }
        if( m_RM->Match(s, "^START OF TURN (\\d+)") )
        {
            m_Turn = m_RM->GetResultInt(0);
        }
        return true;
    }

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
        if( m_RM->Match(s, "^EVENT LOG FOR TURN (\\d+)") )
        {
            m_Turn = m_RM->GetResultInt(0);
            break;
        }
        if( m_RM->Match(s, "^START OF TURN (\\d+)") )
        {
            m_Turn = m_RM->GetResultInt(0);
            break;
        }

        // Species name
        if( m_RM->Match(s, "^Species name: ([^,;]+)") )
            m_GameData->SetSpecies( m_RM->Results[0] );
        // Atmospheric requirements
        else if( m_RM->Match(s, "^Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
            m_GameData->SetAtmosphereReq(
                FHStrings::GasFromString( m_RM->Results[2] ), // required gas
                m_RM->GetResultInt(0),               // min level
                m_RM->GetResultInt(1) );             // max level
        else if( m_RM->MatchList(s, "^Neutral Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAtmosphereNeutral(
                    FHStrings::GasFromString(m_RM->Results[i]) );
        }
        else if( m_RM->MatchList(s, "^Poisonous Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAtmospherePoisonous(
                    FHStrings::GasFromString(m_RM->Results[i]) );
        }
        // Economy
        else if( m_RM->Match(s, "^Economic units = (\\d+)") )
            m_GameData->SetTurnStartEU( m_Turn, m_RM->GetResultInt(0) );
        // Fleet maintenance
        else if( m_RM->Match(s, "^Fleet maintenance cost = (\\d+) \\((\\d+\\.?\\d+)% of total production\\)") )
            m_GameData->SetFleetCost(m_Turn, m_RM->GetResultInt(0), m_RM->GetResultFloat(1));
        // Species...
        else if( Regex("^Species met:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_MET, s, AGGREGATE_LINES_MAX);
        else if( m_RM->Match(s, "^Scan of home star system for SP\\s+([^,;]+):$") )
            m_ScanAlien = m_GameData->AddAlien(m_Turn, m_RM->Results[0]);
        else if( Regex("^Allies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ALLIES, s, AGGREGATE_LINES_MAX);
        else if( Regex("^Enemies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ENEMIES, s, AGGREGATE_LINES_MAX);
        else if( m_RM->Match(s, "^Government name:\\s+(.+)$") )
            m_GameData->GetSpecies()->GovName = m_RM->Results[0];
        else if( m_RM->Match(s, "^Government type:\\s+(.+)$") )
            m_GameData->GetSpecies()->GovType = m_RM->Results[0];
        else if( m_RM->Match(s, "^Aliens at\\s+x\\s+=\\s+(\\d+), y\\s+=\\s+(\\d+), z\\s+=\\s+(\\d+)") )
        {
            m_ScanX = m_RM->GetResultInt(0);
            m_ScanY = m_RM->GetResultInt(1);
            m_ScanZ = m_RM->GetResultInt(2);
            m_Phase = PHASE_ALIENS_REPORT;
        }
        else if( Regex("^Estimate of the technology of SP ").Match(s)->Success )
        {
            m_EstimateAlien = nullptr;
            StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
        }
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
        // Message
        else if( m_RM->Match(s, "^You received the following message from SP ([^,;]+):") )
        {
            m_ScanAlien = m_GameData->AddAlien(m_Turn, m_RM->Results[0]);
            m_Phase = PHASE_MESSAGE;
        }
        // Parsing ends here
        else if( Regex("^ORDER SECTION.").Match(s)->Success )
            m_Phase = PHASE_ORDERS_TEMPLATE;
        break;

    case PHASE_MESSAGE:
        if( Regex("^\\*\\*\\* End of Message \\*\\*\\*$").Match(s)->Success )
            m_Phase = PHASE_GLOBAL;
        else if( m_RM->Match(s, "([a-zA-Z0-9_.]+@[a-zA-Z0-9_.]+\\.[a-zA-Z0-9_]+)") )
        {
            m_ScanAlien->Email = m_RM->Results[0];
            m_Phase = PHASE_GLOBAL;
        }
        break;

    case PHASE_SPECIES_MET:
        if( m_RM->MatchList(FinishLineAggregate(true), "^Species met:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->AddAlien(m_Turn, m_RM->Results[i]);
            break;
        }
        return false;

    case PHASE_SPECIES_ALLIES:
        if( m_RM->MatchList(FinishLineAggregate(true), "^Allies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAlienRelation(m_Turn, m_RM->Results[i], SP_ALLY);
            break;
        }
        return false;

    case PHASE_SPECIES_ENEMIES:
        if( m_RM->MatchList(FinishLineAggregate(true), "^Enemies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAlienRelation(m_Turn, m_RM->Results[i], SP_ENEMY);
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
        {
            m_EstimateAlien = nullptr;
            StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
        }
        break;

    case PHASE_ORDERS_POST_ARRIVAL:
        if( Regex("^Strike orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_STRIKE;
        // System scan
        else if( MatchSystemScanStart(s) )
        {}
        break;

    case PHASE_ORDERS_STRIKE:
        if( Regex("^Other events:").Match(s)->Success ||
            Regex("^SPECIES STATUS$").Match(s)->Success )   // in case no other events happened
            m_Phase = PHASE_GLOBAL;
        // It may happen that no other events were present, but there's incoming message
        else if( m_RM->Match(s, "^You received the following message from SP ([^,;]+):") )
        {
            m_ScanAlien = m_GameData->AddAlien(m_Turn, m_RM->Results[0]);
            m_Phase = PHASE_MESSAGE;
        }
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

        if( m_EstimateAlien != nullptr )
        {
            if( m_RM->Match(s, "MI =\\s+(\\d+), MA =\\s+(\\d+), ML =\\s+(\\d+), GV =\\s+(\\d+), LS =\\s+(\\d+), BI =\\s+(\\d+)\\.") )
            {
                if( m_Turn > m_EstimateAlien->TechEstimateTurn )
                {
                    m_EstimateAlien->TechLevels[TECH_MI] = m_RM->GetResultInt(0);
                    m_EstimateAlien->TechLevels[TECH_MA] = m_RM->GetResultInt(1);
                    m_EstimateAlien->TechLevels[TECH_ML] = m_RM->GetResultInt(2);
                    m_EstimateAlien->TechLevels[TECH_GV] = m_RM->GetResultInt(3);
                    m_EstimateAlien->TechLevels[TECH_LS] = m_RM->GetResultInt(4);
                    m_EstimateAlien->TechLevels[TECH_BI] = m_RM->GetResultInt(5);
                    m_EstimateAlien->TechEstimateTurn = m_Turn;
                    m_EstimateAlien = nullptr;
                    m_Phase = m_PhasePreAggregate;
                }
            }
            else
                m_Phase = m_PhasePreAggregate;
        }
        else
        {
            if( m_RM->Match(s, "^Estimate of the technology of SP\\s+([^,;]+)\\s+\\(government name '([^']+)', government type '([^']+)'\\)") )
            {
                m_EstimateAlien = m_GameData->AddAlien(m_Turn, m_RM->Results[0]);
                m_EstimateAlien->GovName = m_RM->Results[1];
                m_EstimateAlien->GovType = m_RM->Results[2];
            }
            else if( Regex("^Estimate of the technology of SP\\s+").Match(s)->Success )
            {
                m_Phase = m_PhasePreAggregate;
                StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
            }
            else
                m_Phase = m_PhasePreAggregate;
        }
        break;

    case PHASE_SYSTEM_SCAN:
        if( String::IsNullOrEmpty(s) )
        {
            if( m_ScanHasPlanets )
            {
                m_Phase = PHASE_GLOBAL;
                m_ScanAlien = nullptr;
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
        break;

    case PHASE_ALIENS_REPORT:
        if( String::IsNullOrEmpty(s) )
        {
            m_Phase = PHASE_GLOBAL;
        }
        else
            MatchAliensReport(s);
        break;

    case PHASE_ORDERS_TEMPLATE:
        if( ! String::IsNullOrEmpty(s) )
        {
            MatchOrdersTemplate(s);
        }
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
    if( m_RM->Match(s, "^Coordinates:\\s+[Xx]\\s+=\\s+(\\d+)\\s+[Yy]\\s+=\\s+(\\d+)\\s+[Zz]\\s+=\\s+(\\d+)") )
    {
        m_Phase = PHASE_SYSTEM_SCAN;
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);
        m_ScanHasPlanets = false;

        // Set the home system
        if( m_ScanAlien )
            m_ScanAlien->HomeSystem = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);

        return true;
    }
    return false;
}

void Report::MatchPlanetScan(String ^s)
{
    //                          0:plNum   1:dia    2:gv            3:tc       4:pc    5:mining diff
    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.(\\d+)\\s+") )
    {
        int plNum = m_RM->GetResultInt(0);
        Planet ^planet = gcnew Planet(
            m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ),
            plNum,
            m_RM->GetResultInt(1),
            m_RM->GetResultFloat(2),
            m_RM->GetResultInt(3),
            m_RM->GetResultInt(4),
            m_RM->GetResultInt(5) * 100 + m_RM->GetResultInt(6) );

        // Try reading LSN
        if( m_RM->Match(s, "(\\d+)\\s+") )
        {
            planet->LSN = m_RM->GetResultInt(0);

            if( m_ScanAlien && planet->LSN == 0 )
            {
                m_ScanAlien->HomePlanet = plNum;
                m_ScanAlien->AtmReq->TempClass  = planet->TempClass;
                m_ScanAlien->AtmReq->PressClass = planet->PressClass;
            }
        }
        else if( m_RM->Match(s, "[x?\\-]+\\s+") )
        {
            // just skip from input
        }
        else
            throw gcnew FHUIParsingException("Report contains invalid planetary scan (LSN)");

        // Scan gases
        if( m_RM->Match(s, "^No atmosphere\\s*") )
        {
            // just skip from input
        }
        else
        {
            bool bGasMatched = false;
            while( m_RM->Match(s, ",?(\\w+)\\((\\d+)%\\)") )
            {
                bGasMatched = true;
                int gas = FHStrings::GasFromString(m_RM->Results[0]);
                planet->Atmosphere[gas] = m_RM->GetResultInt(1);
            }
            if( !bGasMatched )
                throw gcnew FHUIParsingException("Report contains invalid planetary scan (atmosphere)");
        }

        if( !String::IsNullOrEmpty(s) )
            planet->Comment = s;

        m_GameData->AddPlanetScan( m_Turn, m_ScanX, m_ScanY, m_ScanZ, planet );
        m_ScanHasPlanets = true;
    }
}

bool Report::MatchTech(String ^s, String ^techName, TechType tech)
{
    String ^e2 = String::Format("{0} = (\\d+)/(\\d+)", techName);
    String ^e1 = String::Format("{0} = (\\d+)", techName);
    if( m_RM->Match(s, e2) )
    {
        m_GameData->SetTechLevel(
            m_Turn,
            m_GameData->GetSpecies(),
            tech,
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1));
        return true;
    }
    if( m_RM->Match(s, e1) )
    {
        m_GameData->SetTechLevel(
            m_Turn,
            m_GameData->GetSpecies(),
            tech,
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(0));
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
        if( m_RM->Match(s, "^(HOME|COLONY)\\s+PLANET: PL\\s+") )
        {
            if( m_RM->Results[0][0] == 'C' )
                planetType = PLANET_COLONY;
        }
        else if( m_RM->Match(s, "^(MINING|RESORT)\\s+COLONY: PL\\s+") )
        {
            if( m_RM->Results[0][0] == 'M' )
                planetType = PLANET_COLONY_MINING;
            else
                planetType = PLANET_COLONY_RESORT;
        }
        else
            throw gcnew FHUIParsingException(
                String::Format("Unknown colony type in report: {0}", s) );

        if( m_RM->Match(s, "([^,;]+)\\s+Coordinates: x = (\\d+), y = (\\d+), z = (\\d+), planet number (\\d+)") )
        {
            String ^plName = m_RM->Results[0];
            plName = plName->TrimEnd(' ');

            StarSystem ^system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                m_RM->GetResultInt(3) );

            int plNum = m_RM->GetResultInt(4);
 
            m_ScanColony = m_GameData->AddColony(
                m_Turn,
                m_GameData->GetSpecies(),
                plName,
                system,
                plNum );

            // Species have colony here, so system is visited
            system->LastVisited = m_Turn;

            if( m_ScanColony == nullptr )
            {
                m_Phase = m_PhasePreAggregate;
                return;
            }

            m_ScanColony->PlanetType = planetType;
        }
        else
            throw gcnew FHUIParsingException(
                String::Format("Unable to parse colony coordinates: {0}", s) );

        return;
    }

    // Remaining entries
    switch( m_ScanColony->PlanetType )
    {
    case PLANET_HOME:
    case PLANET_COLONY:
        if( m_RM->Match(s, "^Total available for spending this turn = (\\d+) - (\\d+) = \\d+") )
        {
            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->EUProd );
        }
        break;

    case PLANET_COLONY_MINING:
        if( m_RM->Match(s, "^This mining colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->EUProd );
        }
        break;

    case PLANET_COLONY_RESORT:
        if( m_RM->Match(s, "^This resort colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_Turn, m_ScanColony->EUProd );
        }
        break;
    }

    if( m_RM->Match(s, "^Economic efficiency = (\\d+)%") )
    {
        m_ScanColony->EconomicEff = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^IMPORTANT!  This planet is actively hiding from alien observation!") )
    {
        m_ScanColony->Hidden = true;
    }
    else if( m_RM->Match(s, "^WARNING!  This planet is currently under siege and will remain") )
    {
        m_ScanColony->UnderSiege = true;
    }
    else if( m_RM->Match(s, "^Production penalty = (\\d+)% \\(LSN = (\\d+)\\)") )
    {
        m_ScanColony->ProdPenalty = m_RM->GetResultInt(0);
        m_ScanColony->LSN = m_RM->GetResultInt(1);
    }
    else if( m_RM->Match(s, "^Mining base = (\\d+)\\.(\\d+) \\(MI = \\d+, MD = (\\d+)\\.(\\d+)\\)") )
    {
        m_ScanColony->MiBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
        m_ScanColony->MiDiff = m_RM->GetResultInt(2) * 100 + m_RM->GetResultInt(3);
    }
    else if( m_RM->Match(s, "^Raw Material Units \\(RM,C1\\) carried over from last turn = (\\d+)") )
    {
        m_ScanColony->Inventory[INV_RM] = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Manufacturing base = (\\d+)\\.(\\d+) \\(") )
    {
        m_ScanColony->MaBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
    }
    else if( m_RM->Match(s, "^Shipyard capacity = (\\d+)") )
    {
        m_ScanColony->Shipyards = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Available population units = (\\d+)") )
    {
        m_ScanColony->AvailPop = m_RM->GetResultInt(0);
    }
    else if( Regex("^Planetary inventory:").Match(s)->Success )
        m_Phase = PHASE_COLONY_INVENTORY;
    else if( Regex("^Ships at PL [^,;]+:").Match(s)->Success )
        m_Phase = PHASE_COLONY_SHIPS;
}

void Report::MatchColonyInventoryScan(String ^s)
{
    if( m_RM->Match(s, "^[\\w\\s.-]+\\(PD,C3\\) = (\\d+) (warship equivalence = \\d+ tons)") )
        m_ScanColony->Inventory[INV_PD] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SU,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(CU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_CU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(IU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_IU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(AU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_AU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FS,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FS] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FD,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FD] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(DR,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_DR] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FM,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FM] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FJ,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FJ] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GW,C100\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GW] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GT,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GT] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(JP,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_JP] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(TP,C100\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_TP] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU1,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU1] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU2,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU2] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU3,C15\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU3] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU4,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU4] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU5,C25\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU5] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU6,C30\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU6] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU7,C35\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU7] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU8,C40\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU8] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU9,C45\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU9] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG1,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG1] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG2,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG2] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG3,C15\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG3] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG4,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG4] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG5,C25\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG5] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG6,C30\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG6] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG7,C35\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG7] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG8,C40\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG8] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG9,C45\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG9] = m_RM->GetResultInt(0);
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

    if( m_RM->Match(s, "^BAS\\s+") )
        type = SHIP_BAS;
    else if( m_RM->Match(s, "^TR(\\d+)([Ss]?)\\s+") ) 
    {
        type = SHIP_TR;
        size = m_RM->GetResultInt(0);
        subLight = 0 == String::Compare( m_RM->Results[1]->ToLower(), "s" );
    }
    else
    {
        try
        {
            if( m_RM->Match(s, "^([A-Za-z]{2})([Ss]?)\\s+") )
            {
                type = FHStrings::ShipFromString( m_RM->Results[0] );
                subLight = 0 == String::Compare( m_RM->Results[1]->ToLower(), "s" );
            }
            else
                return;
        }
        catch( FHUIParsingException^ )
        {
            return;
        }
    }

    bool bMatch     = false;
    bool bInFD      = false;
    bool bIncomplete= false;
    String ^name    = nullptr;
    int age         = 0;
    String ^loc     = nullptr;
    int capacity    = 0;
    int planetNum   = -1;
    ShipLocType location = SHIP_LOC_DEEP_SPACE;

    if( m_RM->Match(s, "^\\?\\?\\? \\(([DOLdol])") )
    {
        bMatch  = true;
        bInFD   = true;
        name    = String::Format("??? ({0})", ++m_PirateShipsCnt);
        loc     = m_RM->Results[0]->ToLower();
    }
    else if( m_RM->Match(s, "^([^,;]+) \\(") )
    {
        name    = m_RM->Results[0];
        if( m_RM->Match(s, "^C") )
        {
            bMatch  = true;
            age     = 0;
            bIncomplete = true;
        }
        else if( m_RM->Match(s, "^A(\\d+),([DOLdol])") )
        {
            bMatch  = true;
            age     = m_RM->GetResultInt(0);
            loc     = m_RM->Results[1]->ToLower();
        }
    }

    if( bMatch )
    {
        StarSystem ^system;
        if( bColony )
            system = m_ScanColony->System;
        else
        {
            try
            {
                system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);
            }
            catch( FHUIDataIntegrityException^ )
            {   // When reading other ships and planets,
                // ship may be outside of any system just in deep, empty space
                system = gcnew StarSystem(m_ScanX, m_ScanY, m_ScanZ, "deep space");
            }
        }
        system->LastVisited = m_Turn;

        if( bIncomplete )
        {
            location = SHIP_LOC_LANDED;
            if( bColony )
                planetNum = m_ScanColony->PlanetNum;
            else
                throw gcnew FHUIParsingException("Ship under construction outside colony???");
        }
        else
        {
            switch( loc[0] )
            {
            case 'd':
                location = SHIP_LOC_DEEP_SPACE;
                break;
            case 'o':
                location = SHIP_LOC_ORBIT;
                break;
            case 'l':
                location = SHIP_LOC_LANDED;
                break;
            default:
                throw gcnew FHUIParsingException("Invalid ship location: " + loc[0]);
            }

            if( location != SHIP_LOC_DEEP_SPACE )
            {
                if( m_RM->Match(s, "^(\\d+)") )
                    planetNum = m_RM->GetResultInt(0);
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' location.", name));
            }
        }

        if( type == SHIP_BAS )
        {
            if( m_RM->Match(s, "^,(\\d+) tons") )
                size = m_RM->GetResultInt(0);
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse starbase '{0}' size.", name));
        }

        if( bColony )
        {
            if( m_RM->Match(s, "^\\)\\s+(\\d+)\\s*") )
                capacity = m_RM->GetResultInt(0);
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse ship '{0}' capacity.", name));
        }
        else
        {
            if( !m_RM->Match(s, "^\\)\\s*") )
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse ship '{0}'.", name));
        }

        if( m_Phase != PHASE_ALIENS_REPORT )
        {
            m_ScanShip = m_GameData->AddShip(
                m_Turn, m_GameData->GetSpecies(), type, name, subLight, system);
            if( m_ScanShip == nullptr )
                return;

            if( bIncomplete )
            {
                if( m_RM->Match(s, "^Left to pay = (\\d+)$") )
                {
                    int amount = m_RM->GetResultInt(0);
                    m_ScanShip->EUToComplete = amount;
                    m_ScanShip->CanJump = false;
                }
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' completion cost.", name));
            }

            while( !String::IsNullOrEmpty(s) )
            {
                if( m_RM->Match(s, "^,?\\s*(\\d+)\\s+(\\w+)\\s*") )
                {
                    int amount = m_RM->GetResultInt(0);
                    InventoryType inv = FHStrings::InvFromString( m_RM->Results[1] );
                    m_ScanShip->Cargo[inv] = amount;
                }
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' inventory.", name));
            }
        }
        else
        {   // In aliens report phase read owning species
            if( m_RM->Match(s, "^SP ([^,;]+)\\s*$") )
            {
                String ^spName = m_RM->Results[0];
                Alien ^sp = m_GameData->AddAlien(m_Turn, spName);
                m_ScanShip = m_GameData->AddShip(
                    m_Turn, sp, type, name, subLight, system);

                if( bInFD )
                {
                    sp->Relation  = SP_PIRATE;
                    if( m_ScanShip )
                        m_ScanShip->IsPirate = true;
                }
            }
        }
    }

    m_ScanShip->Age         = age;
    m_ScanShip->Size        = size;
    m_ScanShip->Location    = location;
    m_ScanShip->PlanetNum   = planetNum;
    m_ScanShip->CalculateCapacity();
    if( capacity != 0 &&
        capacity != m_ScanShip->Capacity )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Calculated ship '{0}' capacity ({1}) doesn't match report ({2}).",
                name, m_ScanShip->Capacity, capacity) );
    }
}

void Report::MatchOtherPlanetsShipsScan(String ^s)
{
    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+#(\\d+)\\s+PL\\s+([^,]+),?(.*)$") )
    {
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);

        StarSystem^ system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);

        int plNum = m_RM->GetResultInt(3);
        String^ plName = m_RM->Results[4];
        String^ inventory = m_RM->Results[5];

        m_GameData->AddPlanetName( m_Turn, system, plNum, plName);

        if( inventory->Length )
        {
            // Treat as colony of size 0
            Colony^ colony = m_GameData->AddColony(
                m_Turn,
                m_GameData->GetSpecies(),
                plName,
                system,
                plNum );

            colony->PlanetType = PLANET_COLONY;
            colony->LSN = system->Planets[plNum - 1]->LSN;
            colony->MiDiff = system->Planets[plNum - 1]->MiDiff;
            colony->MiBase = 0;
            colony->MaBase = 0;
            colony->EconomicEff = 0;

            // Species have colony here, so system is visited
            // TODO: Not sure if no CUs are present
            system->LastVisited = m_Turn;

            // TODO: Parse inventory
        }
    }

    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+") )
    {
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);
    }
    MatchShipScan(s, false);
}

void Report::MatchAliensReport(String ^s)
{
    PlanetType plType = PLANET_MAX;
    if( m_RM->Match(s, "^Home planet PL\\s+") )
        plType = PLANET_HOME;
    else if( m_RM->Match(s, "^Colony planet PL\\s+") )
        plType = PLANET_COLONY;
    else if( m_RM->Match(s, "^Mining colony PL\\s+") )
        plType = PLANET_COLONY_MINING;
    else if( m_RM->Match(s, "^Resort colony PL\\s+") )
        plType = PLANET_COLONY_RESORT;
    else if( m_RM->Match(s, "^Uncolonized planet PL\\s+") )
        plType = PLANET_UNCOLONIZED;
    if( plType != PLANET_MAX )
    {
        if( m_RM->Match(s, "^([^,;]+)\\s+\\(pl #(\\d+)\\)\\s+SP\\s+([^,;]+)\\s*$") )
        {
            StarSystem ^system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ);
            String ^name = m_RM->Results[0];
            int plNum    = m_RM->GetResultInt(1);
            Alien ^sp    = m_GameData->AddAlien(m_Turn, m_RM->Results[2]);

            m_ScanColony = m_GameData->AddColony(m_Turn, sp, name, system, plNum);
            if( m_ScanColony )
            {
                m_ScanColony->LastSeen = Math::Max(m_Turn, m_ScanColony->LastSeen);
                m_ScanColony->PlanetType = plType;

                if( plType == PLANET_HOME )
                {
                    sp->HomeSystem = system;
                    sp->HomePlanet = plNum;

                    system->HomeSpecies = sp;
                }
            }
        }
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(Economic base is approximately (\\d+)\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
        {
            m_ScanColony->MiBase = 10 * m_RM->GetResultInt(0);
            m_ScanColony->MaBase = 0;
        }
    }
    else if( m_ScanColony && s == "(No economic base.)" )
    {
        if( m_ScanColony->LastSeen == m_Turn )
        {
            m_ScanColony->MiBase = 0;
            m_ScanColony->MaBase = 0;
        }
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(There are (\\d+) Planetary Defense Units on the planet\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
            m_ScanColony->Inventory[INV_PD] = m_RM->GetResultInt(0);
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(There are (\\d+) shipyards on the planet\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
            m_ScanColony->Shipyards = m_RM->GetResultInt(0);
    }
    else
        MatchShipScan(s, false);
}

void Report::MatchOrdersTemplate(String ^s)
{
    String^ line = s->Replace("\t", " ");

    // INSTALL
    if( m_RM->Match(s, "^Install\\s+\\d+\\s+\\S+\\s+PL\\s+([^,]+)$") )
    {
        StarSystem^ system = m_GameData->GetStarSystem(m_RM->Results[0]);  
        if ( system )
        {
            m_GameData->SetAutoOrderPreDeparture(m_Turn, system, line );
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("INSTALL order for unknown planet: PL {0}", m_RM->Results[0]) );
    }

    // UNLOAD
    if( m_RM->Match(s, "^Unload\\s+(\\S+)\\s+([^,]+)$") )
    {
        Ship^ ship = m_GameData->GetShip(m_RM->Results[1]);  
        if ( ship )
        {
            m_GameData->SetAutoOrderPreDeparture(m_Turn, ship->System, line );
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("UNLOAD order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // JUMP
    if( m_RM->Match(s, "^Jump\\s+(\\S+)\\s+([^,]+),.+$") )
    {
        Ship^ ship = m_GameData->GetShip(m_RM->Results[1]);  
        if ( ship )
        {
            m_GameData->SetAutoOrderJumps(m_Turn, ship, line );
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("JUMP order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // START PRODUCTION
    if( m_RM->Match(s, "^PRODUCTION\\s+PL\\s+([^,]+)$") )
    {
        Colony^ colony = m_GameData->GetColony(m_RM->Results[0]);
        if ( colony )
        {
            m_ColonyProduction = colony;
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("PRODUCTION order for unknown colony: PL {0}", m_RM->Results[0]) );
    }

    // DEVELOP
    if( m_RM->Match(s, "^Develop\\s+(\\d+)$" ) )
    {
        m_GameData->SetAutoOrderProduction(
            m_Turn, m_ColonyProduction, line, m_RM->GetResultInt(0) );
        return;
    }
    if( m_RM->Match(s, "^Develop\\s+(\\d+)\\s+PL\\s+([^,]+)$" ) )
    {
        m_GameData->SetAutoOrderProduction(
            m_Turn, m_ColonyProduction, line, m_RM->GetResultInt(0) );
        return;
    }
    if( m_RM->Match(s, "^Develop\\s+PL\\s+([^,]+),\\s+TR(\\d+)\\s+([^,]+)$") )
    {
        m_GameData->SetAutoOrderProduction(
            m_Turn, m_ColonyProduction, line, Calculators::TransportCapacity(m_RM->GetResultInt(1)) );
        return;
    }

    // CONTINUE
    if( m_RM->Match(s, "^Continue\\s+(\\S+)\\s+([^,]+),\\s+(\\d+).*$") )
    {
        Ship^ ship = m_GameData->GetShip(m_RM->Results[1]);
        if ( ship )
        {
            m_GameData->SetAutoOrderProduction(m_Turn, m_ColonyProduction, line, m_RM->GetResultInt(2) );
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("CONTINUE order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // RECYCLE
    if( m_RM->Match(s, "^Recycle\\s+(\\d+)\\s+RM$") )
    {
        m_GameData->SetAutoOrderProduction( m_Turn, m_ColonyProduction, line, -m_RM->GetResultInt(0)/5 );
        return;
    } 
    if( m_RM->Match(s, "^Recycle\\s+\\S+\\s+([^,]+)$") )
    {
        m_GameData->SetAutoOrderProduction(
            m_Turn, m_ColonyProduction, line, -m_GameData->GetShip(m_RM->Results[0])->GetRecycleValue() );
        return;
    }

    // SCAN (ignore) assume it is generated for all scouts
    // AUTO (ignore) assume it is enabled when at least one other order is found
}

void Report::StartLineAggregate(PhaseType phase, String ^s, int maxLines)
{
    m_bParsingAggregate = true;
    m_StringAggregate = s;
    m_AggregateMaxLines = maxLines;
    m_PhasePreAggregate = m_Phase;
    m_Phase = phase;
}

String^ Report::FinishLineAggregate(bool resetPhase)
{
    m_bParsingAggregate = false;
    String ^ret = m_StringAggregate;
    m_StringAggregate = nullptr;
    if( resetPhase )
        m_Phase = m_PhasePreAggregate;
    return ret;
}

} // end namespace FHUI
