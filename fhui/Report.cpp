#include "StdAfx.h"
#include "Report.h"

using namespace System::Text::RegularExpressions;

Report::Report(GameData ^gd)
    : m_GameData(gd)
    , m_Phase(PHASE_GLOBAL)
    , m_Turn(0)
    , m_bParsingAggregate(false)
    , m_StringAggregate(nullptr)
    , m_ScanX(0)
    , m_ScanY(0)
    , m_ScanZ(0)
    , m_ScanHasPlanets(false)
    , m_ScanHome(nullptr)
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

    if( m_Phase == PHASE_FILE_DETECT )
    {
        if( String::IsNullOrEmpty(s) )
            return true;

        if( Regex("^\\s*EVENT LOG FOR TURN \\d+").Match(s)->Success )
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
            m_StringAggregate = String::Concat(m_StringAggregate, s);
            return true;
        }
    }

    switch( m_Phase )
    {
    case PHASE_GLOBAL:
        // Turn number
        if( MatchWithOutput(s, "^\\s*EVENT LOG FOR TURN (\\d+)") )
        {
            m_Turn = GetMatchResultInt(0);
            break;
        }
        if( MatchWithOutput(s, "START OF TURN (\\d+)") )
        {
            m_Turn = GetMatchResultInt(0);
            break;
        }

        // Species name
        if( MatchWithOutput(s, "Species name: ([^,;]+)") )
            m_GameData->SetSpecies( GetMatchResult(0) );
        // Atmospheric requirements
        else if( MatchWithOutput(s, "Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
            m_GameData->SetAtmosphereReq(
                GasFromString( GetMatchResult(2) ), // required gas
                GetMatchResultInt(0),               // min level
                GetMatchResultInt(1) );             // max level
        else if( MatchAggregateList(s, "Neutral Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmosphereNeutral(
                    GasFromString(GetMatchResult(i)) );
        }
        else if( MatchAggregateList(s, "Poisonous Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmospherePoisonous(
                    GasFromString(GetMatchResult(i)) );
        }
        // Fleet maintenance
        else if( MatchWithOutput(s, "Fleet maintenance cost = (\\d+) \\((\\d+\\.?\\d+)% of total production\\)") )
            m_GameData->SetFleetCost(m_Turn, GetMatchResultInt(0), GetMatchResultFloat(1));
        // Species...
        else if( Regex("^Species met:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_MET, s);
        else if( MatchWithOutput(s, "Scan of home star system for SP\\s+([^,;]+):$") )
            m_ScanHome = m_GameData->AddAlien(m_Turn, GetMatchResult(0));
        else if( Regex("^Allies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ALLIES, s);
        else if( Regex("^Enemies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ENEMIES, s);
        // Tech levels
        else if( Regex("^Tech Levels:").Match(s)->Success )
            m_Phase = PHASE_TECH_LEVELS;
        // System scan
        else if( MatchSystemScanStart(s) )
        {}
        else if( Regex("^Combat orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_COMBAT;
        // Parsing ends here
        else if( Regex("^ORDER SECTION.").Match(s)->Success )
            m_Phase = PHASE_ORDERS_TEMPLATE;
        break;

    case PHASE_SPECIES_MET:
        if( MatchAggregateList(FinishLineAggregate(), "Species met:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->AddAlien(m_Turn, GetMatchResult(i));
            break;
        }
        return false;

    case PHASE_SPECIES_ALLIES:
        if( MatchAggregateList(FinishLineAggregate(), "Allies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAlienRelation(m_Turn, GetMatchResult(i), SP_ALLY);
            break;
        }
        return false;

    case PHASE_SPECIES_ENEMIES:
        if( MatchAggregateList(FinishLineAggregate(), "Enemies:", "SP\\s+([^,;]+)") )
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
        break;

    case PHASE_ORDERS_JUMP:
        if( Regex("^Production orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_PROD;
        break;

    case PHASE_ORDERS_PROD:
        if( Regex("^Post-arrival orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_POST_ARRIVAL;
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
        if( MatchTech(s, "Mining", TECH_MI) ||
            MatchTech(s, "Manufacturing", TECH_MA) ||
            MatchTech(s, "Military", TECH_ML) ||
            MatchTech(s, "Gravitics", TECH_GV) ||
            MatchTech(s, "Life Support", TECH_LS) )
            break;
        else if( MatchTech(s, "Biology", TECH_BI) )
            m_Phase = PHASE_GLOBAL;
        else
            return false;
        break;

    case PHASE_SYSTEM_SCAN:
        if( Regex("^\\s*$").Match(s)->Success )
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
    }

    return true;
}

bool Report::MatchSystemScanStart(String ^s)
{
    if( MatchWithOutput(s, "Coordinates:\\s+[Xx]\\s+=\\s+(\\d+)\\s+[Yy]\\s+=\\s+(\\d+)\\s+[Zz]\\s+=\\s+(\\d+)") )
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
    if( MatchWithOutput(s, "^\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+)\\s+") )
    {
        Planet ^planet = gcnew Planet(
            GetMatchResultInt(1),
            GetMatchResultFloat(2),
            GetMatchResultInt(3),
            GetMatchResultInt(4),
            GetMatchResultFloat(5) );
        int plNum = GetMatchResultInt(0);

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
        else if( MatchWithOutput(s, "[x-?]+\\s+") )
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

void Report::StartLineAggregate(PhaseType phase, String ^s)
{
    m_bParsingAggregate = true;
    m_StringAggregate = s;
    m_Phase = phase;
}

String^ Report::FinishLineAggregate()
{
    m_bParsingAggregate = false;
    String ^ret = m_StringAggregate;
    m_StringAggregate = nullptr;
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
