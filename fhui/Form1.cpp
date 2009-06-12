#include "StdAfx.h"
#include "Form1.h"

#include "Report.h"

using namespace System::IO;

namespace fhui 
{

void Form1::LoadGameData()
{
    try
    {
        InitData();
        LoadGalaxy();
        LoadReports();
        LoadCommands();
    }
    catch( SystemException ^e )
    {
        Summary->Text = "Failed loading game data.";
        RepText->Text = e->ToString();
    }
}

void Form1::LoadGalaxy()
{
}

void Form1::LoadReports()
{
    DirectoryInfo ^dir = gcnew DirectoryInfo("reports");
    for each( FileInfo ^f in dir->GetFiles("*"))
    {
        LoadReport( f->FullName );
    }

    Report ^report = dynamic_cast<Report^>(m_Reports->GetByIndex(0));
    if( report )
    {
        RepText->Text = report->GetContent();
        Summary->Text = report->GetSummary();
    }
}

void Form1::LoadReport(String ^fileName)
{
    Report ^report = gcnew Report(m_GameData);

    StreamReader ^sr = File::OpenText(fileName);
    if( sr == nullptr )
        return;

    String ^line;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        if( false == report->Parse(line) )
            break;
    }
    sr->Close();

    if( report->IsValid() )
        m_Reports->Add(report->GetTour(), report);
}

void Form1::LoadCommands()
{
}

void Form1::InitData()
{
    m_GameData = gcnew GameData;

    RepMode->SelectedIndex = 1;
}

}
