#include "StdAfx.h"
#include "Report.h"
#include "ReportParser.h"

using namespace System::IO;

namespace FHUI
{

ReportParser::ReportParser(GameData^ gd, CommandManager^ cm, String^ galaxyPath, String^ reportPath)
    : m_GameData(gd)
    , m_CommandMgr(cm)
    , m_GalaxyPath(galaxyPath)
    , m_ReportPath(reportPath)
    , m_RM(gcnew RegexMatcher)
    , m_Reports(gcnew SortedList<int, String^>)
    , m_RepFiles(gcnew SortedList<String^, int>)
{
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
        int turn = VerifyReport(f->FullName);
        if( turn != -1 )
        {
            m_RepFiles->Add(f->FullName, turn);
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

        try
        {
            if( currTurn > 0 )
            {
                m_CommandMgr->LoadCommands();
            }
        }
        catch( FHUIParsingException ^ex )
        {
            throw gcnew FHUIParsingException(
                String::Format("Error occured while parsing FHUI orders: turn {0}\r\nError description:\r\n  {1}",
                    currTurn,
                    ex->Message),
                ex );
        }
    }

    Debug::WriteLine( String::Format("Regexp stats: {0,6} matches, {1,6} misses", 
        m_RM->HitCount, m_RM->MissCount ) );
}

int ReportParser::VerifyReport(String ^fileName)
{
    Report ^report = gcnew Report(nullptr, nullptr, m_RM, false); // turn scan mode

    StreamReader ^sr = File::OpenText(fileName);
    String ^line;

    try
    {
        while( (line = sr->ReadLine()) != nullptr ) 
        {
            if( false == report->Parse(line) )
                break;
        }
    }
    catch( Exception ^ex )
    {
        throw gcnew FHUIParsingException(
            String::Format("Error occured while parsing report: {0}, line {1}:\r\n{2}\r\nError description:\r\n  {3}",
                fileName,
                report->GetLineCount(),
                line,
                ex->Message),
            ex );
    }
    finally
    {
        sr->Close();
    }

    return report->GetTurn();
}

void ReportParser::LoadReports( int turn )
{
    String ^content, ^line, ^fileName;

    bool validReportFound = false;

    for( int i = 0; i < m_RepFiles->Count; i++ )
    {
        if ( m_RepFiles->Values[i] != turn )
        {
            continue;
        }

        Report ^report = gcnew Report(m_GameData, m_CommandMgr, m_RM, Verbose);
        StreamReader ^sr;

        try
        {
            fileName = m_RepFiles->Keys[i];
            sr = File::OpenText( fileName );

            Debug::WriteLineIf( Verbose, fileName );

            while( (line = sr->ReadLine()) != nullptr ) 
            {
                if( false == report->Parse(line) )
                    break;
            }

            content +=
                ";===========================================================================================\n"
                "; Report File: " + fileName + "\n";
            content += report->GetContent() + "\n";
            if( report->IsValid() )
                validReportFound = true;
        }
        catch( Exception ^ex )
        {
            throw gcnew FHUIParsingException(
                String::Format("Error occured while parsing report: {0}, line {1}:\r\n{2}\r\nError description:\r\n  {3}",
                    fileName,
                    report->GetLineCount(),
                    line,
                    ex->Message),
                ex );
        }
        finally
        {
            sr->Close();
        }
    }

    if( !validReportFound && turn > 0 )
    {
        throw gcnew FHUIParsingException("No complete report found for turn " + turn.ToString() );
    }

    m_Reports[turn] = content;
}

} // end namespace FHUI

