#include "StdAfx.h"
#include "Form1.h"
#include "BuildInfo.h"

#include "Report.h"
#include "GridFilter.h"

using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace System::Reflection;

#define OPTIMAL_ROW_HEIGHT 18

////////////////////////////////////////////////////////////////

namespace FHUI
{

void Form1::LoadGameData()
{
    try
    {
        InitializeComponent();

        FillAboutBox();
        InitData();
        InitControls();
        LoadPlugins();
        ScanReports();
        GenerateTemplate();
        LoadCommands(); // TBD: Do czego to ma sluzyc?
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

    m_GridToolTip = gcnew ToolTip;

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

    m_OrderList = gcnew List<String^>;
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

void Form1::UpdatePlugins()
{
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->SetGameData(m_GameData);
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
        BuildInfo::ContactEmails,
        BuildInfo::Version,
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
        StreamReader ^sr = File::OpenText(Application::StartupPath + "/changelog.txt");
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
            BuildInfo::Version,
            BuildInfo::ContactEmails,
            BuildInfo::ContactWiki,
            changeLog );
}

void Form1::LoadGalaxy()
{
    String^ galaxyList = "galaxy_list.txt";
    if( !String::IsNullOrEmpty(DataDir) )
        galaxyList = DataDir + "/" + galaxyList;

    StreamReader ^sr = File::OpenText(galaxyList);
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
    String^ reportsDir = "reports";
    if( !String::IsNullOrEmpty(DataDir) )
        reportsDir = DataDir + "/" + reportsDir;

    DirectoryInfo ^dir = gcnew DirectoryInfo(reportsDir);

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
        UpdatePlugins();
        UpdateControls();

        // Display summary
        Summary->Text = m_GameData->GetSummary();

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            m_GameData->GetSpeciesName(),
            m_GameData->GetLastTurn(),
            BuildInfo::Version );

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
        String^ ordersDir = "orders";
        if( !String::IsNullOrEmpty(DataDir) )
            ordersDir = DataDir + "/" + ordersDir;

        DirectoryInfo ^dir = gcnew DirectoryInfo(ordersDir);

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
// Plugins

void Form1::LoadPlugins()
{
    m_GridPlugins = gcnew List<IGridPlugin^>;
    m_OrdersPlugins = gcnew List<IOrdersPlugin^>;

    DirectoryInfo ^dir = gcnew DirectoryInfo(Application::StartupPath);

    for each( FileInfo ^f in dir->GetFiles("fhui.*.dll"))
    {
        if( f->Name->ToLower() == "fhui.datalib.dll" )
            continue;

        try
        {
            Assembly ^assembly = Assembly::LoadFrom(f->FullName);

            // Walk through each type in the assembly
            for each( Type ^type in assembly->GetTypes() )
            {
                if( type->IsClass && type->IsPublic )
                {
                    if( type->GetInterface("FHUI.IPluginBase") )
                    {
                        IPluginBase^ plugin = safe_cast<IPluginBase^>(Activator::CreateInstance(type));
                        
                        IGridPlugin^ gridPlugin = dynamic_cast<IGridPlugin^>(plugin);
                        if( gridPlugin )
                            m_GridPlugins->Add(gridPlugin);

                        IOrdersPlugin^ ordersPlugin = dynamic_cast<IOrdersPlugin^>(plugin);
                        if( ordersPlugin )
                            m_OrdersPlugins->Add(ordersPlugin);
                    }
                }
            }
        }
        catch( Exception ^ex )
        {
            throw gcnew FHUIPluginException(
                String::Format("Error occured while loading plugin: {0}\r\n Error message: {1}",
                    f->Name, ex->Message),
                ex );
        }
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

void Form1::ShowGridContextMenu(DataGridView^ grid, DataGridViewCellMouseEventArgs ^e)
{
    if( e->RowIndex < 0 || e->ColumnIndex < 0 )
        return;

    // Create menu
    Windows::Forms::ContextMenuStrip ^menu = gcnew Windows::Forms::ContextMenuStrip;

    // Populate menu
    if( grid == SystemsGrid )
        SystemsFillMenu(menu, e->RowIndex);
    else if( grid == PlanetsGrid )
        PlanetsFillMenu(menu, e->RowIndex);
    else if( grid == ColoniesGrid )
        ColoniesFillMenu(menu, e->RowIndex);
    else if( grid == ShipsGrid )
        ShipsFillMenu(menu, e->RowIndex);
    else if( grid == AliensGrid )
        AliensFillMenu(menu, e->RowIndex);

    // Show menu
    Rectangle r = grid->GetCellDisplayRectangle(e->ColumnIndex, e->RowIndex, false);
    menu->Show(grid, r.Left + e->Location.X, r.Top + e->Location.Y);
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

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Systems, dataTable);

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

        //SystemsGrid->Columns[colColonies->Ordinal]->DefaultCellStyle->WrapMode = DataGridViewTriState::True;

        for each( IGridPlugin ^plugin in m_GridPlugins )
            plugin->AddRowData(row, system, m_SystemsFilter);

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(SystemsGrid, dataTable, colObject, colDist->Ordinal);
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->GridFormat(GridType::Systems, SystemsGrid);

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

void Form1::SystemsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = SystemsGrid->Columns[0]->Index;
    StarSystem ^system = safe_cast<StarSystem^>(SystemsGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_SystemsMenuRef    = system;
    m_SystemsMenuRefRow = rowIndex;

    if( system->PlanetsCount > 0 )
    {
        menu->Items->Add(
            "Show Planets in " + system->PrintLocation(),
            nullptr,
            gcnew EventHandler(this, &Form1::SystemsMenuShowPlanets) );
    }

    if( system->Colonies->Count > 0 )
    {
        menu->Items->Add(
            "Show Colonies in " + system->PrintLocation(),
            nullptr,
            gcnew EventHandler(this, &Form1::SystemsMenuShowColonies) );
    }

    menu->Items->Add(
        "Select ref: " + system->PrintLocation(),
        nullptr,
        gcnew EventHandler(this, &Form1::SystemsMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::SystemsFiltersReset_Click));
}

void Form1::SystemsMenuShowPlanets(Object^, EventArgs^)
{
    SystemsSelectPlanets(m_SystemsMenuRefRow);
}

void Form1::SystemsMenuShowColonies(Object^, EventArgs^)
{
    ColoniesRefXYZ->Text = m_SystemsMenuRef->PrintLocation();
    ColoniesMaxMishap->Value = 0;
    MenuTabs->SelectedIndex = 4;
}

void Form1::SystemsMenuSelectRef(Object^, EventArgs^)
{
    m_SystemsFilter->SetRefSystem(m_SystemsMenuRef);
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

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Planets, dataTable);

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
            row[colMD]       = (double)planet->MiDiff / 100;
            row[colLSN]      = planet->LSN;
            row[colDist]     = distance;
            row[colMishap]   = String::Format("{0:F2} / {1:F2}", mishap, mishap * mishap / 100.0);
            if( system->LastVisited != -1 )
                row[colVisited] = system->LastVisited;
            row[colScan]     = system->PrintScanStatus();
            row[colColonies] = system->PrintColonies( planet->Number, m_GameData->GetSpecies() );

            if( (planet->NumColonies == 0) &&
                (planet->System->HomeSpecies != nullptr) )
            {
                // A home planet exists in the system. Make a note about that
                String ^note = "";
                if ( planet->Comment && planet->Comment->Length )
                {
                    note = planet->Comment + "; ";
                }
                note += String::Format(" SP {0} home system", planet->System->HomeSpecies->Name );
                row[colNotes] = note;
            }
            else
            {
                row[colNotes]    = planet->Comment;
            }

            for each( IGridPlugin ^plugin in m_GridPlugins )
                plugin->AddRowData(row, planet, m_SystemsFilter);

            dataTable->Rows->Add(row);
        }
    }

    ApplyDataAndFormat(PlanetsGrid, dataTable, colObject, colLSN->Ordinal);
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->GridFormat(GridType::Planets, SystemsGrid);

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

void Form1::PlanetsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = PlanetsGrid->Columns[0]->Index;
    Planet ^planet = safe_cast<Planet^>(PlanetsGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_PlanetsMenuRef    = planet;
    m_PlanetsMenuRefRow = rowIndex;

    String^ name = String::IsNullOrEmpty(planet->Name)
        ? planet->PrintLocation()
        : "PL " + planet->Name;

    if( planet->System->Colonies->Count > 0 )
    {
        menu->Items->Add(
            "Show Colonies near " + name,
            nullptr,
            gcnew EventHandler(this, &Form1::PlanetsMenuShowColonies) );
    }

    menu->Items->Add(
        "Select ref: " + name,
        nullptr,
        gcnew EventHandler(this, &Form1::PlanetsMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::PlanetsFiltersReset_Click));
}

void Form1::PlanetsMenuShowColonies(Object^, EventArgs^)
{
    PlanetsSelectColonies(m_PlanetsMenuRefRow);
}

void Form1::PlanetsMenuSelectRef(Object^, EventArgs^)
{
    m_PlanetsFilter->SetRefSystem(m_PlanetsMenuRef->System);
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

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Colonies, dataTable);

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
        row[colType]        = FHStrings::PlTypeToString(colony->PlanetType);
        row[colLocation]    = colony->PrintLocation();
        if( colony->EconomicBase != -1 )
            row[colSize]    = (double)colony->EconomicBase / 10;
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
                colony->CalculateBalance(m_ColoniesFilter->MiMaBalanced);
                row[colBalance] = colony->PrintBalance();
            }
        }
        else
        {
            if( colony->LastSeen > 0 )
                row[colSeen] = colony->LastSeen;
        }

        for each( IGridPlugin ^plugin in m_GridPlugins )
            plugin->AddRowData(row, colony, m_SystemsFilter);

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(ColoniesGrid, dataTable, colObject, colOwner->Ordinal);
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->GridFormat(GridType::Colonies, SystemsGrid);

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

void Form1::ColoniesFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = ColoniesGrid->Columns[0]->Index;
    m_ColoniesMenuRef = safe_cast<Colony^>(ColoniesGrid->Rows[ rowIndex ]->Cells[index]->Value);

    menu->Items->Add(
        "Select ref: PL " + m_ColoniesMenuRef->PrintRefListEntry(m_GameData->GetSpecies()),
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesFiltersReset_Click));
}

void Form1::ColoniesMenuSelectRef(Object^, EventArgs ^e)
{
    ColoniesRefColony->Text = m_ColoniesMenuRef->PrintRefListEntry(m_GameData->GetSpecies());
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

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Ships, dataTable);

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

        for each( IGridPlugin ^plugin in m_GridPlugins )
            plugin->AddRowData(row, ship, m_SystemsFilter);

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(ShipsGrid, dataTable, colObject, colOwner->Ordinal);
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->GridFormat(GridType::Ships, SystemsGrid);

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
        if( ship->Owner == m_GameData->GetSpecies() )
            ShipsRefShip->Text = ship->PrintRefListEntry();
        else
            m_ShipsFilter->SetRefSystem(ship->System);
    }
}

void Form1::ShipsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = ShipsGrid->Columns[0]->Index;
    m_ShipsMenuRef = safe_cast<Ship^>(ShipsGrid->Rows[ rowIndex ]->Cells[index]->Value);

    menu->Items->Add(
        String::Format("Select ref: {0} {1} (@ {2})",
            m_ShipsMenuRef->PrintClass(),
            m_ShipsMenuRef->Name,
            m_ShipsMenuRef->System->PrintLocation() ),
        nullptr,
        gcnew EventHandler(this, &Form1::ShipsMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::ShipsFiltersReset_Click));
}

void Form1::ShipsMenuSelectRef(Object^, EventArgs ^e)
{
    if( m_ShipsMenuRef->Owner == m_GameData->GetSpecies() )
        ShipsRefShip->Text = m_ShipsMenuRef->PrintRefListEntry();
    else
        m_ShipsFilter->SetRefSystem(m_ShipsMenuRef->System);
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

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Aliens, dataTable);

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

        for each( IGridPlugin ^plugin in m_GridPlugins )
            plugin->AddRowData(row, alien, m_SystemsFilter);

        dataTable->Rows->Add(row);
    }

    ApplyDataAndFormat(AliensGrid, dataTable, colObject, colRelation->Ordinal);
    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->GridFormat(GridType::Aliens, SystemsGrid);

    // Enable filters
    m_AliensFilter->EnableUpdates = true;
}

void Form1::AliensFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = AliensGrid->Columns[0]->Index;
    IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(AliensGrid->Rows[ rowIndex ]->Cells[index]->Value);

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::AliensFiltersReset_Click));
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
