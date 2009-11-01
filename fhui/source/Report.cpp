#include "StdAfx.h"
#include "Report.h"

namespace FHUI
{

struct ReportPhaseStr
{
    PhaseType    type;
    const char  *str;
};

static ReportPhaseStr s_ReportPhaseStrings[PHASE_MAX] =
{
    { PHASE_INIT,                "INIT" },
    { PHASE_GLOBAL,              "GLOBAL" },
    { PHASE_ORDERS_COMBAT,       "ORDERS_COMBAT" },
    { PHASE_ORDERS_PRE_DEP,      "ORDERS_PRE_DEP" },
    { PHASE_ORDERS_JUMP,         "ORDERS_JUMP" },
    { PHASE_ORDERS_PROD,         "ORDERS_PROD" },
    { PHASE_ORDERS_POST_ARRIVAL, "ORDERS_POST_ARRIVAL" },
    { PHASE_ORDERS_STRIKE,       "ORDERS_STRIKE" },
    { PHASE_MESSAGE,             "MESSAGE" },
    { PHASE_SPECIES_MET,         "SPECIES_MET" },
    { PHASE_SPECIES_ALLIES,      "SPECIES_ALLIES" },
    { PHASE_SPECIES_ENEMIES,     "SPECIES_ENEMIES" },
    { PHASE_SYSTEM_SCAN,         "SYSTEM_SCAN" },
    { PHASE_ALIEN_ESTIMATE,      "ALIEN_ESTIMATE" },
    { PHASE_COLONY,              "COLONY" },
    { PHASE_COLONY_INVENTORY,    "COLONY_INVENTORY" },
    { PHASE_COLONY_SHIPS,        "COLONY_SHIPS" },
    { PHASE_OTHER_PLANETS_SHIPS, "OTHER_PLANETS_SHIPS" },
    { PHASE_ALIENS_REPORT,       "ALIENS_REPORT" },
    { PHASE_ORDERS_TEMPLATE,     "ORDERS_TEMPLATE" },
    { PHASE_TECH_LEVELS,         "TECH_LEVELS" }
};

String^ Report::PhaseToString(PhaseType phase)
{
    if( phase >= PHASE_MAX )
        return "???";
    for( int i = 0; i < PHASE_MAX; ++i )
        if( s_ReportPhaseStrings[i].type == phase )
            return gcnew String(s_ReportPhaseStrings[i].str);

    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid ship type: {0}.", (int)phase) );
}

Report::Report(GameData ^gd, CommandManager^ cm, RegexMatcher ^rm, bool verbose)
    : m_GameData(gd)
    , m_CommandMgr(cm)
    , m_RM(rm)
    , m_Phase(PHASE_INIT)
    , m_PhasePreAggregate(PHASE_GLOBAL)
    , m_Turn(-1)
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
    , m_Content(gcnew List<String^>)
    , m_Verbose(verbose)
{
}

bool Report::IsValid()
{
    return
        GameData::Player != nullptr &&
        GameData::Player->Name != nullptr &&
        GameData::Player->AtmReq->IsValid() &&
        m_Phase == PHASE_ORDERS_TEMPLATE;
}

String^ Report::GetText()
{
    String^ text;
    for each( String^ line in m_Content)
    {
        text += line + "\n";
    }
    return text;
}

bool Report::Verify(String^ fileName)
{
    m_Input = File::OpenText(fileName);
    m_Turn = -1;

    while( String ^line = GetLine() ) 
    {
        if( String::IsNullOrEmpty(line) )
            continue;

        if( m_RM->Match(line, "^EVENT LOG FOR TURN (\\d+)") )
        {   // String matched, but keep looking for "Start of turn..."
            m_Turn = m_RM->GetResultInt(0);
        }
        else if( m_RM->Match(line, "^START OF TURN (\\d+)") )
        {
            m_Turn = m_RM->GetResultInt(0);
            break;  // We're satisfied now
        }
    }

    return m_Turn >= 0;
}

void Report::Parse(String^ fileName)
{
    m_Input = File::OpenText(fileName);
    String^ line;

    try
    {
        while ( line = GetLine() )
        {
            ParseInternal( line );
        }
    }
    catch( Exception ^ex )
    {
        throw gcnew FHUIParsingException(
            String::Format("Error occured while parsing report: {0}, line {1}:\r\n{2}\r\nError description:\r\n  {3}",
                fileName,
                m_Content->Count,
                line,
                ex->Message),
            ex );
    }
    finally
    {
        m_Input->Close();
    }
}

String^ Report::GetLine()
{
    String^ line = m_Input->ReadLine();

    if( m_Content )
        m_Content->Add( line );

    if( line )
        return line->Trim();
    else
        return nullptr; //EOF
}

bool Report::ParseInternal(String ^s)
{
    Debug::WriteLineIf( m_Verbose, String::Format( "{0,-20} \"{1}\"", PhaseToString(m_Phase), s) );

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

    bool recognized = false;

    switch( m_Phase )
    {
    case PHASE_INIT:
        m_Phase = PHASE_GLOBAL;
        /* fall through */

    case PHASE_GLOBAL:
        recognized = MatchPhaseGlobal(s);
        break;

    case PHASE_MESSAGE:
        recognized = MatchPhaseMessage(s);
        break;

    case PHASE_SPECIES_MET:
        recognized = MatchPhaseSpeciesMet(s);
        break;

    case PHASE_SPECIES_ALLIES:
        recognized = MatchPhaseSpeciesAllies(s);
        break;

    case PHASE_SPECIES_ENEMIES:
        recognized = MatchPhaseSpeciesEnemies(s);
        break;

    case PHASE_ORDERS_COMBAT:
        recognized = MatchPhaseOrdersCombat(s);
        break;

    case PHASE_ORDERS_PRE_DEP:
        recognized = MatchPhaseOrdersPreDep(s);
        break;

    case PHASE_ORDERS_JUMP:
        recognized = MatchPhaseOrdersJump(s);
        break;

    case PHASE_ORDERS_PROD:
        recognized = MatchPhaseOrdersProd(s);
        break;

    case PHASE_ORDERS_POST_ARRIVAL:
        recognized = MatchPhaseOrdersPostArr(s);
        break;

    case PHASE_ORDERS_STRIKE:
        recognized = MatchPhaseOrdersStrike(s);
        break;

    case PHASE_TECH_LEVELS:
        recognized = MatchPhaseTechLevels(s);
        break;

    case PHASE_ALIEN_ESTIMATE:
        recognized = MatchPhaseAlienEstimate(s);
        break;

    case PHASE_SYSTEM_SCAN:
        recognized = MatchPhaseSystemScan(s);
        break;

    case PHASE_COLONY:
        recognized = MatchPhaseColony(s);
        break;

    case PHASE_COLONY_INVENTORY:
        recognized = MatchPhaseColonyInventory(s);
        break;

    case PHASE_COLONY_SHIPS:
        recognized = MatchPhaseColonyShips(s);
        break;

    case PHASE_OTHER_PLANETS_SHIPS:
        recognized = MatchPhaseOtherPlanetsShips(s);
        break;

    case PHASE_ALIENS_REPORT:
        recognized = MatchPhaseAliensReport(s);
        break;

    case PHASE_ORDERS_TEMPLATE:
        recognized = MatchPhaseOrdersTemplate(s);
        break;

    default:
        return false;
    }

    if( ! recognized )
    {
        //Debug::WriteLine( String::Format("{0,-20} unrecognized line '{1}'", PhaseToString(m_Phase), s) );
    }

    return recognized;
}

bool Report::MatchPhaseGlobal(String ^s)
{
    if( String::IsNullOrEmpty(s) )
        return true;

    // Turn number
    if( m_RM->Match(s, "^EVENT LOG FOR TURN (\\d+)") )
    {
        m_Turn = m_RM->GetResultInt(0);
        return true;
    }

    if( m_RM->Match(s, "^START OF TURN (\\d+)") )
    {
        m_Turn = m_RM->GetResultInt(0);
        return true;
    }

    // Species name
    if( m_RM->Match(s, "^Species name: ([^,;]+)") )
    {
        m_GameData->SetSpecies( m_RM->Results[0] );
        return true;
    }

    // Atmospheric requirements
    if( m_RM->Match(s, "^Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
    {
        m_GameData->SetAtmosphereReq(
            FHStrings::GasFromString( m_RM->Results[2] ), // required gas
            m_RM->GetResultInt(0),               // min level
            m_RM->GetResultInt(1) );             // max level
        return true;
    }

    if( m_RM->MatchList(s, "^Neutral Gases:", "([A-Za-z0-9]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
            m_GameData->SetAtmosphereNeutral(
                FHStrings::GasFromString(m_RM->Results[i]) );
        return true;
    }

    if( m_RM->MatchList(s, "^Poisonous Gases:", "([A-Za-z0-9]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
            m_GameData->SetAtmospherePoisonous(
                FHStrings::GasFromString(m_RM->Results[i]) );
        return true;
    }

    // Economy
    if( m_RM->Match(s, "^Economic units = (\\d+)") )
    {
        m_GameData->SetTurnStartEU( m_RM->GetResultInt(0) );
        return true;
    }

    // Fleet maintenance
    if( m_RM->Match(s, "^Fleet maintenance cost = (\\d+) \\((\\d+)\\.(\\d+)% of total production\\)") )
    {
        m_GameData->SetFleetCost(
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1)*100 + m_RM->GetResultInt(2) );
        return true;
    }

    // Species...
    if( Regex("^Species met:").Match(s)->Success )
    {
        StartLineAggregate(PHASE_SPECIES_MET, s, AGGREGATE_LINES_MAX);
        return true;
    }

    if( m_RM->Match(s, "^Scan of home star system for SP\\s+([^,;]+):$") )
    {
        m_ScanAlien = m_GameData->AddAlien( m_RM->Results[0] );
        m_GameData->SetSpecies( m_RM->Results[0] );
        return true;
    }
    
    if( Regex("^Allies:").Match(s)->Success )
    {
        StartLineAggregate(PHASE_SPECIES_ALLIES, s, AGGREGATE_LINES_MAX);
        return true;
    }
    
    if( Regex("^Enemies:").Match(s)->Success )
    {
        StartLineAggregate(PHASE_SPECIES_ENEMIES, s, AGGREGATE_LINES_MAX);
        return true;
    }

    if( m_RM->Match(s, "^Government name:\\s+(.+)$") )
    {
        GameData::Player->GovName = m_RM->Results[0];
        return true;
    }

    if( m_RM->Match(s, "^Government type:\\s+(.+)$") )
    {
        GameData::Player->GovType = m_RM->Results[0];
        return true;
    }

    if( m_RM->Match(s, "^Aliens at\\s+x\\s+=\\s+(\\d+), y\\s+=\\s+(\\d+), z\\s+=\\s+(\\d+)") )
    {
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);
        m_Phase = PHASE_ALIENS_REPORT;
        return true;
    }

    if( Regex("^Estimate of the technology of SP ").Match(s)->Success )
    {
        m_EstimateAlien = nullptr;
        StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
        return true;
    }

    // Tech levels
    if( Regex("^Tech Levels:").Match(s)->Success )
    {
        m_Phase = PHASE_TECH_LEVELS;
        return true;
    }

    // System scan
    if( MatchSystemScanStart(s) )
    {
        return true;
    }
    
    if( Regex("^\\w+ PLANET: PL.+").Match(s)->Success ||
        Regex("^\\w+ COLONY: PL.+").Match(s)->Success )
    {
        StartLineAggregate(PHASE_COLONY, s, 1);
        return true;
    }

    if( Regex("^Other planets and ships:").Match(s)->Success )
    {
        m_Phase = PHASE_OTHER_PLANETS_SHIPS;
        return true;
    }

    if( Regex("^Combat orders:").Match(s)->Success )
    {
        m_Phase = PHASE_ORDERS_COMBAT;
        return true;
    }

    // Message
    if( m_RM->Match(s, "^You received the following message from SP ([^,;]+):") )
    {
        m_ScanAlien = m_GameData->AddAlien(m_RM->Results[0]);
        m_ScanMessage = "";
        m_Phase = PHASE_MESSAGE;
        return true;
    }

    // Intelligence
    if( m_RM->Match(s, "^INFO SP ([^,;]+)\\s*;\\s*") )
    {
        MatchAlienInfo(s, m_GameData->AddAlien(m_RM->Results[0]));
        return true;
    }
    
    // Ship misjump
    if( m_RM->Match(s, "^!!! [A-Za-z0-9]+ ([^,;]+) mis-jumped to \\d+ \\d+ \\d+!$") )
    {
        m_GameData->AddMishap(m_RM->Results[0]);
        return true;
    }

    if( Regex("^ORDER SECTION.").Match(s)->Success )
    {
        m_Phase = PHASE_ORDERS_TEMPLATE;
        m_TemplatePhase = CommandPhase::Custom;
        return true;
    }

    return true;
}

bool Report::MatchPhaseMessage(String ^s)
{
    if( s == "*** End of Message ***" )
    {
        m_ScanAlien->LastMessageRecv = m_ScanMessage;
        m_ScanAlien->LastMessageRecvTurn = m_GameData->CurrentTurn - 1;
        m_ScanMessage = nullptr;
        m_Phase = PHASE_GLOBAL;
    }
    else
    {
        m_ScanMessage += s + "\r\n";
        if( m_RM->Match(s, "([a-zA-Z0-9_.-]+@[a-zA-Z0-9_.]+\\.[a-zA-Z0-9_]+)") )
            m_ScanAlien->Email = m_RM->Results[0];
    }
    return true;
}

bool Report::MatchPhaseSpeciesMet(String ^s)
{
    if( m_RM->MatchList(FinishLineAggregate(true), "^Species met:", "SP\\s+([^,;]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
            m_GameData->AddAlien(m_RM->Results[i]);
    }
    return false;
}

bool Report::MatchPhaseSpeciesAllies(String ^s)
{
    if( m_RM->MatchList(FinishLineAggregate(true), "^Allies:", "SP\\s+([^,;]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
            m_GameData->SetAlienRelation(m_RM->Results[i], SP_ALLY);
    }
    return false;
}

bool Report::MatchPhaseSpeciesEnemies(String ^s)
{
    if( m_RM->MatchList(FinishLineAggregate(true), "^Enemies:", "SP\\s+([^,;]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
            m_GameData->SetAlienRelation(m_RM->Results[i], SP_ENEMY);
    }
    return false;
}

bool Report::MatchPhaseOrdersCombat(String ^s)
{
    if( Regex("^Pre-departure orders:").Match(s)->Success )
    {
        m_Phase = PHASE_ORDERS_PRE_DEP;
    }
    return true;
}

bool Report::MatchPhaseOrdersPreDep(String ^s)
{
    if( m_RM->Match(s, "Named PL ([^,;]+) at (\\d+)\\s+(\\d+)\\s+(\\d+), planet #(\\d+)\\.") )
    {
        // Ignore, planet will be parsed later in the turn status
    }
    else if( Regex("^Jump orders:").Match(s)->Success )
        m_Phase = PHASE_ORDERS_JUMP;
    // System scan
    else if( MatchSystemScanStart(s) )
    {}
    else if( MatchMessageSent(s) )
    {}
    return true;
}

bool Report::MatchPhaseOrdersJump(String ^s)
{
    //if( m_RM->Match(s, "^TR1 ([^,;]+) will jump via natural wormhole at\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.$") )
    if( m_RM->Match(s, "^[A-Za-z0-9]+\\s+([^,;]+) will jump via natural wormhole at\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.$") )
    {
        m_GameData->AddWormholeJump(
            m_RM->Results[0],
            GameData::GetSystemId(
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                m_RM->GetResultInt(3) ) );
    }
    else if( m_RM->Match(s, "^The star system at\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+was marked as visited\\.$") )
    {
        GameData::GetStarSystem(
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2),
            false)->IsMarkedVisited = true;
    }
    else if( Regex("^Production orders:").Match(s)->Success )
        m_Phase = PHASE_ORDERS_PROD;
    return true;
}

bool Report::MatchPhaseOrdersProd(String ^s)
{
    if( Regex("^Post-arrival orders:").Match(s)->Success )
        m_Phase = PHASE_ORDERS_POST_ARRIVAL;
    else if( Regex("^Estimate of the technology of SP ").Match(s)->Success )
    {
        m_EstimateAlien = nullptr;
        StartLineAggregate(PHASE_ALIEN_ESTIMATE, s, 1);
    }
    return true;
}

bool Report::MatchPhaseOrdersPostArr(String ^s)
{
    if( Regex("^Strike orders:").Match(s)->Success )
        m_Phase = PHASE_ORDERS_STRIKE;
    // System scan
    else if( MatchSystemScanStart(s) )
    {}
    else if( MatchMessageSent(s) )
    {}
    return true;
}

bool Report::MatchPhaseOrdersStrike(String ^s)
{
    if( Regex("^Other events:").Match(s)->Success ||
        Regex("^SPECIES STATUS$").Match(s)->Success )   // in case no other events happened
        m_Phase = PHASE_GLOBAL;
    // It may happen that no other events were present, but there's incoming message
    else if( m_RM->Match(s, "^You received the following message from SP ([^,;]+):") )
    {
        m_ScanAlien = m_GameData->AddAlien(m_RM->Results[0]);
        m_Phase = PHASE_MESSAGE;
    }
    return true;
}

bool Report::MatchPhaseTechLevels(String ^s)
{
    if( MatchTech(s, "^Mining", TECH_MI) ||
        MatchTech(s, "^Manufacturing", TECH_MA) ||
        MatchTech(s, "^Military", TECH_ML) ||
        MatchTech(s, "^Gravitics", TECH_GV) ||
        MatchTech(s, "^Life Support", TECH_LS) )
        return true;
    else if( MatchTech(s, "Biology", TECH_BI) )
        m_Phase = PHASE_GLOBAL;
    else
        return false;
    return true;
}

bool Report::MatchPhaseAlienEstimate(String ^s)
{
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
            m_EstimateAlien = m_GameData->AddAlien(m_RM->Results[0]);
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
    return true;
}

bool Report::MatchPhaseSystemScan(String ^s)
{
    if( String::IsNullOrEmpty(s) )
    {
        if( m_ScanHasPlanets )
        {
            m_Phase = m_PhasePreScan;
            m_ScanAlien = nullptr;
        }
    }
    else
        MatchPlanetScan(s);
    return true;
}

bool Report::MatchPhaseColony(String ^s)
{
    if( MatchSectionEnd(s) )
    {
        m_Phase = PHASE_GLOBAL;
        m_ScanColony = nullptr;
        m_ScanShip = nullptr;
    }
    else
        MatchColonyScan(s);
    return true;
}

bool Report::MatchPhaseColonyInventory(String ^s)
{
    if( String::IsNullOrEmpty(s) )
        m_Phase = PHASE_COLONY;
    else
        MatchColonyInventoryScan(s);
    return true;
}

bool Report::MatchPhaseColonyShips(String ^s)
{
   if( String::IsNullOrEmpty(s) )
    {
        m_Phase = PHASE_COLONY;
        m_ScanShip = nullptr;
    }
    else
        MatchColonyShipsScan(s);
    return true;
}

bool Report::MatchPhaseOtherPlanetsShips(String ^s)
{
    if( MatchSectionEnd(s) )
    {
        m_Phase = PHASE_GLOBAL;
        m_ScanColony = nullptr;
        m_ScanShip = nullptr;
    }
    else
        MatchOtherPlanetsShipsScan(s);
    return true;
}

bool Report::MatchPhaseAliensReport(String ^s)
{
    if( String::IsNullOrEmpty(s) )
    {
        m_Phase = PHASE_GLOBAL;
    }
    else
        MatchAliensReport(s);
    return true;
}

bool Report::MatchPhaseOrdersTemplate(String ^s)
{
    if( ! String::IsNullOrEmpty(s) )
    {
        MatchOrdersTemplate(s);
    }
    return true;
}

} // end namespace FHUI