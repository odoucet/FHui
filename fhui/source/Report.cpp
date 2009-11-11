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
{                            //123456789012
    { PHASE_NONE,             "NONE" },
    { PHASE_LOG_USER,         "LOG_USER" },
    { PHASE_LOG_COMBAT,       "LOG_COMBAT" },
    { PHASE_LOG_PRE_DEP,      "LOG_PRE_DEP" },
    { PHASE_LOG_JUMP,         "LOG_JUMP" },
    { PHASE_LOG_PROD,         "LOG_PROD" },
    { PHASE_LOG_POST_ARRIVAL, "LOG_POST_ARR" },
    { PHASE_LOG_STRIKE,       "LOG_STRIKE" },
    { PHASE_LOG_OTHER,        "LOG_OTHER" },
    { PHASE_SPECIES_STATUS,   "SP_STATUS" },
    { PHASE_COLONIES,         "COLONIES" },
    { PHASE_TEMPLATE,         "TEMPLATE" },
};

String^ Report::PhaseToString(PhaseType phase)
{
    if( phase >= PHASE_MAX )
        return "???";
    for( int i = 0; i < PHASE_MAX; ++i )
        if( s_ReportPhaseStrings[i].type == phase )
            return gcnew String(s_ReportPhaseStrings[i].str);

    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid phase type: {0}.", (int)phase) );
}

Report::Report(GameData ^gd, CommandManager^ cm, RegexMatcher ^rm, bool verbose)
    : m_GameData(gd)
    , m_CommandMgr(cm)
    , m_RM(rm)
    , m_Phase(PHASE_NONE)
    , m_Turn(-1)
    , m_Content(gcnew List<String^>)
    , m_Verbose(verbose)
{
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

    while( String ^s = GET_NON_EMPTY_LINE() )
    {
        if( m_RM->Match(s, "^EVENT LOG FOR TURN (\\d+)") )
        {
            if ( 0 == ( m_Turn = m_RM->GetResultInt(0) ) )
            {
                // User content / turn 0
                return true;
            }
            else
            {
                // Regular report / turn N+1
                m_Turn++;
                return true;
            }
        }
        else if( m_RM->Match(s, "^START OF TURN (\\d+)") )
        {
            if ( 0 == ( m_Turn = m_RM->GetResultInt(0) ) )
            {
                // User content / turn 0
                return true;
            }
            else
            {
                // User content / turn N
                return true;
            }
        }
    }

    return false;
}

void Report::Parse(String^ fileName)
{
    m_Input = File::OpenText(fileName);
    String ^line;

    try
    {
        while ( ! String::IsNullOrEmpty( line = GET_NON_EMPTY_LINE() ) )
        {
            ParseInternal( line );
        }
        if( m_Phase == PHASE_LOG_USER )
            GameData::SetParsingUserContent(false);
        m_Phase = PHASE_NONE;
    }
    catch( Exception ^ex )
    {
        throw gcnew FHUIParsingException(
            String::Format("Error occured while parsing report: {0}, line {1}:\r\n{2}\r\nError description:\r\n  {3}",
                fileName,
                m_Content->Count,
                m_Content[m_Content->Count-1],
                ex->Message),
            ex );
    }
    finally
    {
        m_Input->Close();
    }
}

void Report::ExpectedEmptyLine( String^ srcFun, int srcLine )
{
    if ( ! String::IsNullOrEmpty( GetLine( srcFun, srcLine ) ) )
        throw gcnew FHUIParsingException("Empty line was expected");
}

String^ Report::GetNonEmptyLine( String^ srcFun, int srcLine )
{
    String ^s = String::Empty;

    while ( s && ( s->Length == 0 ) )
    {
        s = GetLine( srcFun, srcLine );
    }

    return s;
}

String^ Report::GetMergedLines( String^ srcFun, int srcLine )
{
    String ^merged;

    while ( String ^s = GetLine( srcFun, srcLine ) )
    {
        if( String::IsNullOrEmpty(s) )
            break;

        merged += " " + s;
    }

    return merged;
}

String^ Report::GetLine( String^ srcFun, int srcLine )
{
    if ( m_Verbose && m_Content && m_Content->Count )
    {
        Debug::WriteLine( String::Format("{0,-24} {1,4} {2,-12} \"{3}\"", 
            srcFun->Substring(14),
            srcLine,
            PhaseToString(m_Phase),
            m_Content[m_Content->Count - 1]) );
    }

    return GetLine();
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
    bool recognized = false;

    switch( m_Phase )
    {
    case PHASE_NONE:
        recognized = MatchPhaseNone(s);
        break;

    case PHASE_LOG_USER:
        recognized = MatchPhaseLogUser(s);
        break;

    case PHASE_LOG_COMBAT:
        recognized = MatchPhaseLogCombat(s);
        break;

    case PHASE_LOG_PRE_DEP:
        recognized = MatchPhaseLogPreDep(s);
        break;

    case PHASE_LOG_JUMP:
        recognized = MatchPhaseLogJump(s);
        break;

    case PHASE_LOG_PROD:
        recognized = MatchPhaseLogProd(s);
        break;

    case PHASE_LOG_POST_ARRIVAL:
        recognized = MatchPhaseLogPostArr(s);
        break;

    case PHASE_LOG_STRIKE:
        recognized = MatchPhaseLogStrike(s);
        break;

    case PHASE_LOG_OTHER:
        recognized = MatchPhaseLogOther(s);
        break;

    case PHASE_SPECIES_STATUS:
        recognized = MatchPhaseSpeciesStatus(s);
        break;

    case PHASE_COLONIES:
        recognized = MatchPhaseColonies(s);
        break;

    case PHASE_TEMPLATE:
        recognized = MatchPhaseTemplate(s);
        break;

    default:
        recognized = false;
    }

    if( ! recognized )
    {
        Debug::WriteLine( String::Format(">>> unrecognized line \"{0}\"", s) );
    }

    return recognized;
}

bool Report::MatchPhaseNone(String ^s)
{
    // System scan
    if( MatchSystemScan(s) )
    {
        return true;
    }

    if( s == "SPECIES STATUS" )
    {
        m_Phase = PHASE_SPECIES_STATUS;
        return true;
    }

    // Turn number
    if( s->StartsWith("EVENT") && m_RM->Match(s, "^EVENT LOG FOR TURN (\\d+)" ) )
    {
        m_Turn = m_RM->GetResultInt(0);
        if( m_Turn == 0 )
        {
            m_Phase = PHASE_LOG_USER;
            GameData::SetParsingUserContent(true);
        }
        return true;
    }
    else if( s->StartsWith("START") && m_RM->Match(s, "^START OF TURN (\\d+)" ) )
    {
        m_Turn = m_RM->GetResultInt(0);
        m_Phase = PHASE_LOG_USER;
        GameData::SetParsingUserContent(true);
        return true;
    }

    // Initial home system data
    if( m_RM->Match(s, "^Scan of home star system for SP\\s+([^,;]+):$") )
    {
        m_GameData->AddAlien( m_RM->Results[0] );
        m_GameData->SetSpecies( m_RM->Results[0] );
        if ( String::IsNullOrEmpty( s = GET_LINE() ) )
        {
            s = GET_LINE();
        }
        return MatchSystemScan(s);
    }

    if( s == "Combat orders:" || s == "Combat log:" )
    {
        m_Phase = PHASE_LOG_COMBAT;
        return true;
    }

    if( s == "Pre-departure orders:" )
    {
        m_Phase = PHASE_LOG_PRE_DEP;
        return true;
    }

    if( s == "Jump orders:" )
    {
        m_Phase = PHASE_LOG_JUMP;
        return true;
    }

    if( s == "Production orders:" )
    {
        m_Phase = PHASE_LOG_PROD;
        return true;
    }

    return true;
}

bool Report::MatchPhaseLogUser(String ^s)
{
    // System scan
    if( MatchSystemScan(s) )
    {
        return true;
    }

    // Alien colonies and ships
    if( MatchAliens(s) )
    {
        return true;
    }

    // Estimate
    if( MatchAlienEstimate(s) )
    {
        return true;
    }

    // Intelligence
    if( m_RM->Match(s, "^INFO SP ([^,;]+)\\s*;\\s*") )
    {
        MatchAlienInfo(s, m_GameData->AddAlien(m_RM->Results[0]));
        return true;
    }

    // Received message
    if ( MatchMessageReceived(s) )
    {
        return true;
    }

    // TODO: Parse user log contents
    return false;
}

bool Report::MatchPhaseLogCombat(String ^s)
{
    if( s == "Pre-departure orders:" )
    {
        m_Phase = PHASE_LOG_PRE_DEP;
        return true;
    }

    // TODO: Parse combat log contents
    return true;
}

bool Report::MatchPhaseLogPreDep(String ^s)
{
    if( s == "Jump orders:" )
    {
        m_Phase = PHASE_LOG_JUMP;
        return true;
    }
    // System scan
    if( MatchSystemScan(s) || 
        MatchMessageSent(s) )
    {
        return true;
    }

    // TODO: Parse pre-departure log contents
    return true;
}

bool Report::MatchPhaseLogJump(String ^s)
{
    if( s == "Production orders:" )
    {
        m_Phase = PHASE_LOG_PROD;
        return true;
    }

    if( m_RM->Match(s, "^The star system at\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+was marked as visited\\.$") )
    {
        GameData::GetStarSystem(
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2),
            false)->IsMarkedVisited = true;
        return true;
    }

    if( m_RM->Match(s, "^[A-Za-z0-9]+\\s+([^,;]+) will jump via natural wormhole at\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.$") )
    {
        m_GameData->AddWormholeJump(
            m_RM->Results[0],
            GameData::GetSystemId(
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                m_RM->GetResultInt(3) ) );
        return true;
    }

    // TODO: Parse jumps log contents
    return true;
}


bool Report::MatchPhaseLogProd(String ^s)
{
    if( s == "Post-arrival orders:" )
    {
        m_Phase = PHASE_LOG_POST_ARRIVAL;
        return true;
    }

    if( MatchAlienEstimate(s) )
    {
        return true;
    }
    // TODO: Parse production log contents
    return true;
}

bool Report::MatchPhaseLogPostArr(String ^s)
{
    if( s == "Strike orders:" || s == "Strike log:" )
    {
        m_Phase = PHASE_LOG_STRIKE;
        return true;
    }
    if( s == "Other events:" )
    {
        m_Phase = PHASE_LOG_OTHER;
        return true;
    }
    if( s == "SPECIES STATUS" )
    {
        m_Phase = PHASE_SPECIES_STATUS;
        return true;
    }

    // System scan
    if( MatchSystemScan(s) || 
        MatchMessageSent(s) )
    {
        return true;
    }

    // TODO: Parse post-arrival log contents
    return true;
}

bool Report::MatchPhaseLogStrike(String ^s)
{
    if( s == "Other events:" )
    {
        m_Phase = PHASE_LOG_OTHER;
        return true;
    }

    if( s == "SPECIES STATUS" )
    {
        m_Phase = PHASE_SPECIES_STATUS;
        return true;
    }

    // TODO: Parse strike log contents
    return true;
}

bool Report::MatchPhaseLogOther(String ^s)
{
    if( s == "SPECIES STATUS" )
    {
        m_Phase = PHASE_SPECIES_STATUS;
        return true;
    }
    // Message 
    if( MatchMessageReceived(s) )
    {
        return true;
    }
    
    // Ship misjump
    if( m_RM->Match(s, "^!!! [A-Za-z0-9]+ ([^,;]+) mis-jumped to \\d+ \\d+ \\d+!$") )
    {
        m_GameData->AddMishap(m_RM->Results[0]);
        return true;
    }

    // TODO: Parse other log contents
    return true;
}

bool Report::MatchPhaseSpeciesStatus(String ^s)
{
    if( m_RM->Match(s, "^START OF TURN (\\d+)" ) )
    {
        m_Turn = m_RM->GetResultInt(0);

        if( ! String::IsNullOrEmpty( GET_LINE() ) )
            throw gcnew FHUIParsingException("Species Status: empty line was expected");

        // Species name
        if( m_RM->Match(GET_LINE(), "^Species name: ([^,;]+)") )
        {
            m_GameData->SetSpecies( m_RM->Results[0] );
        }
        else
        {
            throw gcnew FHUIParsingException("Species Status: unexpected line");
        }

        if( m_RM->Match(GET_LINE(), "^Government name:\\s+(.+)$") )
        {
            GameData::Player->GovName = m_RM->Results[0];
        }
        else
        {
            throw gcnew FHUIParsingException("Species Status: unexpected line");
        }

        if( m_RM->Match(GET_LINE(), "^Government type:\\s+(.+)$") )
        {
            GameData::Player->GovType = m_RM->Results[0];
        }
        else
        {
            throw gcnew FHUIParsingException("Species Status: unexpected line");
        }
        
        if( ! String::IsNullOrEmpty( GET_LINE() ) )
            throw gcnew FHUIParsingException("Species Status: empty line was expected");

        // Tech levels
        if( ! MatchTechLevels( GET_LINE() ) )
        {
            throw gcnew FHUIParsingException("Species Status: tech status was expected");
        }

        if( ! String::IsNullOrEmpty( GET_LINE() ) )
            throw gcnew FHUIParsingException("Species Status: empty line was expected");

        // Atmospheric requirements
        if( ! MatchAtmReq( GET_LINE() ) )
        {
            throw gcnew FHUIParsingException("Species Status: atmospheric requirements were expected");
        }

        if( ! String::IsNullOrEmpty( GET_LINE() ) )
            throw gcnew FHUIParsingException("Species Status: empty line was expected");
      
        // Fleet maintenance
        if( m_RM->Match( GET_LINE(), "^Fleet maintenance cost = (\\d+) \\((\\d+)\\.(\\d+)% of total production\\)") )
        {
            m_GameData->SetFleetCost(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1)*100 + m_RM->GetResultInt(2) );
        }
        else
        {
            throw gcnew FHUIParsingException("Species Status: unexpected line");
        }

        s = GET_NON_EMPTY_LINE();

        // Species met, aliens and enemies
        if( MatchSpeciesMet(s) )
        {
            s = GET_LINE();
        }
        if( MatchAllies(s) )
        {
            s = GET_LINE();
        }
        if( MatchEnemies(s) )
        {
            s = GET_LINE();
        }

        // Leftover EU
        if( m_RM->Match(s, "^Economic units = (\\d+)") )
        {
            m_GameData->SetTurnStartEU( m_RM->GetResultInt(0) );
            s = GET_NON_EMPTY_LINE();
        }  

        if( MatchSectionEnd(s) )
        {
            m_Phase = PHASE_COLONIES;
            return true;
        }
        else
        {
            throw gcnew FHUIParsingException("Species Status: unexpected line");
        }
    }
    return false;
}

bool Report::MatchPhaseColonies(String ^s)
{
    while( MatchColonyInfo(s) )
    {
        s = GET_NON_EMPTY_LINE();
    }

    if( MatchOtherPlanetsShips(s) )
    {
        s = GET_NON_EMPTY_LINE();
    }

    while( MatchAliens(s) || MatchSectionEnd(s) )
    {
        s = GET_NON_EMPTY_LINE();
    }

    if( s->StartsWith( "ORDER SECTION" ) &&
        (s = GET_LINE())->StartsWith("them, and submit") )
    {
        m_Phase = PHASE_TEMPLATE;
        return true;
    }
    return false;
}

bool Report::MatchPhaseTemplate(String ^s)
{
    while( MatchTemplateEntry(s) )
    {
        s = GET_NON_EMPTY_LINE();

        if( s == nullptr )
            break;
    }
    m_Phase = PHASE_NONE;
    return true;
}

} // end namespace FHUI