#include "StdAfx.h"
#include "Form1.h"

#include "Report.h"

using namespace System::IO;
using namespace System::Text::RegularExpressions;

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

        SetupSystems();
        SetupPlanets();
    }
    catch( SystemException ^e )
    {
        Summary->Text = "Failed loading game data.";
        RepText->Text = e->ToString();
    }
}

void Form1::LoadGalaxy()
{
    StreamReader ^sr = File::OpenText("galaxy_list.txt");
    String ^line;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        Match ^m = Regex("^x\\s+=\\s+(\\d+)\\s*y\\s+=\\s+(\\d+)\\s*z\\s+=\\s+(\\d+)\\s*stellar type =\\s+(\\w+)\\s*$").Match(line);
        if( m->Success )
        {
            int x = int::Parse(m->Groups[1]->ToString());
            int y = int::Parse(m->Groups[2]->ToString());
            int z = int::Parse(m->Groups[3]->ToString());
            String ^type = m->Groups[4]->ToString();

            m_GameData->AddStarSystem(x, y, z, type);
        }
    }
    sr->Close();
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
    String ^line;
    while( (line = sr->ReadLine()) != nullptr ) 
    {
        if( false == report->Parse(line) )
            break;
    }
    sr->Close();

    if( report->IsValid() )
        m_Reports->Add(report->GetTurn(), report);
}

void Form1::LoadCommands()
{
}

void Form1::InitData()
{
    m_GameData = gcnew GameData;

    RepMode->SelectedIndex = 1;
}

void Form1::SetupSystems()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("X", int::typeid );
    dataTable->Columns->Add("Y", int::typeid );
    dataTable->Columns->Add("Z", int::typeid );
    dataTable->Columns->Add("Type", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );
    dataTable->Columns->Add("Planets", String::typeid );
    dataTable->Columns->Add("Distance", double::typeid );
    dataTable->Columns->Add("Mishap %", double::typeid );

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        DataRow^ row = dataTable->NewRow();
        row["X"] = system->X;
        row["Y"] = system->Y;
        row["Z"] = system->Z;
        row["Type"] = system->Type;
        row["Scan"] = system->ScanTurn;
        row["Planets"] = system->NumPlanets;
        row["Distance"] = 0.0; // calculate it
        row["Mishap %"] = 0.0; // calculate it
        dataTable->Rows->Add(row);
    }
    SystemsGrid->DataSource = dataTable;
}

void Form1::SetupPlanets()
{
}

}
