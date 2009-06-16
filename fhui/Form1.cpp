#include "StdAfx.h"
#include "Form1.h"

#include "Report.h"
#include "svn_rev.h"

using namespace System::IO;
using namespace System::Text::RegularExpressions;

#define FHUI_CONTACT_EMAILS "jdukat+fhui@gmail.com"
#define FHUI_CONTACT_WIKI   "http://www.cetnerowski.com/farhorizons/pmwiki/pmwiki.php/FHUI/FHUI"

#define FHUI_BUILD_INFO_APPENDIX ""
#define FHUI_BUILD_INFO() String::Format("{0}{1}{2}", FHUI_REVISION_NUMBER, FHUI_BUILD_INFO_APPENDIX, FHUI_REVISION_MODIFIED ? " (modified)" : "")


namespace fhui 
{

void Form1::LoadGameData()
{
    try
    {
        FillAboutBox();
        InitData();
        LoadGalaxy();
        LoadReports();
        LoadCommands();

        SetupSystems();
        SetupPlanets();

        this->Text = String::Format("[SP {0}] Far Horizons User Interface, build {1}",
            m_GameData->GetSpeciesName(), FHUI_BUILD_INFO() );
    }
    catch( SystemException ^e )
    {
        Summary->Text = "Failed loading game data.";
        RepText->Text = String::Format(
            "Fatal Exception !\r\n"
            "---------------------------------------------------------------------------\r\n"
            "Please send this message and problem description to:\r\n"
            "   {0}.\r\n"
            "---------------------------------------------------------------------------\r\n"
            "FHUI rev.: {1}\r\n"
            "Message  : {2}\r\n"
            "Type     : {3}\r\n"
            "---------------------------------------------------------------------------\r\n"
            "{4}\r\n"
            "---------------------------------------------------------------------------\r\n",
            FHUI_CONTACT_EMAILS,
            FHUI_BUILD_INFO(),
            e->Message,
            e->GetType()->ToString(),
            e->StackTrace );
    }
}

void Form1::FillAboutBox()
{
    String ^changeLog = nullptr;

    try {
        StreamReader ^sr = File::OpenText("changelog.txt");
        changeLog = sr->ReadToEnd();
    }
    catch( Exception^ )
    {
        changeLog = "** Change log file not found or unreadable **";
    }

    TextAbout->Text = String::Format(
        "---------------------------------------------------------------------------\r\n"
        "   Far Horizons User Interface\r\n"
        "     Revision: {0}\r\n"
        "     Contact : {1}\r\n"
        "     WIKI    : {2}\r\n"
        "     License : Freeware\r\n"
        "---------------------------------------------------------------------------\r\n"
        "{3}\r\n"
        "---------------------------------------------------------------------------\r\n",
            FHUI_BUILD_INFO(),
            FHUI_CONTACT_EMAILS,
            FHUI_CONTACT_WIKI,
            changeLog );
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

    if( m_Reports->Count > 0 )
    {
        m_GameData->UpdatePlanets();

        // Display summary
        Summary->Text = m_GameData->GetSummary();

        // Setup combo box with list of loaded reports
        array<String^> ^arr = gcnew array<String^>(m_Reports->Count);
        for( int n = 0, i = m_Reports->Count - 1; i >= 0; --i, ++n )
        {
            arr[n] = String::Format("Turn {0}", m_Reports->GetKey(i));
        }
        RepTurnNr->DataSource = arr;
    }
    else
    {
        RepText->Text = "No report successfully loaded.";
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

void Form1::DisplayReport()
{
    String ^sel = RepTurnNr->SelectedItem->ToString();
    int key = int::Parse(sel->Substring(5));    // Skip 'Turn '

    Report ^report = safe_cast<Report^>(m_Reports[key]);
    RepText->Text = report->GetContent();
}

void Form1::LoadCommands()
{
}

void Form1::InitData()
{
    m_GameData = gcnew GameData;

    RepMode->SelectedIndex = 1;
}

String^ Form1::SystemsGetRowTooltip(DataGridViewRow ^row)
{
    int x = int::Parse(row->Cells[ SystemsGrid->Columns["X"]->Index ]->Value->ToString());
    int y = int::Parse(row->Cells[ SystemsGrid->Columns["Y"]->Index ]->Value->ToString());
    int z = int::Parse(row->Cells[ SystemsGrid->Columns["Z"]->Index ]->Value->ToString());
    StarSystem ^system = m_GameData->GetStarSystem(x, y, z);
    return system->GenerateScan();
}

void Form1::SetupSystems()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("X", int::typeid );
    dataTable->Columns->Add("Y", int::typeid );
    dataTable->Columns->Add("Z", int::typeid );
    dataTable->Columns->Add("Type", String::typeid );
    dataTable->Columns->Add("Planets", String::typeid );
    dataTable->Columns->Add("Distance", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->m_TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double mishap = system->CalcMishap(sp->m_HomeSystem, gv, 0);

        DataRow^ row = dataTable->NewRow();
        row["X"] = system->X;
        row["Y"] = system->Y;
        row["Z"] = system->Z;
        row["Type"] = system->Type;
        row["Planets"] = system->NumPlanets;
        row["Distance"] = system->CalcDistance(sp->m_HomeSystem);
        row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row["Scan"] = system->ScanTurn;

        dataTable->Rows->Add(row);
    }
    SystemsGrid->DataSource = dataTable;

    // Formatting
    SystemsGrid->Columns["Distance"]->DefaultCellStyle->Format = "F2";

    // Some columns are not sortable... yet
    SystemsGrid->Columns["Scan"]->SortMode = DataGridViewColumnSortMode::NotSortable;
    SystemsGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    SystemsGrid->Sort( SystemsGrid->Columns["Distance"], ListSortDirection::Ascending );
}

void Form1::SetupPlanets()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("Name", String::typeid );
    dataTable->Columns->Add("Coords", String::typeid );
    dataTable->Columns->Add("TC", int::typeid );
    dataTable->Columns->Add("PC", int::typeid );
    dataTable->Columns->Add("MD", double::typeid );
    dataTable->Columns->Add("LSN", int::typeid );
    dataTable->Columns->Add("Distance", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->m_TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double distance = system->CalcDistance(sp->m_HomeSystem);
        double mishap = system->CalcMishap(sp->m_HomeSystem, gv, 0);

        for each( Planet ^planet in system->m_Planets )
        {
            DataRow^ row = dataTable->NewRow();
            row["Name"] = planet->m_Name;
            row["Coords"] = String::Format("{0} {1} {2} {3}",
                system->X, system->Y, system->Z, planet->m_Number);
            row["TC"] = planet->m_TempClass;
            row["PC"] = planet->m_PressClass;
            row["MD"] = planet->m_MiningDiff;
            row["LSN"] = planet->m_LSN;
            row["Distance"] = distance;
            row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
            row["Scan"] = system->ScanTurn;

            dataTable->Rows->Add(row);
        }
    }
    PlanetsGrid->DataSource = dataTable;

    // Formatting
    PlanetsGrid->Columns["MD"]->DefaultCellStyle->Format = "F2";
    PlanetsGrid->Columns["Distance"]->DefaultCellStyle->Format = "F2";

    // Some columns are not sortable... yet
    PlanetsGrid->Columns["Scan"]->SortMode = DataGridViewColumnSortMode::NotSortable;
    PlanetsGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    PlanetsGrid->Sort( PlanetsGrid->Columns["LSN"], ListSortDirection::Ascending );
}

}
