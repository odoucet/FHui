#include "StdAfx.h"
#include "Form1.h"

#include "Report.h"
#include "GridFilter.h"
#include "svn_rev.h"

using namespace System::IO;
using namespace System::Text::RegularExpressions;

#define FHUI_CONTACT_EMAILS "jdukat+fhui@gmail.com"
#define FHUI_CONTACT_WIKI   "http://www.cetnerowski.com/farhorizons/pmwiki/pmwiki.php/FHUI/FHUI"

#define FHUI_BUILD_INFO_APPENDIX ""
#define FHUI_BUILD_INFO() String::Format("{0}{1}{2}", FHUI_REVISION_NUMBER, FHUI_BUILD_INFO_APPENDIX, FHUI_REVISION_MODIFIED ? " (modified)" : "")

#define OPTIMAL_ROW_HEIGHT 18

////////////////////////////////////////////////////////////////

namespace fhui 
{

void Form1::LoadGameData()
{
    try
    {
        FillAboutBox();
        InitData();
        InitControls();
        ScanReports();
        LoadCommands();
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed loading game data.";
        ShowException(e);
    }
}

void Form1::InitControls()
{
    System::Text::RegularExpressions::Regex::CacheSize = 500;

    GridFilter ^filter;

    // -- systems
    filter = gcnew GridFilter(SystemsGrid, m_bGridUpdateEnabled);
    filter->OnGridSetup += gcnew GridSetupHandler(this, &Form1::SystemsSetup);
    filter->OnGridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = SystemsRef;
    filter->CtrlRefHome     = SystemsRefHome;
    filter->CtrlRefXYZ      = SystemsRefXYZ;
    filter->CtrlRefColony   = SystemsRefColony;
    filter->CtrlRefShip     = SystemsRefShip;
    filter->CtrlGV          = SystemsGV;
    filter->CtrlShipAge     = SystemsShipAge;
    filter->CtrlMaxMishap   = SystemsMaxMishap;
    filter->CtrlMaxLSN      = SystemsMaxLSN;
    filter->CtrlFiltVisV    = SystemsFiltVisV;
    filter->CtrlFiltVisN    = SystemsFiltVisN;
    filter->CtrlFiltColC    = SystemsFiltColC;
    filter->CtrlFiltColN    = SystemsFiltColN;

    m_SystemsFilter = filter;

    // -- planets
    filter = gcnew GridFilter(PlanetsGrid, m_bGridUpdateEnabled);
    filter->OnGridSetup += gcnew GridSetupHandler(this, &Form1::PlanetsSetup);
    filter->OnGridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = PlanetsRef;
    filter->CtrlRefHome     = PlanetsRefHome;
    filter->CtrlRefXYZ      = PlanetsRefXYZ;
    filter->CtrlRefColony   = PlanetsRefColony;
    filter->CtrlRefShip     = PlanetsRefShip;
    filter->CtrlGV          = PlanetsGV;
    filter->CtrlShipAge     = PlanetsShipAge;
    filter->CtrlMaxMishap   = PlanetsMaxMishap;
    filter->CtrlMaxLSN      = PlanetsMaxLSN;
    filter->CtrlFiltVisV    = PlanetsFiltVisV;
    filter->CtrlFiltVisN    = PlanetsFiltVisN;
    filter->CtrlFiltColC    = PlanetsFiltColC;
    filter->CtrlFiltColN    = PlanetsFiltColN;

    m_PlanetsFilter = filter;

    // -- colonies
    filter = gcnew GridFilter(ColoniesGrid, m_bGridUpdateEnabled);
    filter->OnGridSetup += gcnew GridSetupHandler(this, &Form1::ColoniesSetup);
    filter->OnGridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = ColoniesRef;
    filter->CtrlRefHome     = ColoniesRefHome;
    filter->CtrlRefXYZ      = ColoniesRefXYZ;
    filter->CtrlRefColony   = ColoniesRefColony;
    filter->CtrlRefShip     = ColoniesRefShip;
    filter->CtrlGV          = ColoniesGV;
    filter->CtrlShipAge     = ColoniesShipAge;
    filter->CtrlMaxMishap   = ColoniesMaxMishap;
    filter->CtrlMaxLSN      = ColoniesMaxLSN;
    filter->CtrlFiltOwnO    = ColoniesFiltOwnO;
    filter->CtrlFiltOwnN    = ColoniesFiltOwnN;
    filter->CtrlMiMaBalance = ColoniesMiMaBalanced;

    m_ColoniesFilter = filter;

    // -- ships
    filter = gcnew GridFilter(ShipsGrid, m_bGridUpdateEnabled);
    filter->OnGridSetup += gcnew GridSetupHandler(this, &Form1::ShipsSetup);
    filter->OnGridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = ShipsRef;
    filter->CtrlRefHome     = ShipsRefHome;
    filter->CtrlRefXYZ      = ShipsRefXYZ;
    filter->CtrlRefColony   = ShipsRefColony;
    filter->CtrlRefShip     = ShipsRefShip;
    filter->CtrlGV          = ShipsGV;
    filter->CtrlShipAge     = ShipsShipAge;
    filter->CtrlMaxMishap   = ShipsMaxMishap;
    filter->CtrlFiltOwnO    = ShipsFiltOwnO;
    filter->CtrlFiltOwnN    = ShipsFiltOwnN;
    filter->CtrlFiltRelA    = ShipsFiltRelA;
    filter->CtrlFiltRelE    = ShipsFiltRelE;
    filter->CtrlFiltRelN    = ShipsFiltRelN;
    filter->CtrlFiltRelP    = ShipsFiltRelP;
    filter->CtrlFiltTypeBas = ShipsFiltTypeBA;
    filter->CtrlFiltTypeMl  = ShipsFiltTypeML;
    filter->CtrlFiltTypeTr  = ShipsFiltTypeTR;

    m_ShipsFilter = filter;

    // -- aliens
    filter = gcnew GridFilter(AliensGrid, m_bGridUpdateEnabled);
    filter->OnGridSetup += gcnew GridSetupHandler(this, &Form1::AliensSetup);
    filter->OnGridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlFiltRelA    = AliensFiltRelA;
    filter->CtrlFiltRelE    = AliensFiltRelE;
    filter->CtrlFiltRelN    = AliensFiltRelN;
    filter->CtrlFiltRelP    = AliensFiltRelP;

    m_AliensFilter = filter;
}

void Form1::InitData()
{
    m_HadException = false;

    m_bGridUpdateEnabled = gcnew bool(false);

    m_GalaxySize = 0;

    m_RepTurnNrData = nullptr;

    m_GameTurns = gcnew SortedList<int, GameData^>;
    m_Reports   = gcnew SortedList<int, String^>;
    m_RepFiles  = gcnew SortedList<int, String^>;
    m_CmdFiles  = gcnew SortedList<String^, String^>;
}

void Form1::InitRefLists()
{
    m_RefListSystemsXYZ = gcnew List<String^>;
    m_RefListHomes      = gcnew List<String^>;
    m_RefListColonies   = gcnew List<String^>;
    m_RefListShips      = gcnew List<String^>;

    Alien ^sp = m_GameData->GetSpecies();

    // -- ref systems xyz:
    m_RefListSystemsXYZ->Add( GridFilter::s_CaptionXYZ );
    for each( StarSystem ^system in m_GameData->GetStarSystems() )
        m_RefListSystemsXYZ->Add( system->PrintLocation() );

    // -- home systems:
    m_RefListHomes->Add( GridFilter::s_CaptionHome );
    m_RefListHomes->Add( sp->Name );
    for each( Alien ^alien in m_GameData->GetAliens() )
        if( alien != sp && alien->HomeSystem )
            m_RefListHomes->Add( alien->Name );

    // -- colonies:
    // owned first
    m_RefListColonies->Add( GridFilter::s_CaptionColony );
    for each( Colony ^colony in sp->Colonies )
        m_RefListColonies->Add( colony->PrintRefListEntry(sp) );
    // then alien
    for each( Colony ^colony in m_GameData->GetColonies() )
        if( colony->Owner != sp )
            m_RefListColonies->Add( colony->PrintRefListEntry(sp) );

    // -- ref ship age:
    m_RefListShips->Add( GridFilter::s_CaptionShip );
    for each( Ship ^ship in sp->Ships )
        if( ship->Type != SHIP_BAS &&
            ship->SubLight == false )
        {
            m_RefListShips->Add( ship->PrintRefListEntry() );
        }
}

void Form1::UpdateControls()
{
    InitRefLists();

    TurnSelect->Enabled = true;
    MenuTabs->Enabled   = true;

    RepModeReports->Checked = true;

    SystemsUpdateControls();
    PlanetsUpdateControls();
    ColoniesUpdateControls();
    ShipsUpdateControls();
    AliensUpdateControls();
}

void Form1::ShowException(Exception ^e)
{
    m_HadException = true;

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

    // Bring up the first tab
    MenuTabs->SelectedTab = MenuTabs->TabPages[0];

    // Disable some critical controls
    TurnSelect->Enabled = false;
    //MenuTabs->Enabled = false;
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

void Form1::ScanReports()
{
    DirectoryInfo ^dir = gcnew DirectoryInfo("reports");

    for each( FileInfo ^f in dir->GetFiles("*"))
    {   // First check each file if it is a report and find out for which turn.
        // Then reports will be loaded in chronological order.
        int turn = CheckReport(f->FullName);
        if( turn != -1 )
        {
            m_RepFiles[turn] = f->FullName;
        }
    }

    if( m_RepFiles->Count > 0 )
    {
        // Setup combo box with list of loaded reports
        int n0 = m_RepFiles->Count;
        int n1 = n0 - (m_RepFiles->ContainsKey(0) ? 1 : 0);
        array<String^> ^arrN0 = gcnew array<String^>(n0);
        array<String^> ^arrN1 = gcnew array<String^>(n1);
        for each( int key in m_RepFiles->Keys )
        {
            String ^text = "Turn " + key.ToString();
            arrN0[--n0] = text;
            if( key != 0 )
                arrN1[--n1] = "Status for " + text;
        }

        m_RepTurnNrData = arrN0;
        TurnSelect->DataSource = arrN1;
    }
    else
    {
        RepText->Text = "No report successfully loaded.";
    }
}

void Form1::TurnReload()
{
    m_GameTurns->Remove( m_GameData->GetLastTurn() );
    DisplayTurn();
}

void Form1::DisplayTurn()
{
    try
    {
        String ^sel = TurnSelect->Text;
        int turn = int::Parse(sel->Substring(16));    // Skip 'Status for Turn '

        *m_bGridUpdateEnabled = false;
        LoadGameTurn(turn);
        RepModeChanged();
        UpdateControls();

        // Display summary
        Summary->Text = m_GameData->GetSummary();

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            m_GameData->GetSpeciesName(),
            m_GameData->GetLastTurn(),
            FHUI_BUILD_INFO() );

        MapSetup();
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed loading game data.";
        ShowException(e);
    }
}

void Form1::LoadGameTurn(int turn)
{
    if( m_GameTurns->ContainsKey(turn) )
    {
        m_GameData = m_GameTurns[turn];
        return;
    }

    m_GameData = gcnew GameData;
    LoadGalaxy();

    for each( int t in m_RepFiles->Keys )
    {
        if( t <= turn )
            LoadReport( m_RepFiles[t] );
        else
            break;
    }

    m_GameData->Update();
    m_GameTurns[turn] = m_GameData;
}

int Form1::CheckReport(String ^fileName)
{
    Report ^report = gcnew Report(nullptr); // turn scan mode

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

void Form1::RepModeChanged()
{
    if( m_HadException )
        return;

    if( RepModeReports->Checked )
    {
        RepTurnNr->DataSource = m_RepTurnNrData;
    }
    else if( RepModeCommands->Checked )
    {
        List<String^> ^items = gcnew List<String^>;
        for each( String ^s in m_CmdFiles->Keys )
            items->Add(s);
        items->Sort(StringComparer::CurrentCultureIgnoreCase);
        RepTurnNr->DataSource = items;
    }
}

void Form1::DisplayReport()
{
    if( RepTurnNr->SelectedItem == nullptr )
    {
        RepText->Text = "";
        return;
    }

    if( RepModeReports->Checked )
    {
        String ^sel = RepTurnNr->SelectedItem->ToString();
        int key = int::Parse(sel->Substring(5));    // Skip 'Turn '

        RepText->Text = m_Reports[key];
    }
    else if( RepModeCommands->Checked )
    {
        RepText->Text = m_CmdFiles[ RepTurnNr->SelectedItem->ToString() ];
    }
}

void Form1::LoadCommands()
{
    m_CmdFiles->Clear();

    try
    {
        DirectoryInfo ^dir = gcnew DirectoryInfo("orders");

        for each( FileInfo ^f in dir->GetFiles("*"))
        {
            if( f->Length <= 0x10000 ) // 64 KB, more than enough for any commands
            {
                m_CmdFiles[f->Name] = File::OpenText(f->FullName)->ReadToEnd();
            }
            else
            {
                m_CmdFiles[f->Name] = "File too large (limit is 64KB).";
            }
        }
    }
    catch( DirectoryNotFoundException^ )
    {
    }
    finally
    {
        RepModeCommands->Enabled = m_CmdFiles->Count > 0;
    }
}

////////////////////////////////////////////////////////////////
// GUI misc

void Form1::ApplyDataAndFormat(
    DataGridView ^grid,
    DataTable ^data,
    DataColumn ^objColumn,
    int defaultSortColIdx )
{
    int sortBy = defaultSortColIdx;
    ListSortDirection sortDir = ListSortDirection::Ascending;
    if( grid->SortedColumn )
    {
        sortBy = grid->SortedColumn->Index;
        switch( grid->SortOrder )
        {
        case SortOrder::None:
            sortBy = -1;
            break;
        case SortOrder::Descending:
            sortDir = ListSortDirection::Descending;
            break;
        }

    }

    // Setup data source
    grid->DataSource = data;

    // Make object link column invisible
    grid->Columns[objColumn->Ordinal]->Visible = false;

    // Adjust row height
    grid->RowTemplate->Height = OPTIMAL_ROW_HEIGHT;

    // Formatting
    for each( DataColumn ^col in data->Columns )
    {
        if( col->DataType == double::typeid )
            grid->Columns[col->Ordinal]->DefaultCellStyle->Format = "F2";
        if( col->DataType == double::typeid || col->DataType == int::typeid )
            grid->Columns[col->Ordinal]->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Sort grid
    if( sortBy != -1 )
    {
        grid->Sort( grid->Columns[sortBy], sortDir );
    }

    grid->ClearSelection();
}

Color Form1::GetAlienColor(Alien ^sp)
{
    if( sp == nullptr )
        return Color::White;

    if( sp == m_GameData->GetSpecies() )
        return Color::FromArgb(225, 255, 255);

    switch( sp->Relation )
    {
    case SP_NEUTRAL:    return Color::FromArgb(255, 255, 210);
    case SP_ALLY:       return Color::FromArgb(220, 255, 210);
    case SP_ENEMY:      return Color::FromArgb(255, 220, 220);
    case SP_PIRATE:     return Color::FromArgb(230, 230, 230);
    case SP_MIXED:      return Color::FromArgb(235, 235, 235);
    }

    return Color::White;
}

void Form1::SetGridBgAndTooltip(DataGridView ^grid)
{
    try
    {
        int index = grid->Columns[0]->Index;

        for each( DataGridViewRow ^row in grid->Rows )
        {
            IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(row->Cells[index]->Value);
            String ^tooltip = iDataSrc->GetTooltipText();
            Color bgColor = GetAlienColor( iDataSrc->GetAlienForBgColor() );
            for each( DataGridViewCell ^cell in row->Cells )
            {
                cell->ToolTipText = tooltip;
                cell->Style->BackColor = bgColor;
            }
        }
    }
    catch( Exception ^e )
    {
        ShowException(e);
    }
}

void Form1::SetGridRefSystemOnMouseClick(DataGridView ^grid, int rowIndex)
{
    if( rowIndex >= 0 )
    {
        int index = grid->Columns[0]->Index;
        IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(grid->Rows[ rowIndex ]->Cells[index]->Value);

        StarSystem ^system = iDataSrc->GetFilterSystem();
        if( system )
        {
            IGridFilter ^filter = nullptr;
            if( grid == SystemsGrid )
                filter = m_SystemsFilter;
            else if( grid == PlanetsGrid )
                filter = m_PlanetsFilter;
            else if( grid == ColoniesGrid )
                filter = m_ColoniesFilter;
            else if( grid == ShipsGrid )
                filter = m_ShipsFilter;
            else if( grid == AliensGrid )
                filter = m_AliensFilter;
            if( filter )
                filter->SetRefSystem(system);
        }
    }
}

////////////////////////////////////////////////////////////////
// Systems

void Form1::SystemsUpdateControls()
{
    // Inhibit grid update
    m_SystemsFilter->EnableUpdates  = false;

    m_SystemsFilter->GameData       = m_GameData;

    SystemsGV->Value                = Math::Max(1, m_GameData->GetSpecies()->TechLevels[TECH_GV]);

    SystemsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    SystemsRefHome->DataSource      = m_RefListHomes;
    SystemsRefColony->DataSource    = m_RefListColonies;
    SystemsRefShip->DataSource      = m_RefListShips;

    // Trigger grid update
    m_SystemsFilter->EnableUpdates  = true;
    SystemsRefHome->Text            = m_GameData->GetSpeciesName();
    m_SystemsFilter->Reset();
}

void Form1::SystemsSetup()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    DataColumn ^colObject   = dataTable->Columns->Add("system",     StarSystem::typeid );
    DataColumn ^colX        = dataTable->Columns->Add("X",          int::typeid );
    DataColumn ^colY        = dataTable->Columns->Add("Y",          int::typeid );
    DataColumn ^colZ        = dataTable->Columns->Add("Z",          int::typeid );
    DataColumn ^colType     = dataTable->Columns->Add("Type",       String::typeid );
    DataColumn ^colPlanets  = dataTable->Columns->Add("Planets",    int::typeid );
    DataColumn ^colLSN      = dataTable->Columns->Add("Min LSN",    int::typeid );
    DataColumn ^colLSNAvail = dataTable->Columns->Add("Free LSN",   int::typeid );
    DataColumn ^colDist     = dataTable->Columns->Add("Dist.",      double::typeid );
    DataColumn ^colMishap   = dataTable->Columns->Add("Mishap %",   String::typeid );
    DataColumn ^colVisited  = dataTable->Columns->Add("Visited",    int::typeid );
    DataColumn ^colScan     = dataTable->Columns->Add("Scan",       String::typeid );
    DataColumn ^colColonies = dataTable->Columns->Add("Colonies",   String::typeid );
    DataColumn ^colNotes    = dataTable->Columns->Add("Notes",      String::typeid );

    int gv  = Decimal::ToInt32(SystemsGV->Value);
    int age = Decimal::ToInt32(SystemsShipAge->Value);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        if( m_SystemsFilter->Filter(system) )
            continue;

        double mishap = system->CalcMishap(m_SystemsFilter->RefSystem, gv, age);

        DataRow^ row = dataTable->NewRow();
        row[colObject]      = system;
        row[colX]           = system->X;
        row[colY]           = system->Y;
        row[colZ]           = system->Z;
        row[colType]        = system->Type;
        if( system->IsExplored() )
        {
            row[colPlanets] = system->PlanetsCount;
            row[colLSN]     = system->MinLSN;
            if( system->MinLSNAvail != 99999 )
                row[colLSNAvail]= system->MinLSNAvail;
        }
        row[colDist]        = system->CalcDistance(m_SystemsFilter->RefSystem);
        row[colMishap]      = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row[colScan]        = system->PrintScanStatus();
        if( system->LastVisited != -1 )
            row[colVisited] = system->LastVisited;
        row[colColonies]    = system->PrintColonies( -1, m_GameData->GetSpecies() );
        row[colNotes]       = system->Comment;

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(SystemsGrid, dataTable, colObject, colDist->Ordinal);

    // Some columns are not sortable... yet
    SystemsGrid->Columns[colMishap->Ordinal]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Enable filters
    m_SystemsFilter->EnableUpdates = true;
}

void Form1::SystemsSelectPlanets( int rowIndex )
{
    if( rowIndex >= 0 )
    {
        int index = SystemsGrid->Columns[0]->Index;
        IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(SystemsGrid->Rows[ rowIndex ]->Cells[index]->Value);

        PlanetsRefXYZ->Text = iDataSrc->GetFilterSystem()->PrintLocation();
        PlanetsMaxMishap->Value = 0;
        MenuTabs->SelectedIndex = 3;
    }
}

////////////////////////////////////////////////////////////////
// Planets

void Form1::PlanetsUpdateControls()
{
    // Inhibit grid update
    m_PlanetsFilter->EnableUpdates  = false;

    m_PlanetsFilter->GameData       = m_GameData;
    m_PlanetsFilter->DefaultLSN     = Math::Min(99, m_GameData->GetSpecies()->TechLevels[TECH_LS]);

    PlanetsGV->Value                = Math::Max(1, m_GameData->GetSpecies()->TechLevels[TECH_GV]);

    PlanetsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    PlanetsRefHome->DataSource      = m_RefListHomes;
    PlanetsRefColony->DataSource    = m_RefListColonies;
    PlanetsRefShip->DataSource      = m_RefListShips;

    PlanetsRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    m_PlanetsFilter->EnableUpdates  = true;
    PlanetsRefHome->Text = m_GameData->GetSpeciesName();
    m_PlanetsFilter->Reset();
}

void Form1::PlanetsSetup()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    DataColumn ^colObject   = dataTable->Columns->Add("planet",     Planet::typeid );
    DataColumn ^colName     = dataTable->Columns->Add("Name",       String::typeid );
    DataColumn ^colCoords   = dataTable->Columns->Add("Coords",     String::typeid );
    DataColumn ^colTemp     = dataTable->Columns->Add("Temp",       int::typeid );
    DataColumn ^colPress    = dataTable->Columns->Add("Press",      int::typeid );
    DataColumn ^colMD       = dataTable->Columns->Add("MD",         double::typeid );
    DataColumn ^colLSN      = dataTable->Columns->Add("LSN",        int::typeid );
    DataColumn ^colDist     = dataTable->Columns->Add("Dist.",      double::typeid );
    DataColumn ^colMishap   = dataTable->Columns->Add("Mishap %",   String::typeid );
    DataColumn ^colVisited  = dataTable->Columns->Add("Visited",    int::typeid );
    DataColumn ^colScan     = dataTable->Columns->Add("Scan",       String::typeid );
    DataColumn ^colColonies = dataTable->Columns->Add("Colonies",   String::typeid );
    DataColumn ^colNotes    = dataTable->Columns->Add("Notes",      String::typeid );

    int gv  = Decimal::ToInt32(PlanetsGV->Value);
    int age = Decimal::ToInt32(PlanetsShipAge->Value);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        double distance = system->CalcDistance(m_PlanetsFilter->RefSystem);
        double mishap = system->CalcMishap(m_PlanetsFilter->RefSystem, gv, age);

        for each( Planet ^planet in system->GetPlanets() )
        {
            if( m_PlanetsFilter->Filter(planet) )
                continue;

            DataRow^ row = dataTable->NewRow();
            row[colObject]   = planet;
            row[colName]     = planet->Name;
            row[colCoords]   = planet->PrintLocation();
            row[colTemp]     = planet->TempClass;
            row[colPress]    = planet->PressClass;
            row[colMD]       = planet->MiningDiff;
            row[colLSN]      = planet->LSN;
            row[colDist]     = distance;
            row[colMishap]   = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
            if( system->LastVisited != -1 )
                row[colVisited] = system->LastVisited;
            row[colScan]     = system->PrintScanStatus();
            row[colColonies] = system->PrintColonies( planet->Number, m_GameData->GetSpecies() );
            row[colNotes]    = planet->Comment;

            dataTable->Rows->Add(row);
        }
    }

    ApplyDataAndFormat(PlanetsGrid, dataTable, colObject, colLSN->Ordinal);

    // Some columns are not sortable... yet
    PlanetsGrid->Columns[colMishap->Ordinal]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Enable filters
    m_PlanetsFilter->EnableUpdates = true;
}

void Form1::PlanetsSelectColonies( int rowIndex )
{
    if( rowIndex != -1 )
    {
        int index = PlanetsGrid->Columns[0]->Index;
        Planet ^planet = safe_cast<Planet^>(PlanetsGrid->Rows[ rowIndex ]->Cells[index]->Value);

        if( planet->System->Colonies->Count > 0 )
        {
            ColoniesRefXYZ->Text = planet->System->PrintLocation();
            ColoniesMaxMishap->Value = 0;
            MenuTabs->SelectedIndex = 4;
        }
        else
        {
            PlanetsRefXYZ->Text = planet->System->PrintLocation();
            PlanetsMaxMishap->Value = 0;
            PlanetsMaxLSN->Value = 99;
        }
    }
}

////////////////////////////////////////////////////////////////
// Colonies

void Form1::ColoniesUpdateControls()
{
    // Inhibit grid update
    m_ColoniesFilter->EnableUpdates = false;

    m_ColoniesFilter->GameData      = m_GameData;

    ColoniesGV->Value               = Math::Max(1, m_GameData->GetSpecies()->TechLevels[TECH_GV]);

    ColoniesRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ColoniesRefHome->DataSource     = m_RefListHomes;
    ColoniesRefColony->DataSource   = m_RefListColonies;
    ColoniesRefShip->DataSource     = m_RefListShips;

    ColoniesRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    m_ColoniesFilter->EnableUpdates = true;
    ColoniesRefHome->Text = m_GameData->GetSpeciesName();
    m_ColoniesFilter->Reset();
}

void Form1::ColoniesSetup()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    DataColumn ^colObject       = dataTable->Columns->Add("colony",     Colony::typeid );
    DataColumn ^colOwner        = dataTable->Columns->Add("Owner",      String::typeid );
    DataColumn ^colName         = dataTable->Columns->Add("Name",       String::typeid );
    DataColumn ^colType         = dataTable->Columns->Add("Type",       String::typeid );
    DataColumn ^colLocation     = dataTable->Columns->Add("Location",   String::typeid );
    DataColumn ^colSize         = dataTable->Columns->Add("Size",       double::typeid );
    DataColumn ^colSeen         = dataTable->Columns->Add("Seen",       int::typeid );
    DataColumn ^colProd         = dataTable->Columns->Add("Prod.",      int::typeid );
    DataColumn ^colLSN          = dataTable->Columns->Add("LSN",        int::typeid );
    DataColumn ^colProdPerc     = dataTable->Columns->Add("Pr[%]",      int::typeid );
    DataColumn ^colBalance      = dataTable->Columns->Add("Balance",    String::typeid );
    DataColumn ^colPop          = dataTable->Columns->Add("Pop",        int::typeid );
    DataColumn ^colDist         = dataTable->Columns->Add("Dist.",      double::typeid );
    DataColumn ^colMishap       = dataTable->Columns->Add("Mishap %",   String::typeid );
    DataColumn ^colInventory    = dataTable->Columns->Add("Inventory",  String::typeid );

    int gv  = Decimal::ToInt32(ColoniesGV->Value);
    int age = Decimal::ToInt32(ColoniesShipAge->Value);
    Alien ^sp = m_GameData->GetSpecies();

    for each( Colony ^colony in m_GameData->GetColonies() )
    {
        if( m_ColoniesFilter->Filter(colony) )
            continue;

        double distance = colony->System->CalcDistance(m_ColoniesFilter->RefSystem);
        double mishap = colony->System->CalcMishap(m_ColoniesFilter->RefSystem, gv, age);

        DataRow^ row = dataTable->NewRow();
        row[colObject]      = colony;
        row[colOwner]       = colony->Owner == sp ? String::Format("* {0}", sp->Name) : colony->Owner->Name;
        row[colName]        = colony->Name;
        row[colType]        = PlTypeToString(colony->PlanetType);
        row[colLocation]    = colony->PrintLocation();
        if( colony->EconomicBase != -1 )
            row[colSize]    = colony->EconomicBase;
        row[colDist]        = distance;
        row[colMishap]      = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        row[colInventory]   = colony->PrintInventoryShort();
        if( colony->Planet )
            row[colLSN]     = colony->Planet->LSN;

        if( colony->Owner == sp )
        {
            row[colProd]    = colony->EUProd - colony->EUFleet;
            row[colProdPerc]= 100 - colony->ProdPenalty;
            row[colPop]     = colony->AvailPop;

            if( colony->PlanetType == PLANET_HOME ||
                colony->PlanetType == PLANET_COLONY )
            {
                int miTech = sp->TechLevels[TECH_MI];
                int maTech = sp->TechLevels[TECH_MA];
                if( m_ColoniesFilter->MiMaBalanced )
                    miTech = maTech = Math::Max(miTech, maTech);

                int mi = (int)((miTech * colony->MiBase) / colony->Planet->MiningDiff);
                int ma = (int)(maTech * colony->MaBase);
                if( mi == ma )
                    row[colBalance] = "Balanced";
                else if( mi < ma )
                    row[colBalance] = String::Format("+{0} MA cap.", ma - mi);
                else
                    row[colBalance] = String::Format("-{0} MA cap.", mi - ma);
            }
        }
        else
        {
            if( colony->LastSeen > 0 )
                row[colSeen] = colony->LastSeen;
        }

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(ColoniesGrid, dataTable, colObject, colOwner->Ordinal);

    // Formatting
    ColoniesGrid->Columns[colSize->Ordinal]->DefaultCellStyle->Format = "F1";

    // Some columns are not sortable... yet
    ColoniesGrid->Columns[colMishap->Ordinal]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Enable filters
    m_ColoniesFilter->EnableUpdates = true;
}

void Form1::ColoniesSetRef( int rowIndex )
{
    if( rowIndex != -1 )
    {
        int index = PlanetsGrid->Columns[0]->Index;
        Colony ^colony = safe_cast<Colony^>(ColoniesGrid->Rows[ rowIndex ]->Cells[index]->Value);
        ColoniesRefColony->Text = colony->PrintRefListEntry( m_GameData->GetSpecies() );
    }
}

////////////////////////////////////////////////////////////////
// Ships

void Form1::ShipsUpdateControls()
{
    // Inhibit grid update
    m_ShipsFilter->EnableUpdates = false;

    m_ShipsFilter->GameData      = m_GameData;

    ShipsGV->Value               = Math::Max(1, m_GameData->GetSpecies()->TechLevels[TECH_GV]);

    ShipsRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ShipsRefHome->DataSource     = m_RefListHomes;
    ShipsRefColony->DataSource   = m_RefListColonies;
    ShipsRefShip->DataSource     = m_RefListShips;

    ShipsRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    m_ShipsFilter->EnableUpdates = true;
    ShipsRefHome->Text = m_GameData->GetSpeciesName();
    m_ShipsFilter->Reset();
}

void Form1::ShipsSetup()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    DataColumn ^colObject   = dataTable->Columns->Add("ship",       Ship::typeid );
    DataColumn ^colOwner    = dataTable->Columns->Add("Owner",      String::typeid );
    DataColumn ^colClass    = dataTable->Columns->Add("Class",      String::typeid );
    DataColumn ^colName     = dataTable->Columns->Add("Name",       String::typeid );
    DataColumn ^colLocation = dataTable->Columns->Add("Location",   String::typeid );
    DataColumn ^colAge      = dataTable->Columns->Add("Age",        int::typeid );
    DataColumn ^colCap      = dataTable->Columns->Add("Cap.",       int::typeid );
    DataColumn ^colCargo    = dataTable->Columns->Add("Cargo",      String::typeid );
    DataColumn ^colDist     = dataTable->Columns->Add("Dist.",      double::typeid );
    DataColumn ^colMishap   = dataTable->Columns->Add("Mishap %",   String::typeid );
    DataColumn ^colMaint    = dataTable->Columns->Add("Maint",      double::typeid );
    DataColumn ^colUpgCost  = dataTable->Columns->Add("Upg.Cost",   int::typeid );
    DataColumn ^colRecVal   = dataTable->Columns->Add("Rec.Val",    int::typeid );

    Alien ^sp = m_GameData->GetSpecies();
    int gv  = Decimal::ToInt32(PlanetsGV->Value);
    int ml = sp->TechLevels[TECH_ML];
    double discount = (100.0 - (ml / 2)) / 100.0;

    for each( Ship ^ship in m_GameData->GetShips() )
    {
        if( m_ShipsFilter->Filter(ship) )
            continue;

        double distance = StarSystem::CalcDistance(
            ship->X,
            ship->Y,
            ship->Z,
            m_ShipsFilter->RefSystem->X,
            m_ShipsFilter->RefSystem->Y,
            m_ShipsFilter->RefSystem->Z);
        double mishap = StarSystem::CalcMishap(
            ship->X,
            ship->Y,
            ship->Z,
            m_ShipsFilter->RefSystem->X,
            m_ShipsFilter->RefSystem->Y,
            m_ShipsFilter->RefSystem->Z,
            gv,
            ship->Age);

        DataRow^ row = dataTable->NewRow();
        row[colObject]      = ship;
        row[colOwner]       = ship->Owner == sp ? String::Format("* {0}", sp->Name) : ship->Owner->Name;
        row[colClass]       = ship->PrintClass();
        row[colName]        = ship->Name;
        row[colLocation]    = ship->PrintLocation( m_GameData->GetSpecies() );
        if( !ship->IsPirate )
            row[colAge]     = ship->Age;
        row[colCap]         = ship->Capacity;
        row[colDist]        = distance;
        row[colMishap]      = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
        if( sp == ship->Owner )
        {
            row[colCargo]   = ship->PrintCargo();
            row[colMaint]   = ship->GetMaintenanceCost() * discount;
            row[colUpgCost] = ship->GetUpgradeCost();
            row[colRecVal]  = ship->GetRecycleValue();
        }
        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(ShipsGrid, dataTable, colObject, colOwner->Ordinal);

    // Some columns are not sortable... yet
    ShipsGrid->Columns[colMishap->Ordinal]->SortMode = DataGridViewColumnSortMode::NotSortable;

    // Enable filters
    m_ShipsFilter->EnableUpdates = true;
}

void Form1::ShipsSetRef( int rowIndex )
{
    if( rowIndex != -1 )
    {
        int index = ShipsGrid->Columns[0]->Index;
        Ship ^ship = safe_cast<Ship^>(ShipsGrid->Rows[ rowIndex ]->Cells[index]->Value);
        ShipsRefShip->Text = ship->PrintRefListEntry();
    }
}

////////////////////////////////////////////////////////////////
// Aliens

void Form1::AliensUpdateControls()
{
    m_AliensFilter->EnableUpdates = true;
    m_AliensFilter->Reset();
}

void Form1::AliensSetup()
{
    // Put system data in a DataTable so that column sorting works.
    DataTable ^dataTable = gcnew DataTable();

    DataColumn ^colObject   = dataTable->Columns->Add("alien",          Alien::typeid );
    DataColumn ^colName     = dataTable->Columns->Add("Name",           String::typeid );
    DataColumn ^colRelation = dataTable->Columns->Add("Relation",       String::typeid );
    DataColumn ^colHome     = dataTable->Columns->Add("Home",           String::typeid );
    DataColumn ^colTechLev  = dataTable->Columns->Add("Tech Levels",    String::typeid );
    DataColumn ^colTemp     = dataTable->Columns->Add("Temp",           int::typeid );
    DataColumn ^colPress    = dataTable->Columns->Add("Press",          int::typeid );
    DataColumn ^colEMail    = dataTable->Columns->Add("EMail",          String::typeid );

    for each( Alien ^alien in m_GameData->GetAliens() )
    {
        if( m_AliensFilter->Filter(alien) )
            continue;

        DataRow^ row = dataTable->NewRow();
        row[colObject]      = alien;
        row[colName]        = alien->Name;
        row[colRelation]    = alien->PrintRelation();
        row[colHome]        = alien->PrintHome();
        row[colTechLev]     = alien->PrintTechLevels();
        if( alien->AtmReq->TempClass != -1 &&
            alien->AtmReq->PressClass != -1 )
        {
            row[colTemp]    = alien->AtmReq->TempClass;
            row[colPress]   = alien->AtmReq->PressClass;
        }
        row[colEMail]       = alien->Email;
        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(AliensGrid, dataTable, colObject, colRelation->Ordinal);

    // Enable filters
    m_AliensFilter->EnableUpdates = true;
}

////////////////////////////////////////////////////////////////

}
