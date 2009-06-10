#include "StdAfx.h"
#include "Report.h"

using namespace System::Text::RegularExpressions;

Report::Report(GameData ^gd)
    : m_GameData(gd)
    , m_Phase(PHASE_GLOBAL)
    , m_Tour(0)
    , m_bParsingAggregate(false)
    , m_StringAggregate(nullptr)
{
    m_Content           = gcnew String("");
    m_TmpRegexResult    = gcnew array<String^>(1);
}

bool Report::IsValid()
{
    return
        m_GameData->GetSpecies() != nullptr &&
        m_GameData->GetAtmosphere()->m_ReqGas != nullptr &&
        m_Phase == PHASE_ORDERS_TEMPLATE;
}

String^ Report::GetSummary()
{
    if( !IsValid() )
        return "ERROR";

    return String::Format(
        "{1}"
        "--------------------------------\r\n"
        "{2}"
        "{3}"
        "--------------------------------\r\n"
        "{4}",
        "--------------------------------\r\n",
        GetSpeciesSummary(),
        GetAllTechsSummary(),
        GetFleetSummary(),
        GetAliensSummary() );
}

String^ Report::GetSpeciesSummary()
{
    return String::Format(
        "Species: {0}\r\n"
        "T:{1} P:{2} A:{3} {4}-{5}%\r\n",
        m_GameData->GetSpecies(),
        "?",    // TODO: temp class
        "?",    // TODO: press class
        m_GameData->GetAtmosphere()->m_ReqGas,
        m_GameData->GetAtmosphere()->m_ReqMin,
        m_GameData->GetAtmosphere()->m_ReqMax );
}

String^ Report::GetAllTechsSummary()
{
    return String::Format(
        "  MI={0} | MA={1}\r\n"
        "  ML={2} | GV={3}\r\n"
        "  LS={4} | BI={5}\r\n",
        GetTechSummary(TECH_MI),
        GetTechSummary(TECH_MA),
        GetTechSummary(TECH_ML),
        GetTechSummary(TECH_GV),
        GetTechSummary(TECH_LS),
        GetTechSummary(TECH_BI) );
}

String^ Report::GetFleetSummary()
{
    int cost;
    float costPerc;
    m_GameData->GetFleetCost(cost, costPerc);

    return String::Format("Fleet maint. = {0} ({1}%)\r\n",
        cost, costPerc);
}

String^ Report::GetTechSummary(TechType tech)
{
    int lev, levTeach;
    m_GameData->GetTechLevel(tech, lev, levTeach);

    if( lev != levTeach )
    {
        return String::Format("{0,3}/{1,3}", lev, levTeach);
    }

    return String::Format("{0,3}    ", lev);
}

String^ Report::GetAliensSummary()
{
    String ^ret = gcnew String("");

    SortedList ^aliens = m_GameData->GetAliensAll();

    for( int i = 0; i < aliens->Count; ++i )
    {
        Alien ^alien = safe_cast<Alien^>(aliens->GetByIndex(i));
        if( alien->m_Relation == SP_ALLY )
        {
            ret = String::Format("{0}Ally: SP {0}\r\n",
                ret, alien->m_Name);
        }
    }

    for( int i = 0; i < aliens->Count; ++i )
    {
        Alien ^alien = safe_cast<Alien^>(aliens->GetByIndex(i));
        if( alien->m_Relation == SP_ENEMY )
        {
            ret = String::Format("{0}Enemy: SP {0}\r\n",
                ret, alien->m_Name);
        }
    }

    if( ret->Length == 0 )
        ret = "No Allies/Enemies";

    return ret;
}

// ------------------------------------------------------------
// Parser

bool Report::Parse(const std::string &line)
{
    m_Content = String::Concat(m_Content,
        gcnew String((line + "\r\n").c_str()) );

    String ^s = gcnew String(line.c_str());

    if( m_Phase == PHASE_FILE_DETECT )
    {
        if( line == "" )
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
        if( line != "" )
        {
            m_StringAggregate = String::Concat(m_StringAggregate, s);
            return true;
        }
    }

    switch( m_Phase )
    {
    case PHASE_GLOBAL:
        // Turn number
        if( MatchWithOutput(s, "START OF TURN (\\d+)") )
        {
            m_Tour = GetMatchResultInt(0);
            break;
        }

        // Species name
        if( MatchWithOutput(s, "Species name: ([^,;]+)") )
            m_GameData->SetSpecies( GetMatchResult(0) );
        // Atmospheric requirements
        else if( MatchWithOutput(s, "Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
            m_GameData->SetAtmosphereReq(
                GetMatchResult(2),      // required gas
                GetMatchResultInt(0),   // min level
                GetMatchResultInt(1) ); // max level
        else if( MatchAggregateList(s, "Neutral Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmosphereNeutral(GetMatchResult(i));
        }
        else if( MatchAggregateList(s, "Poisonous Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAtmospherePoisonous(GetMatchResult(i));
        }
        // Fleet maintenance
        else if( MatchWithOutput(s, "Fleet maintenance cost = (\\d+) \\((\\d+\\.?\\d+)% of total production\\)") )
            m_GameData->SetFleetCost(m_Tour, GetMatchResultInt(0), GetMatchResultFloat(1));
        // Species...
        else if( Regex("^Species met:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_MET, s);
        else if( Regex("^Allies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ALLIES, s);
        else if( Regex("^Enemies:").Match(s)->Success )
            StartLineAggregate(PHASE_SPECIES_ENEMIES, s);
        else if( Regex("^Combat orders:").Match(s)->Success )
            m_Phase = PHASE_ORDERS_COMBAT;
        // Tech levels
        else if( Regex("^Tech Levels:").Match(s)->Success )
            m_Phase = PHASE_TECH_LEVELS;
        // Parsing ends here
        else if( Regex("^ORDER SECTION.").Match(s)->Success )
            m_Phase = PHASE_ORDERS_TEMPLATE;
        break;

    case PHASE_SPECIES_MET:
        if( MatchAggregateList(FinishLineAggregate(), "Species met:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->AddAlien(m_Tour, GetMatchResult(i));
            break;
        }
        return false;

    case PHASE_SPECIES_ALLIES:
        if( MatchAggregateList(FinishLineAggregate(), "Allies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAlienRelation(m_Tour, GetMatchResult(i), SP_ALLY);
            break;
        }
        return false;

    case PHASE_SPECIES_ENEMIES:
        if( MatchAggregateList(FinishLineAggregate(), "Enemies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_TmpRegexResult->Length; ++i )
                m_GameData->SetAlienRelation(m_Tour, GetMatchResult(i), SP_ENEMY);
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
    }

    return true;
}

bool Report::MatchTech(String ^s, String ^techName, TechType tech)
{
    String ^e2 = String::Format("{0} = (\\d+)/(\\d+)", techName);
    String ^e1 = String::Format("{0} = (\\d+)", techName);
    if( MatchWithOutput(s, e2) )
    {
        m_GameData->SetTechLevel(m_Tour, tech, GetMatchResultInt(0), GetMatchResultInt(1));
        return true;
    }
    if( MatchWithOutput(s, e1) )
    {
        m_GameData->SetTechLevel(m_Tour, tech, GetMatchResultInt(0), GetMatchResultInt(0));
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

bool Report::MatchWithOutput(String ^s, String ^exp)
{
    Array::Clear(m_TmpRegexResult, 0, m_TmpRegexResult->Length);

    Match ^m = Regex(exp).Match(s);
    if( m->Success )
    {
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
