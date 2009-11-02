#include "StdAfx.h"
#include "Report.h"
#include "ReportParser.h"

using namespace System::IO;

namespace FHUI
{

ReportParser::ReportParser(GameData^ gd, CommandManager^ cm, RegexMatcher ^rm, String^ galaxyPath, String^ reportPath)
    : m_GameData(gd)
    , m_CommandMgr(cm)
    , m_GalaxyPath(galaxyPath)
    , m_ReportPath(reportPath)
    , m_RM(rm)
    , m_Reports(gcnew SortedList<int, Report^>)
    , m_RepFiles(gcnew SortedList<String^, int>)
{
    if( rm == nullptr)
        m_RM = gcnew RegexMatcher;
}

void ReportParser::LoadGalaxy()
{
    StreamReader ^sr = File::OpenText(m_GalaxyPath);
    String ^line;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        if( String::IsNullOrEmpty(line) )
            continue;

        Match ^m = Regex("^x\\s+=\\s+(\\d+)\\s*y\\s+=\\s+(\\d+)\\s*z\\s+=\\s+(\\d+)\\s*stellar type =\\s+(\\w+)\\s*(\\w*)$").Match(line);
        if( m->Success )
        {
            int x = int::Parse(m->Groups[1]->ToString());
            int y = int::Parse(m->Groups[2]->ToString());
            int z = int::Parse(m->Groups[3]->ToString());
            String ^type = m->Groups[4]->ToString();
            String ^comment = m->Groups[5]->ToString();

            m_GameData->AddStarSystem(x, y, z, type, comment);
        }
        else
        {
            m = Regex("^The galaxy has a radius of (\\d+) parsecs.").Match(line);
            if( m->Success )
            {
                GameData::GalaxyDiameter = 2 * int::Parse(m->Groups[1]->ToString());
                break;
            }
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unrecognized entry in galaxy list: {0}", line) );
        }
    }
    sr->Close();
}

void ReportParser::ScanReports()
{
    DirectoryInfo ^dir = gcnew DirectoryInfo(m_ReportPath);

    int minTurn = int::MaxValue;
    int maxTurn = 0;

    for each( FileInfo ^f in dir->GetFiles() )
    {   // First check each file if it is a report and find out for which turn.
        // Then reports will be loaded in chronological order.

        Report ^report = gcnew Report(nullptr, nullptr, m_RM, false);
        if( report->Verify(f->FullName) )
        {
            int turn = report->GetTurn();
            m_RepFiles->Add( f->FullName, turn );
            minTurn = Math::Min(minTurn, turn);
            maxTurn = Math::Max(maxTurn, turn);
        }
    }

    for (int currTurn = minTurn; currTurn <= maxTurn; currTurn ++)
    {
        m_GameData->SelectTurn(currTurn);
        m_CommandMgr->SelectTurn(currTurn);

        if ( currTurn == minTurn )
        {   // First turn on the list, parse galaxy data
            LoadGalaxy();
        }

        LoadReports( currTurn );
        m_GameData->Update();

        if( Stats )
        {
            m_GameData->PrintStats( true );
        }

        if( currTurn > 0 )
        {
            m_CommandMgr->LoadCommands();
        }
    }
}

void ReportParser::LoadReports( int turn )
{
    Report ^report = gcnew Report(m_GameData, m_CommandMgr, m_RM, Verbose);

    Debug::WriteLineIf( Verbose || Stats, String::Format( "=== TURN {0}", turn ) );

    for( int i = 0; i < m_RepFiles->Count; i++ )
    {
        if ( m_RepFiles->Values[i] != turn )
        {
            continue;
        }

        Debug::WriteLineIf( Verbose || Stats, m_RepFiles->Keys[i] );

        report->Parse( m_RepFiles->Keys[i] );
    }
    m_Reports[turn] = report;
}

} // end namespace FHUI

