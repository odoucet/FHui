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

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            m_GameData->GetSpeciesName(),
            m_GameData->GetLastTurn(),
            FHUI_BUILD_INFO() );
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed loading game data.";
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

////////////////////////////////////////////////////////////////
// Systems

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
    dataTable->Columns->Add("Planets", int::typeid );
    dataTable->Columns->Add("MinLSN", int::typeid );
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Scan", String::typeid );
    dataTable->Columns->Add("Notes", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->m_TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double mishap = system->CalcMishap(sp->m_HomeSystem, gv, 0);

        DataRow^ row = dataTable->NewRow();
        row["X"]        = system->X;
        row["Y"]        = system->Y;
        row["Z"]        = system->Z;
        row["Type"]     = system->Type;
        if( system->m_TurnScanned != -1 )
        {
            int minLSN = 99999;
            for each( Planet ^pl in system->m_Planets )
                minLSN = Math::Min(minLSN, pl->m_LSN == -1 ? 99999 : pl->m_LSN);
            row["Planets"]  = system->m_Planets->Length;
            row["MinLSN"]   = minLSN;
        }
        row["Dist."]    = system->CalcDistance(sp->m_HomeSystem);
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
    if( sp == m_GameData->GetSpecies() )
        return Color::FromArgb(225, 255, 255);

    switch( sp->m_Relation )
    {
    case SP_NEUTRAL:    return Color::FromArgb(255, 255, 220);
    case SP_ALLY:       return Color::FromArgb(220, 255, 210);
    case SP_ENEMY:      return Color::FromArgb(255, 220, 220);
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
    int gv = sp->m_TechLevels[TECH_GV];

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double distance = system->CalcDistance(sp->m_HomeSystem);
        double mishap = system->CalcMishap(sp->m_HomeSystem, gv, 0);

        for each( Planet ^planet in system->m_Planets )
        {
            DataRow^ row = dataTable->NewRow();
            row["Name"]     = planet->m_Name;
            row["Coords"]   = String::Format("{0,2} {1,2} {2,2} {3}",
                system->X, system->Y, system->Z, planet->m_Number);
            row["Temp"]     = planet->m_TempClass;
            row["Press"]    = planet->m_PressClass;
            row["MD"]       = planet->m_MiningDiff;
            row["LSN"]      = planet->m_LSN;
            row["Dist."]    = distance;
            row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
            row["Scan"]     = system->PrintScanTurn();
            row["Notes"]    = planet->m_Comment;

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
    return GetAlienColor( m_GameData->GetColony(name)->m_Owner );
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
    dataTable->Columns->Add("Dist.", double::typeid );
    dataTable->Columns->Add("Mishap %", String::typeid );
    dataTable->Columns->Add("Inventory", String::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->m_TechLevels[TECH_GV];

    for each( DictionaryEntry ^entry in m_GameData->GetColonies() )
    {
        Colony ^colony = safe_cast<Colony^>(entry->Value);

        double distance = colony->m_System->CalcDistance(sp->m_HomeSystem);
        double mishap = colony->m_System->CalcMishap(sp->m_HomeSystem, gv, 0);

        DataRow^ row = dataTable->NewRow();
        row["Owner"]    = colony->m_Owner == sp ? String::Format("* {0}", sp->m_Name) : colony->m_Owner->m_Name;
        row["Name"]     = colony->m_Name;
        row["Type"]     = PlTypeToString(colony->m_PlanetType);
        row["Location"] = colony->PrintLocation();
        row["Size"]     = colony->m_MiBase + colony->m_MaBase;
        row["Dist."]    = distance;
        row["Mishap %"] = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row["Inventory"]= colony->PrintInventoryShort();

        if( colony->m_Owner == sp )
        {
            row["Prod."]    = colony->m_EUProd - colony->m_EUFleet;
            row["Pr[%]"]    = 100 - colony->m_ProdPenalty;

            if( colony->m_PlanetType == PLANET_HOME ||
                colony->m_PlanetType == PLANET_COLONY )
            {
                int mi = (int)((sp->m_TechLevels[TECH_MI] * colony->m_MiBase) / colony->m_Planet->m_MiningDiff);
                int ma = (int)(sp->m_TechLevels[TECH_MA] * colony->m_MaBase);
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
            row["Seen"]     = colony->m_LastSeen;
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
    return GetAlienColor( m_GameData->GetShip(name)->m_Owner );
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
    int gv = sp->m_TechLevels[TECH_GV];

    for each( DictionaryEntry ^entry in m_GameData->GetShips() )
    {
        Ship ^ship = safe_cast<Ship^>(entry->Value);

        double distance = StarSystem::CalcDistance(
            ship->m_X,
            ship->m_Y,
            ship->m_Z,
            sp->m_HomeSystem->X,
            sp->m_HomeSystem->Y,
            sp->m_HomeSystem->Z);
        double mishap = StarSystem::CalcMishap(
            ship->m_X,
            ship->m_Y,
            ship->m_Z,
            sp->m_HomeSystem->X,
            sp->m_HomeSystem->Y,
            sp->m_HomeSystem->Z,
            gv,
            ship->m_Age);

        DataRow^ row = dataTable->NewRow();
        row["Owner"]    = ship->m_Owner == sp ? String::Format("* {0}", sp->m_Name) : ship->m_Owner->m_Name;
        row["Class"]    = ship->PrintClass();
        row["Name"]     = ship->m_Name;
        row["Location"] = ship->PrintLocation();
        row["Age"]      = ship->m_Age;
        row["Cap."] = ship->m_Capacity;
        row["Cargo"]    = ( ship->m_Owner == sp )
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

    for each( DictionaryEntry ^entry in m_GameData->GetAliens() )
    {
        Alien ^alien = safe_cast<Alien^>(entry->Value);

        DataRow^ row = dataTable->NewRow();
        row["Name"]         = alien->m_Name;
        row["Relation"]     = alien->PrintRelation();
        row["Home"]         = alien->PrintHome();
        row["Tech Levels"]  = alien->PrintTechLevels();
        if( alien->m_AtmReq->m_TempClass != -1 &&
            alien->m_AtmReq->m_PressClass != -1 )
        {
            row["Temp"]     = alien->m_AtmReq->m_TempClass;
            row["Press"]    = alien->m_AtmReq->m_PressClass;
        }
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
