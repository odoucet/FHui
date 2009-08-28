#include "StdAfx.h"
#include "Report.h"
#include "RegexMatcher.h"
#include "ReportParser.h"
#include "CommandManager.h"

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
    , m_RepFiles(gcnew SortedList<int, String^>)
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

    for each( FileInfo ^f in dir->GetFiles("*"))
    {   // First check each file if it is a report and find out for which turn.
        // Then reports will be loaded in chronological order.
        int turn = VerifyReport(f->FullName);
        if( turn != -1 )
        {
            m_RepFiles[turn] = f->FullName;
        }
    }

    int prevTurn = -1;
    for each( int currTurn in m_RepFiles->Keys )
    {
        m_GameData->SelectTurn(currTurn);
        m_CommandMgr->SelectTurn(currTurn);

        if ( prevTurn == -1 )
        {
            // first turn on the list, parse galaxy data
            LoadGalaxy();
        }
        else
        {
            m_GameData->InitTurnFrom(prevTurn);
        }

        LoadReport( m_RepFiles[currTurn] );
        m_GameData->Update();
        m_CommandMgr->LoadCommands();

        prevTurn = currTurn;
    }
}

int ReportParser::VerifyReport(String ^fileName)
{
    Report ^report = gcnew Report(nullptr, nullptr, m_RM); // turn scan mode

    StreamReader ^sr = File::OpenText(fileName);
    String ^line;

    try
    {
        while( (line = sr->ReadLine()) != nullptr ) 
        {
            if( false == report->Parse(line) )
                break;
            if( report->GetTurn() != -1 )
                return report->GetTurn();
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
    return -1;
}

void ReportParser::LoadReport(String ^fileName)
{
    Report ^report = gcnew Report(m_GameData, m_CommandMgr, m_RM);

    StreamReader ^sr = File::OpenText(fileName);
    String ^line;

    try
    {
        while( (line = sr->ReadLine()) != nullptr ) 
        {
            if( false == report->Parse(line) )
                break;
        }

        if( report->GetTurn() > 0 &&
            !report->IsValid() )
            throw gcnew FHUIParsingException("File is not a valid FH report.");

        m_Reports[report->GetTurn()] = report->GetContent();
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

} // end namespace FHUI

