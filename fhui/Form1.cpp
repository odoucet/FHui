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


////////////////////////////////////////////////////////////////

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
        SetupColonies();
        SetupShips();
        SetupAliens();
        SetupMap();

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            m_GameData->GetSpeciesName(),
            m_GameData->GetLastTurn(),
            FHUI_BUILD_INFO() );
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed loading game data.";
        ShowException(e);
    }
}

void Form1::ShowException(Exception ^e)
{
    RepText->Text = String::Format(
        "Fatal Exception !\r\n"
        "---------------------------------------------------------------------------\r\n"
        "Please send this message and problem description to:\r\n"
        "   {0}.\r\n"
        "---------------------------------------------------------------------------\r\n"
        "FHUI rev.: {1}\r\n"
        "Type     : {2}\r\n"
        "Message  : {3}\r\n"
        "---------------------------------------------------------------------------\r\n"
        "{4}\r\n"
        "---------------------------------------------------------------------------\r\n",
        FHUI_CONTACT_EMAILS,
        FHUI_BUILD_INFO(),
        e->GetType()->ToString(),
        e->Message,
        e->InnerException == nullptr ? e->StackTrace : e->InnerException->StackTrace );
}

void Form1::FillAboutBox()
{
    String ^changeLog = nullptr;

    try {
        StreamReader ^sr = File::OpenText("changelog.txt");
        changeLog = sr->ReadToEnd();
    }
    catch( SystemException^ )
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
                m_GalaxySize = 2 * int::Parse(m->Groups[1]->ToString());
                break;
            }
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unrecognized entry in galaxy list: {0}", line) );
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
        int n = m_Reports->Count;
        array<String^> ^arr = gcnew array<String^>(n);
        for each( int key in m_Reports->Keys )
            arr[--n] = "Turn " + key.ToString();
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

    try
    {
        while( (line = sr->ReadLine()) != nullptr ) 
        {
            if( false == report->Parse(line) )
                break;
        }

        if( report->IsValid() )
            m_Reports->Add(report->GetTurn(), report);
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

void Form1::DisplayReport()
{
    String ^sel = RepTurnNr->SelectedItem->ToString();
    int key = int::Parse(sel->Substring(5));    // Skip 'Turn '

    RepText->Text = m_Reports[key]->GetContent();
}

void Form1::LoadCommands()
{
}

void Form1::InitData()
{
    m_GameData = gcnew GameData;

    m_GalaxySize = 0;
    RepMode->SelectedIndex = 1;
}

////////////////////////////////////////////////////////////////
// Systems

String^ Form1::SystemsGetRowTooltip(DataGridViewRow ^row)
{
    try
    {
        int x = int::Parse(row->Cells[ SystemsGrid->Columns["X"]->Index ]->Value->ToString());
        int y = int::Parse(row->Cells[ SystemsGrid->Columns["Y"]->Index ]->Value->ToString());
        int z = int::Parse(row->Cells[ SystemsGrid->Columns["Z"]->Index ]->Value->ToString());
        StarSystem ^system = m_GameData->GetStarSystem(x, y, z);
        return system->GenerateScan();
    }
    catch( Exception ^e )
    {
        ShowException(e);
    }

    return nullptr;
}

void Form1::SetupSystems()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("X", int::typeid );
    dataTable->Columns->Add("Y", int::typeid );
    dataTable->Columns->Add("Z", int::typeid );
    dataTable->Columns->Add("Type", String::typeid );
    dataTable->Columns->Add("Planets", int::typeid );
    dataTable->Columns->Add("MinLSN", int::typeid );
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );
    dataTable->Columns->Add("Notes", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double mishap = system->CalcMishap(sp->HomeSystem, gv, 0);

        DataRow^ row = dataTable->NewRow();
        row["X"]        = system->X;
        row["Y"]        = system->Y;
        row["Z"]        = system->Z;
        row["Type"]     = system->Type;
        if( system->IsExplored() )
        {
            row["Planets"]  = system->PlanetsCount;
            row["MinLSN"]   = system->GetMinLSN();
        }
        row["Dist."]    = system->CalcDistance(sp->HomeSystem);
        row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row["Scan"]     = system->PrintScanTurn();
        row["Notes"]     = system->Comment;

        dataTable->Rows->Add(row);
    }
    SystemsGrid->DataSource = dataTable;

    // Formatting
    SystemsGrid->Columns["Dist."]->DefaultCellStyle->Format = "F2";
    for each( DataColumn ^col in dataTable->Columns )
    {
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            SystemsGrid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Some columns are not sortable... yet
    SystemsGrid->Columns["Scan"]->SortMode = DataGridViewColumnSortMode::NotSortable;
    SystemsGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    SystemsGrid->Sort( SystemsGrid->Columns["Dist."], ListSortDirection::Ascending );
}

////////////////////////////////////////////////////////////////
// GUI misc

Color Form1::GetAlienColor(Alien ^sp)
{
    try
    {
        if( sp == m_GameData->GetSpecies() )
            return Color::FromArgb(225, 255, 255);

        switch( sp->Relation )
        {
        case SP_NEUTRAL:    return Color::FromArgb(255, 255, 220);
        case SP_ALLY:       return Color::FromArgb(220, 255, 210);
        case SP_ENEMY:      return Color::FromArgb(255, 220, 220);
        case SP_PIRATE:     return Color::FromArgb(230, 230, 230);
        }
    }
    catch( Exception ^e )
    {
        ShowException(e);
    }

    return Color::White;
}

////////////////////////////////////////////////////////////////
// Planets

void Form1::SetupPlanets()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("Name", String::typeid );
    dataTable->Columns->Add("Coords", String::typeid );
    dataTable->Columns->Add("Temp", int::typeid );
    dataTable->Columns->Add("Press", int::typeid );
    dataTable->Columns->Add("MD", double::typeid );
    dataTable->Columns->Add("LSN", int::typeid );
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );
    dataTable->Columns->Add("Notes", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double distance = system->CalcDistance(sp->HomeSystem);
        double mishap = system->CalcMishap(sp->HomeSystem, gv, 0);

        for each( Planet ^planet in system->GetPlanets() )
        {
            DataRow^ row = dataTable->NewRow();
            row["Name"]     = planet->Name;
            row["Coords"]   = String::Format("{0,2} {1,2} {2,2} {3}",
                system->X, system->Y, system->Z, planet->Number);
            row["Temp"]     = planet->TempClass;
            row["Press"]    = planet->PressClass;
            row["MD"]       = planet->MiningDiff;
            row["LSN"]      = planet->LSN;
            row["Dist."]    = distance;
            row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
            row["Scan"]     = system->PrintScanTurn();
            row["Notes"]    = planet->Comment;

            dataTable->Rows->Add(row);
        }
    }
    PlanetsGrid->DataSource = dataTable;

    // Formatting
    PlanetsGrid->Columns["MD"]->DefaultCellStyle->Format = "F2";
    PlanetsGrid->Columns["Dist."]->DefaultCellStyle->Format = "F2";
    for each( DataColumn ^col in dataTable->Columns )
    {
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            PlanetsGrid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Some columns are not sortable... yet
    PlanetsGrid->Columns["Scan"]->SortMode = DataGridViewColumnSortMode::NotSortable;
    PlanetsGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    PlanetsGrid->Sort( PlanetsGrid->Columns["LSN"], ListSortDirection::Ascending );
}

////////////////////////////////////////////////////////////////
// Colonies

Color Form1::ColoniesGetRowColor(DataGridViewRow ^row)
{
    String ^name = row->Cells[ ColoniesGrid->Columns["Name"]->Index ]->Value->ToString();
    return GetAlienColor( m_GameData->GetColony(name)->Owner );
}

void Form1::SetupColonies()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("Owner", String::typeid );
    dataTable->Columns->Add("Name", String::typeid );
    dataTable->Columns->Add("Type", String::typeid );
    dataTable->Columns->Add("Location", String::typeid );
    dataTable->Columns->Add("Size", double::typeid );
    dataTable->Columns->Add("Seen", int::typeid );
    dataTable->Columns->Add("Prod.", int::typeid );
    dataTable->Columns->Add("Pr[%]", int::typeid );
    dataTable->Columns->Add("Balance", String::typeid );
    dataTable->Columns->Add("Pop", int::typeid );
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Inventory", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->TechLevels[TECH_GV];

    for each( Colony ^colony in m_GameData->GetColonies() )
    {
        double distance = colony->System->CalcDistance(sp->HomeSystem);
        double mishap = colony->System->CalcMishap(sp->HomeSystem, gv, 0);

        DataRow^ row = dataTable->NewRow();
        row["Owner"]    = colony->Owner == sp ? String::Format("* {0}", sp->Name) : colony->Owner->Name;
        row["Name"]     = colony->Name;
        row["Type"]     = PlTypeToString(colony->PlanetType);
        row["Location"] = colony->PrintLocation();
        if( colony->MaBase != 0 || colony->MiBase != 0 )
            row["Size"]     = colony->MiBase + colony->MaBase;
        row["Dist."]    = distance;
        row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row["Inventory"]= colony->PrintInventoryShort();

        if( colony->Owner == sp )
        {
            row["Prod."]    = colony->EUProd - colony->EUFleet;
            row["Pr[%]"]    = 100 - colony->ProdPenalty;
            row["Pop"]      = colony->AvailPop;

            if( colony->PlanetType == PLANET_HOME ||
                colony->PlanetType == PLANET_COLONY )
            {
                int mi = (int)((sp->TechLevels[TECH_MI] * colony->MiBase) / colony->Planet->MiningDiff);
                int ma = (int)(sp->TechLevels[TECH_MA] * colony->MaBase);
                if( mi == ma )
                    row["Balance"] = "Balanced";
                else if( mi < ma )
                    row["Balance"] = String::Format("+{0} MA cap.", ma - mi);
                else
                    row["Balance"] = String::Format("-{0} MA cap.", mi - ma);
            }
        }
        else
        {
            row["Seen"]     = colony->LastSeen;
        }

        dataTable->Rows->Add(row);
    }
    ColoniesGrid->DataSource = dataTable;

    // Formatting
    ColoniesGrid->Columns["Dist."]->DefaultCellStyle->Format = "F2";
    ColoniesGrid->Columns["Size"]->DefaultCellStyle->Format = "F1";
    for each( DataColumn ^col in dataTable->Columns )
    {
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            ColoniesGrid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Some columns are not sortable... yet
    ColoniesGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    ColoniesGrid->Sort( ColoniesGrid->Columns["Owner"], ListSortDirection::Ascending );
}

////////////////////////////////////////////////////////////////
// Ships

Color Form1::ShipsGetRowColor(DataGridViewRow ^row)
{
    String ^name = row->Cells[ ShipsGrid->Columns["Name"]->Index ]->Value->ToString();
    return GetAlienColor( m_GameData->GetShip(name)->Owner );
}

void Form1::SetupShips()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("Owner", String::typeid );
    dataTable->Columns->Add("Class", String::typeid );
    dataTable->Columns->Add("Name", String::typeid );
    dataTable->Columns->Add("Location", String::typeid );
    dataTable->Columns->Add("Age", int::typeid );
    dataTable->Columns->Add("Cap.", int::typeid );
    dataTable->Columns->Add("Cargo", String::typeid );
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->TechLevels[TECH_GV];

    for each( Ship ^ship in m_GameData->GetShips() )
    {
        double distance = StarSystem::CalcDistance(
            ship->X,
            ship->Y,
            ship->Z,
            sp->HomeSystem->X,
            sp->HomeSystem->Y,
            sp->HomeSystem->Z);
        double mishap = StarSystem::CalcMishap(
            ship->X,
            ship->Y,
            ship->Z,
            sp->HomeSystem->X,
            sp->HomeSystem->Y,
            sp->HomeSystem->Z,
            gv,
            ship->Age);

        DataRow^ row = dataTable->NewRow();
        row["Owner"]    = ship->Owner == sp ? String::Format("* {0}", sp->Name) : ship->Owner->Name;
        row["Class"]    = ship->PrintClass();
        row["Name"]     = ship->Name;
        row["Location"] = ship->PrintLocation();
        if( !ship->IsPirate )
            row["Age"]  = ship->Age;
        row["Cap."]     = ship->Capacity;
        row["Cargo"]    = ( ship->Owner == sp )
            ? ship->PrintCargo()
            : "n/a";
        row["Dist."]    = distance;
        row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);

        dataTable->Rows->Add(row);
    }
    ShipsGrid->DataSource = dataTable;

    // Formatting
    ShipsGrid->Columns["Dist."]->DefaultCellStyle->Format = "F2";
    for each( DataColumn ^col in dataTable->Columns )
    {
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            ShipsGrid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Some columns are not sortable... yet
    ShipsGrid->Columns["Mishap %"]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Default sort column
    ShipsGrid->Sort( ShipsGrid->Columns["Owner"], ListSortDirection::Ascending );
}

////////////////////////////////////////////////////////////////
// Aliens

Color Form1::AliensGetRowColor(DataGridViewRow ^row)
{
    String ^name = row->Cells[ AliensGrid->Columns["Name"]->Index ]->Value->ToString();
    return GetAlienColor( m_GameData->GetAlien(name) );
}

void Form1::SetupAliens()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    dataTable->Columns->Add("Name", String::typeid );
    dataTable->Columns->Add("Relation", String::typeid );
    dataTable->Columns->Add("Home", String::typeid );
    dataTable->Columns->Add("Tech Levels", String::typeid );
    dataTable->Columns->Add("Temp", int::typeid );
    dataTable->Columns->Add("Press", int::typeid );
    dataTable->Columns->Add("EMail", String::typeid );

    for each( Alien ^alien in m_GameData->GetAliens() )
    {
        DataRow^ row = dataTable->NewRow();
        row["Name"]         = alien->Name;
        row["Relation"]     = alien->PrintRelation();
        row["Home"]         = alien->PrintHome();
        row["Tech Levels"]  = alien->PrintTechLevels();
        if( alien->AtmReq->TempClass != -1 &&
            alien->AtmReq->PressClass != -1 )
        {
            row["Temp"]     = alien->AtmReq->TempClass;
            row["Press"]    = alien->AtmReq->PressClass;
        }
        row["EMail"]        = alien->Email;
        dataTable->Rows->Add(row);
    }
    AliensGrid->DataSource = dataTable;

    // Formatting
    for each( DataColumn ^col in dataTable->Columns )
    {
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            AliensGrid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Default sort column
    AliensGrid->Sort( AliensGrid->Columns["Relation"], ListSortDirection::Ascending );
}


////////////////////////////////////////////////////////////////

}
