#include "StdAfx.h"
#include "Form1.h"
#include "BuildInfo.h"

#include "GridFilter.h"
#include "GridSorter.h"

#include "ReportParser.h"
#include "PluginManager.h"
#include "CommandManager.h"

using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace System::Reflection;

#define OPTIMAL_ROW_HEIGHT 18

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

generic <typename T>
ref class ToolStripMenuItemCustom : public ToolStripMenuItem
{
public:
    ToolStripMenuItemCustom(String ^s, T data)
        : ToolStripMenuItem(s)
        , m_Data(data)
    {}

    event EventHandler1Arg<T>^ CustomClick;
    virtual void OnClick(EventArgs^ e) override
    {
        CustomClick(m_Data);
        ToolStripMenuItem::OnClick(e);
    }

protected:
    T       m_Data;
};

////////////////////////////////////////////////////////////////

void Form1::Initialize()
{
    try
    {
        InitializeComponent();
        InitData();
        InitControls();
        LoadOrders();
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed to initialize application.";
        ShowException(e);
    }
}

void Form1::InitControls()
{
    System::Text::RegularExpressions::Regex::CacheSize = 500;

    m_GridToolTip = gcnew ToolTip;

    TextAbout->Text = GetAboutText();

    SystemsInitControls();
    PlanetsInitControls();
    ColoniesInitControls();
    ShipsInitControls();
    AliensInitControls();

    if( m_ReportParser->Reports->Count > 0 )
    {
        // Setup combo box with list of loaded reports
        int n0 = m_ReportParser->Reports->Count;
        int n1 = n0 - (m_ReportParser->Reports->ContainsKey(0) ? 1 : 0);
        array<String^> ^arrN0 = gcnew array<String^>(n0);
        array<String^> ^arrN1 = gcnew array<String^>(n1);
        for each( int key in m_ReportParser->Reports->Keys )
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

void Form1::InitData()
{
    m_bHadException = false;
    m_bGridUpdateEnabled = gcnew bool(false);

    m_OrderFiles = gcnew SortedList<String^, String^>;
    m_RepTurnNrData = nullptr;

    m_GameData      = gcnew GameData;
    m_PluginMgr     = gcnew PluginManager(m_GameData, Application::StartupPath);
    m_CommandMgr    = gcnew CommandManager(m_GameData, GetDataDir(OrdersDir::Folder));
    m_ReportParser  = gcnew ReportParser(m_GameData, m_CommandMgr, GetDataDir("galaxy_list.txt"), GetDataDir("reports"));

    if (EnablePlugins)
    {
        m_PluginMgr->LoadPlugins();
    }
    m_ReportParser->ScanReports();
    m_CommandMgr->LoadCommands();
}

void Form1::InitRefLists()
{
    m_RefListSystemsXYZ = gcnew List<String^>;
    m_RefListHomes      = gcnew List<String^>;
    m_RefListColonies   = gcnew List<String^>;
    m_RefListShips      = gcnew List<String^>;

    Alien ^sp = GameData::Player;

    // -- ref systems xyz:
    m_RefListSystemsXYZ->Add( GridFilter::s_CaptionXYZ );
    for each( StarSystem ^system in m_GameData->GetStarSystems() )
        m_RefListSystemsXYZ->Add( system->PrintLocationAligned() );

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
        m_RefListColonies->Add( colony->PrintRefListEntry() );
    // then alien
    for each( Colony ^colony in m_GameData->GetColonies() )
        if( colony->Owner != sp )
            m_RefListColonies->Add( colony->PrintRefListEntry() );

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

    TechLevelsResetToCurrent();

    SystemsUpdateControls();
    PlanetsUpdateControls();
    ColoniesUpdateControls();
    ShipsUpdateControls();
    AliensUpdateControls();

    UtilUpdateAll();
}

void Form1::UpdateTabs()
{
    switch( MenuTabs->SelectedIndex )
    {
    case TabIndex::Map:
        {
            System::Windows::Forms::DialogResult result = MessageBox::Show(
                this,
                "MAP is under construction and is not functional yet...",
                "Map tab alert",
                MessageBoxButtons::OKCancel,
                MessageBoxIcon::Question,
                MessageBoxDefaultButton::Button2);
            if( result == System::Windows::Forms::DialogResult::OK )
            {
                MapDraw();
            }
            else
            {
                MenuTabs->SelectedTab = MenuTabs->TabPages[0];
            }
        }
        break;

    case TabIndex::Orders:
        m_CommandMgr->GenerateTemplate( OrderTemplate );
        break;

    case TabIndex::Utils:
        UtilTabSelected();
        break;
    }
}

void Form1::TechLevelsResetToCurrent()
{
    Alien ^sp = GameData::Player;

    // Inhibit grid update
    *m_bGridUpdateEnabled = false;

    TechMI->Minimum = Math::Max(1, sp->TechLevels[TECH_MI]);
    TechMA->Minimum = Math::Max(1, sp->TechLevels[TECH_MA]);
    TechML->Minimum = Math::Max(1, sp->TechLevels[TECH_ML]);
    TechGV->Minimum = Math::Max(1, sp->TechLevels[TECH_GV]);
    TechLS->Minimum = Math::Max(1, sp->TechLevels[TECH_LS]);
    TechBI->Minimum = Math::Max(1, sp->TechLevels[TECH_BI]);

    TechMI->Value = sp->TechLevelsAssumed[TECH_MI] = Math::Max(1, sp->TechLevels[TECH_MI]);
    TechMA->Value = sp->TechLevelsAssumed[TECH_MA] = Math::Max(1, sp->TechLevels[TECH_MA]);
    TechML->Value = sp->TechLevelsAssumed[TECH_ML] = Math::Max(1, sp->TechLevels[TECH_ML]);
    TechGV->Value = sp->TechLevelsAssumed[TECH_GV] = Math::Max(1, sp->TechLevels[TECH_GV]);
    TechLS->Value = sp->TechLevelsAssumed[TECH_LS] = Math::Max(1, sp->TechLevels[TECH_LS]);
    TechBI->Value = sp->TechLevelsAssumed[TECH_BI] = Math::Max(1, sp->TechLevels[TECH_BI]);

    *m_bGridUpdateEnabled = true;
}

void Form1::TechLevelsResetToTaught()
{
    Alien ^sp = GameData::Player;

    // Inhibit grid update
    *m_bGridUpdateEnabled = false;

    TechMI->Value = sp->TechLevelsAssumed[TECH_MI] = Math::Max(1, sp->TechLevelsTeach[TECH_MI]);
    TechMA->Value = sp->TechLevelsAssumed[TECH_MA] = Math::Max(1, sp->TechLevelsTeach[TECH_MA]);
    TechML->Value = sp->TechLevelsAssumed[TECH_ML] = Math::Max(1, sp->TechLevelsTeach[TECH_ML]);
    TechGV->Value = sp->TechLevelsAssumed[TECH_GV] = Math::Max(1, sp->TechLevelsTeach[TECH_GV]);
    TechLS->Value = sp->TechLevelsAssumed[TECH_LS] = Math::Max(1, sp->TechLevelsTeach[TECH_LS]);
    TechBI->Value = sp->TechLevelsAssumed[TECH_BI] = Math::Max(1, sp->TechLevelsTeach[TECH_BI]);

    *m_bGridUpdateEnabled = true;
}

void Form1::TechLevelsChanged()
{
    Alien ^sp = GameData::Player;

    sp->TechLevelsAssumed[TECH_MI] = Decimal::ToInt32(TechMI->Value);
    sp->TechLevelsAssumed[TECH_MA] = Decimal::ToInt32(TechMA->Value);
    sp->TechLevelsAssumed[TECH_ML] = Decimal::ToInt32(TechML->Value);
    sp->TechLevelsAssumed[TECH_GV] = Decimal::ToInt32(TechGV->Value);
    sp->TechLevelsAssumed[TECH_LS] = Decimal::ToInt32(TechLS->Value);
    sp->TechLevelsAssumed[TECH_BI] = Decimal::ToInt32(TechBI->Value);

    if( *m_bGridUpdateEnabled )
    {
        SystemsGrid->Filter->Update();
        PlanetsGrid->Filter->Update();
        ColoniesGrid->Filter->Update();
        ShipsGrid->Filter->Update();

        if( MenuTabs->SelectedIndex == TabIndex::Map )
            MapDraw();
        if( MenuTabs->SelectedIndex == TabIndex::Orders )
            m_CommandMgr->GenerateTemplate( OrderTemplate );
    }
}

void Form1::ShowException(Exception ^e)
{
    m_bHadException = true;

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
}

String^ Form1::GetAboutText()
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

    return String::Format(
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

String^ Form1::GetDataDir(String ^suffix)
{
    String ^ret = suffix;
    if( !String::IsNullOrEmpty(DataDir) )
        ret = DataDir + "/" + ret;
    return ret;
}

void Form1::TurnReload()
{
    throw gcnew FHUIDataImplException();
    System::Windows::Forms::DialogResult result = MessageBox::Show(
        this,
        "Delete ALL FHUI Commands?",
        "Reload Turn",
        MessageBoxButtons::YesNo,
        MessageBoxIcon::Question,
        MessageBoxDefaultButton::Button1);
    if( result == System::Windows::Forms::DialogResult::Yes )
    {
        m_CommandMgr->DeleteCommands();
        DisplayTurn();
    }
}

void Form1::DisplayTurn()
{
    try
    {
        String ^sel = TurnSelect->Text;
        int turn = int::Parse(sel->Substring(16));    // Skip 'Status for Turn '

        *m_bGridUpdateEnabled = false;
        if ( m_GameData->SelectTurn(turn) == false)
        {
            throw gcnew FHUIDataIntegrityException(String::Format("No information about selected turn #{0}.", turn));
        }
        RepModeChanged();
        m_PluginMgr->UpdatePlugins();
        UpdateControls();

        m_CommandMgr->SelectTurn(turn);
        //TODO: Set it properly. What should it be ?
        //m_ColoniesMenuRef = 

        // Display summary
        Summary->Text = m_GameData->GetSummary();

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            GameData::Player->Name,
            GameData::CurrentTurn,
            BuildInfo::Version );

        MapSetup();
        UpdateTabs();
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed loading game data.";
        ShowException(e);
    }
}

void Form1::RepModeChanged()
{
    if( m_bHadException )
        return;

    if( RepModeReports->Checked )
    {
        RepTurnNr->DataSource = m_RepTurnNrData;
    }
    else if( RepModeCommands->Checked )
    {
        List<String^> ^items = gcnew List<String^>;
        for each( String ^s in m_OrderFiles->Keys )
        {
            items->Add(s);
        }
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

        RepText->Text = m_ReportParser->Reports[key];
    }
    else if( RepModeCommands->Checked )
    {
        RepText->Text = m_OrderFiles[ RepTurnNr->SelectedItem->ToString() ];
    }
}

void Form1::LoadOrders()
{
    m_OrderFiles->Clear();

    try
    {
        String^ ordersDir = GetDataDir("orders");
        DirectoryInfo ^dir = gcnew DirectoryInfo(ordersDir);

        for each( FileInfo ^f in dir->GetFiles("*"))
        {
            if( f->Length <= 0x10000 ) // 64 KB, more than enough for any commands
            {
                m_OrderFiles[f->Name] = File::OpenText(f->FullName)->ReadToEnd();
            }
            else
            {
                m_OrderFiles[f->Name] = "File too large (limit is 64KB).";
            }
        }
    }
    catch( DirectoryNotFoundException^ )
    {
    }
    finally
    {
        RepModeCommands->Enabled = m_OrderFiles->Count > 0;
    }
}

////////////////////////////////////////////////////////////////
// GUI misc

void Form1::ApplyDataAndFormat(
    DataGridView ^grid,
    DataTable ^data,
    int objColumnIndex,
    int defaultSortColIdx,
    IGridSorter ^sorter )
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
    if( data )
        grid->DataSource = data;

    // Make object link column invisible
    grid->Columns[objColumnIndex]->Visible = false;

    // Adjust row height
    grid->RowTemplate->Height = OPTIMAL_ROW_HEIGHT;

    // Formatting
    for each( DataGridViewColumn ^col in grid->Columns )
    {
        Type ^type = col->ValueType;
        if( type == nullptr )
            type = col->CellTemplate->ValueType;

        if( type == double::typeid )
            col->DefaultCellStyle->Format = "F2";
        if( type == double::typeid || type == int::typeid )
            col->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleRight;
    }

    // Sort grid
    if( sortBy != -1 )
    {
        if( sorter )
        {
            sorter->SetSortColumn( sortBy );
        }
        else
            grid->Sort( grid->Columns[sortBy], sortDir );
    }

    grid->ClearSelection();
}

void Form1::ColumnsFilterMenu(DataGridView ^grid, DataGridViewCellMouseEventArgs ^e)
{
    // Create menu
    Windows::Forms::ContextMenuStrip ^menu = gcnew Windows::Forms::ContextMenuStrip;

    EventHandler1Arg<ColumnsFilterData^> ^handler =
        gcnew EventHandler1Arg<ColumnsFilterData^>(this, &Form1::ColoniesFilterOnOff);

    for( int i = 0; i < grid->Columns->Count; ++i )
    {
        String ^text = (String^)grid->Columns[i]->HeaderCell->Value;
        if( !String::IsNullOrEmpty(text) )
        {
            ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
                text,
                gcnew ColumnsFilterData(grid, i),
                handler );
            menuItem->Checked = grid->Columns[i]->Visible;
            menu->Items->Add( menuItem );
        }
    }

    // Show menu
    Rectangle r = grid->GetCellDisplayRectangle(e->ColumnIndex, e->RowIndex, false);
    menu->Show(grid, r.Left + e->Location.X, r.Top + e->Location.Y);
}

void Form1::ColoniesFilterOnOff(ColumnsFilterData ^data)
{
    data->A->Columns[data->B]->Visible = !data->A->Columns[data->B]->Visible;
}

Color Form1::GetAlienColor(Alien ^sp)
{
    if( sp == nullptr )
        return Color::White;

    if( sp == GameData::Player )
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

void Form1::SetGridRefSystemOnMouseClick(DblBufDGV ^grid, int rowIndex)
{
    if( rowIndex >= 0 )
    {
        int index = grid->Columns[0]->Index;
        IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(grid->Rows[ rowIndex ]->Cells[index]->Value);

        StarSystem ^system = iDataSrc->GetFilterSystem();
        if( system )
        {
            IGridFilter ^filter = grid->Filter;
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

generic<typename T>
ToolStripMenuItem^ Form1::CreateCustomMenuItem(
    String ^text,
    T data,
    EventHandler1Arg<T> ^handler )
{
    ToolStripMenuItemCustom<T> ^item = gcnew ToolStripMenuItemCustom<T>(text, data);
    item->CustomClick += handler;
    return item;
}

String^ Form1::GridPrintDistance(StarSystem ^from, StarSystem ^to, int gv, int age)
{
    if( from->HasWormhole &&
        from->WormholeTarget == to )
    {
        return "Wormhole";
    }

    double distance = from->CalcDistance(to);
    double mishap = from->CalcMishap(to, gv, age);

    return String::Format("{0:F1}  ({1:F1}%)", distance, mishap);
}

////////////////////////////////////////////////////////////////
// Systems

void Form1::SystemsInitControls()
{
    SystemsGridSorter ^sorter = gcnew SystemsGridSorter(SystemsGrid);

    // Add columns
    SystemsColumns %c = m_SystemsColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object   = ADD_COLUMN(nullptr,        nullptr,                StarSystem, None,       Default);
    c.X        = ADD_COLUMN("X",            "System X coordinate",  int,        Ascending,  Default);
    c.Y        = ADD_COLUMN("Y",            "System Y coordinate",  int,        Ascending,  Default);
    c.Z        = ADD_COLUMN("Z",            "System Z coordinate",  int,        Ascending,  Default);
    c.Type     = ADD_COLUMN("Type",         "Star type",            String,     Ascending,  Default);
    c.Planets  = ADD_COLUMN("PL",           "Number of planets",    int,        Descending, Default);
    c.LSN      = ADD_COLUMN("Min LSN",      "Minimum LSN",          int,        Ascending,  Default);
    c.LSNAvail = ADD_COLUMN("Free LSN",     "Minimum free LSN",     int,        Ascending,  Default);
    c.Dist     = ADD_COLUMN("Distance",     "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.Visited  = ADD_COLUMN("Vis",          "Last turn you visited this system", int, Descending, Default);
    c.Scan     = ADD_COLUMN("Scan",         "System scan source",   String,     Ascending,  Default);
    // WORMHOLE TODO
    //c.Wormhole = ADD_COLUMN("WH",           "Wormhole target",      String,     Ascending,  Default);
    c.Colonies = ADD_COLUMN("Colonies",     "Summary of colonies and named planets", String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Systems, sorter);

    c.Notes    = ADD_COLUMN("Notes", "Notes", String, Ascending,  Default);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(SystemsGrid, nullptr, c.Object, c.LSNAvail, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Systems, SystemsGrid);

    SystemsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    SystemsGrid->Columns[c.Colonies]->DefaultCellStyle->Font =
        gcnew System::Drawing::Font(L"Tahoma", 6.75F);

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(SystemsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::SystemsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlRef         = SystemsRef;
    filter->CtrlRefHome     = SystemsRefHome;
    filter->CtrlRefEdit     = SystemsRefEdit;
    filter->CtrlRefXYZ      = SystemsRefXYZ;
    filter->CtrlRefColony   = SystemsRefColony;
    filter->CtrlRefShip     = SystemsRefShip;
    filter->CtrlGV          = TechGV;
    filter->CtrlShipAge     = SystemsShipAge;
    filter->CtrlMaxMishap   = SystemsMaxMishap;
    filter->CtrlMaxLSN      = SystemsMaxLSN;
    filter->CtrlFiltVisV    = SystemsFiltVisV;
    filter->CtrlFiltVisN    = SystemsFiltVisN;
    filter->CtrlFiltColC    = SystemsFiltColC;
    filter->CtrlFiltColN    = SystemsFiltColN;
    filter->CtrlNumRows     = SystemsNumRows;

    // Store objects
    SystemsGrid->Filter = filter;
    SystemsGrid->Sorter = sorter;
}

void Form1::SystemsUpdateControls()
{
    // Inhibit grid update
    SystemsGrid->Filter->EnableUpdates  = false;

    SystemsGrid->Filter->GameData       = m_GameData;

    SystemsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    SystemsRefHome->DataSource      = m_RefListHomes;
    SystemsRefColony->DataSource    = m_RefListColonies;
    SystemsRefShip->DataSource      = m_RefListShips;

    // Trigger grid update
    SystemsGrid->Filter->EnableUpdates  = true;
    SystemsRefHome->Text            = GameData::Player->Name;
    SystemsGrid->Filter->Reset();
}

void Form1::SystemsSetup()
{
    SystemsGrid->FullUpdateBegin();

    SystemsColumns %c = m_SystemsColumns;

    int gv  = Decimal::ToInt32(TechGV->Value);
    int age = Decimal::ToInt32(SystemsShipAge->Value);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        if( SystemsGrid->Filter->Filter(system) )
            continue;

        DataGridViewRow ^row = SystemsGrid->Rows[ SystemsGrid->Rows->Add() ];
        DataGridViewCellCollection ^cells = row->Cells;
        cells[c.Object]->Value      = system;
        cells[c.X]->Value           = system->X;
        cells[c.Y]->Value           = system->Y;
        cells[c.Z]->Value           = system->Z;
        cells[c.Type]->Value        = system->Type;
        if( system->IsExplored() )
        {
            cells[c.Planets]->Value = system->Planets->Count;
            cells[c.LSN]->Value     = system->MinLSN;
            if( system->MinLSNAvail != 99999 )
                cells[c.LSNAvail]->Value= system->MinLSNAvail;
        }
        cells[c.Dist]->Value        = GridPrintDistance(system, SystemsGrid->Filter->RefSystem, gv, age);
        cells[c.Scan]->Value        = system->PrintScanStatus();
        /*
        // WORMHOLE TODO
        if( system->HasWormhole )
            cells[c.Wormhole]->Value= system->PrintWormholeTarget();
        */
        if( system->LastVisited != -1 )
            cells[c.Visited]->Value = system->LastVisited;
        cells[c.Colonies]->Value    = system->PrintColonies( -1, GameData::Player );
        cells[c.Notes]->Value       = system->Comment;

        //SystemsGrid->Columns[colColonies->Ordinal]->DefaultCellStyle->WrapMode = DataGridViewTriState::True;

        for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
            plugin->AddRowData(row, SystemsGrid->Filter, system);
    }

    // Setup tooltips
    SetGridBgAndTooltip(SystemsGrid);

    SystemsGrid->FullUpdateEnd();
}

void Form1::SystemsSelectPlanets( int rowIndex )
{
    if( rowIndex >= 0 )
    {
        int index = SystemsGrid->Columns[0]->Index;
        IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(SystemsGrid->Rows[ rowIndex ]->Cells[index]->Value);

        PlanetsRefXYZ->Text = iDataSrc->GetFilterSystem()->PrintLocation();
        PlanetsMaxMishap->Value = 0;
        MenuTabs->SelectedIndex = TabIndex::Planets;
    }
}

void Form1::SystemsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = SystemsGrid->Columns[0]->Index;
    StarSystem ^system  = safe_cast<StarSystem^>(SystemsGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_SystemsMenuRef    = system;
    m_SystemsMenuRefRow = rowIndex;

    if( system->Planets->Count > 0 )
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

    // Ship jumps to this system
    ToolStripMenuItem ^jumpMenu = ShipsMenuAddJumpsHere( system, -1 );
    if( jumpMenu )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add( jumpMenu );
    }
}

void Form1::SystemsMenuShowPlanets(Object^, EventArgs^)
{
    SystemsSelectPlanets(m_SystemsMenuRefRow);
}

void Form1::SystemsMenuShowColonies(Object^, EventArgs^)
{
    ColoniesRefXYZ->Text = m_SystemsMenuRef->PrintLocation();
    ColoniesMaxMishap->Value = 0;
    MenuTabs->SelectedIndex = TabIndex::Colonies;
}

void Form1::SystemsMenuSelectRef(Object^, EventArgs^)
{
    SystemsGrid->Filter->SetRefSystem(m_SystemsMenuRef);
}

////////////////////////////////////////////////////////////////
// Planets

void Form1::PlanetsInitControls()
{
    PlanetsGridSorter ^sorter = gcnew PlanetsGridSorter(PlanetsGrid);

    // Add columns
    PlanetsColumns %c = m_PlanetsColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object    = ADD_COLUMN(nullptr,       nullptr,                Planet,     None,       Default);
    c.Name      = ADD_COLUMN("Name",        "Planet name",          String,     Ascending,  Default);
    c.Location  = ADD_COLUMN("Location",    "X Y Z #Planet Number", String,     Ascending,  Location);
    c.TC        = ADD_COLUMN("TC",          "Temperature class",    int,        Ascending,  Default);
    c.PC        = ADD_COLUMN("PC",          "Pressure class",       int,        Ascending,  Default);
    c.MD        = ADD_COLUMN("MD",          "Mining difficulty",    double,     Ascending,  Default);
    c.LSN       = ADD_COLUMN("LSN",         "LSN",                  int,        Ascending,  Default);
    c.Dist      = ADD_COLUMN("Distance",    "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.Visited   = ADD_COLUMN("Vis",         "Last turn you visited planet's system", int, Descending, Default);
    c.Scan      = ADD_COLUMN("Scan",        "Planet scan source",   String,     Ascending,  Default);
    c.Colonies  = ADD_COLUMN("Colonies",    "Summary of colonies and named planets", String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Planets, sorter);

    c.Notes    = ADD_COLUMN("Notes", "Notes", String, Ascending,  Default);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(PlanetsGrid, nullptr, c.Object, c.LSN, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Planets, PlanetsGrid);

    PlanetsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    PlanetsGrid->Columns[c.Colonies]->DefaultCellStyle->Font =
        gcnew System::Drawing::Font(L"Tahoma", 6.75F);

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(PlanetsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::PlanetsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlRef         = PlanetsRef;
    filter->CtrlRefHome     = PlanetsRefHome;
    filter->CtrlRefEdit     = PlanetsRefEdit;
    filter->CtrlRefXYZ      = PlanetsRefXYZ;
    filter->CtrlRefColony   = PlanetsRefColony;
    filter->CtrlRefShip     = PlanetsRefShip;
    filter->CtrlGV          = TechGV;
    filter->CtrlShipAge     = PlanetsShipAge;
    filter->CtrlMaxMishap   = PlanetsMaxMishap;
    filter->CtrlMaxLSN      = PlanetsMaxLSN;
    filter->CtrlFiltVisV    = PlanetsFiltVisV;
    filter->CtrlFiltVisN    = PlanetsFiltVisN;
    filter->CtrlFiltColC    = PlanetsFiltColC;
    filter->CtrlFiltColN    = PlanetsFiltColN;
    filter->CtrlNumRows     = PlanetsNumRows;

    // Store objects
    PlanetsGrid->Filter = filter;
    PlanetsGrid->Sorter = sorter;
}

void Form1::PlanetsUpdateControls()
{
    // Inhibit grid update
    PlanetsGrid->Filter->EnableUpdates  = false;

    PlanetsGrid->Filter->GameData       = m_GameData;
    PlanetsGrid->Filter->DefaultLSN     = Math::Min(99, GameData::Player->TechLevelsAssumed[TECH_LS]);

    PlanetsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    PlanetsRefHome->DataSource      = m_RefListHomes;
    PlanetsRefColony->DataSource    = m_RefListColonies;
    PlanetsRefShip->DataSource      = m_RefListShips;

    PlanetsRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    PlanetsGrid->Filter->EnableUpdates  = true;
    PlanetsRefHome->Text = GameData::Player->Name;
    PlanetsGrid->Filter->Reset();
}

void Form1::PlanetsSetup()
{
    PlanetsGrid->FullUpdateBegin();

    PlanetsColumns %c = m_PlanetsColumns;

    int gv  = Decimal::ToInt32(TechGV->Value);
    int age = Decimal::ToInt32(PlanetsShipAge->Value);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        for each( Planet ^planet in system->Planets->Values )
        {
            if( PlanetsGrid->Filter->Filter(planet) )
                continue;

            DataGridViewRow ^row = PlanetsGrid->Rows[ PlanetsGrid->Rows->Add() ];
            DataGridViewCellCollection ^cells = row->Cells;
            cells[c.Object]->Value  = planet;
            cells[c.Name]->Value    = planet->GetNameWithOrders();
            cells[c.Location]->Value= planet->PrintLocation();
            cells[c.TC]->Value      = planet->TempClass;
            cells[c.PC]->Value      = planet->PressClass;
            cells[c.MD]->Value      = (double)planet->MiDiff / 100;
            cells[c.LSN]->Value     = planet->LSN;
            cells[c.Dist]->Value    = GridPrintDistance(system, PlanetsGrid->Filter->RefSystem, gv, age);
            if( system->LastVisited != -1 )
                cells[c.Visited]->Value = system->LastVisited;
            cells[c.Scan]->Value    = system->PrintScanStatus();
            cells[c.Colonies]->Value= system->PrintColonies( planet->Number, GameData::Player );

            if( (planet->System->HomeSpecies != nullptr) )
            {
                // A home planet exists in the system. Make a note about that
                String ^note = planet->PrintComment();
                if ( !String::IsNullOrEmpty(note) )
                {
                    note += "; ";
                }
                note += String::Format("SP {0} home system", planet->System->HomeSpecies->Name );
                cells[c.Notes]->Value = note;
            }
            else
            {
                cells[c.Notes]->Value    = planet->Comment;
            }

            for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
                plugin->AddRowData(row, PlanetsGrid->Filter, planet);
        }
    }

    // Setup tooltips
    SetGridBgAndTooltip(PlanetsGrid);

    PlanetsGrid->FullUpdateEnd();
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
            MenuTabs->SelectedIndex  = TabIndex::Colonies;
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

    if( String::IsNullOrEmpty(planet->Name) )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add(
            "Name this planet...",
            nullptr,
            gcnew EventHandler(this, &Form1::PlanetsMenuAddNameStart) );
    }
    else if( planet->NameIsDisband )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add(
            "Cancel Disband",
            nullptr,
            gcnew EventHandler(this, &Form1::PlanetsMenuRemoveNameCancel) );
    }
    else if( planet->NameIsNew ||
        planet->NumColoniesOwned == 0 )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add(
            "Remove Name",
            nullptr,
            gcnew EventHandler(this, &Form1::PlanetsMenuRemoveName) );
    }

    // Ship jumps to this planet
    ToolStripMenuItem ^jumpMenu = ShipsMenuAddJumpsHere( planet->System, planet->Number );
    if( jumpMenu )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add( jumpMenu );
    }
}

void Form1::PlanetsMenuShowColonies(Object^, EventArgs^)
{
    PlanetsSelectColonies(m_PlanetsMenuRefRow);
}

void Form1::PlanetsMenuSelectRef(Object^, EventArgs^)
{
    PlanetsGrid->Filter->SetRefSystem(m_PlanetsMenuRef->System);
}

void Form1::PlanetsMenuAddNameStart(Object^, EventArgs^)
{
    PlanetsGrid->ClearSelection();
    PlanetsGrid->CurrentCell = PlanetsGrid[
        PlanetsGrid->Columns["Name"]->Index,
        m_PlanetsMenuRefRow];
    PlanetsGrid->ReadOnly = false;
    PlanetsGrid->BeginEdit(true);
}

void Form1::PlanetsMenuAddName(DataGridViewCellEventArgs ^cell)
{
    PlanetsGrid->ReadOnly = true;
    String ^name = PlanetsGrid[cell->ColumnIndex, cell->RowIndex]->Value->ToString();
    name = name->Trim();
    if( String::IsNullOrEmpty(name) )
        return;

    bool addName = true;

    // Check for duplicate name
    String ^nameLower = name->ToLower();
    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        for each( Planet ^planet in system->Planets->Values )
            if( !String::IsNullOrEmpty(planet->Name) &&
                planet->Name->ToLower() == nameLower )
            {
                MessageBox::Show(
                    this,
                    "Planet named '" + name + "' already exists.",
                    "Planet Name",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);

                addName = false;
            }
    }

    if( addName )
    {
        m_CommandMgr->AddCommand( 
            gcnew CmdPlanetName( m_PlanetsMenuRef->System, m_PlanetsMenuRef->Number, name ) );

        m_PlanetsMenuRef->AddName(name);
    }

    PlanetsGrid->Filter->Update();
}

void Form1::PlanetsMenuRemoveName(Object^, EventArgs^)
{
    if( m_PlanetsMenuRef->NameIsNew )
    {   // Remove Name command
        for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
        {
            if( iCmd->GetType() == CommandType::Name )
            {
                CmdPlanetName ^cmd = safe_cast<CmdPlanetName^>(iCmd);
                if( cmd->m_System == m_PlanetsMenuRef->System &&
                    cmd->m_PlanetNum == m_PlanetsMenuRef->Number )
                {
                    m_CommandMgr->DelCommand(iCmd);
                    break;
                }
            }
        }
    }
    else
    {   // Add Disband command
        m_CommandMgr->AddCommand( gcnew CmdDisband( m_PlanetsMenuRef->Name ) );
    }

    m_PlanetsMenuRef->DelName();

    PlanetsGrid->Filter->Update();
}

void Form1::PlanetsMenuRemoveNameCancel(Object^, EventArgs^)
{
    // Remove Disband command
    for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
    {
        if( iCmd->GetType() == CommandType::Disband )
        {
            CmdDisband ^cmd = safe_cast<CmdDisband^>(iCmd);
            if( cmd->m_Name == m_PlanetsMenuRef->Name )
            {
                m_CommandMgr->DelCommand(iCmd);
                m_PlanetsMenuRef->NameIsDisband = false;
                PlanetsGrid->Filter->Update();
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////
// Colonies

void Form1::ColoniesInitControls()
{
    ColoniesGridSorter ^sorter = gcnew ColoniesGridSorter(ColoniesGrid);

    // Add columns
    ColoniesColumns %c = m_ColoniesColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object    = ADD_COLUMN(nullptr,       nullptr,                        Colony, None,       Default);
    c.Owner     = ADD_COLUMN("Owner",       "Colony owner",                 String, Ascending,  Owner);
    c.Name      = ADD_COLUMN("Name",        "Colony name",                  String, Ascending,  Default);
    c.Type      = ADD_COLUMN("Type",        "Colony type",                  String, Ascending,  Type);
    c.Location  = ADD_COLUMN("Loc",         "X Y Z #Planet location",       String, Ascending,  Location);
    c.Size      = ADD_COLUMN("Size",        "Colony economic base",         double, Descending, Default);
    c.Prod      = ADD_COLUMN("PR",          "Production",                   int,    Descending, Default);
    c.Shipyards = ADD_COLUMN("SY",          "Shipyards",                    int,    Descending, Default);
    c.LSN       = ADD_COLUMN("LSN",         "LSN",                          int,    Ascending,  Default);
    c.Balance   = ADD_COLUMN("Balance",     "IU/AU balance",                String, Descending, Default);
    c.Pop       = ADD_COLUMN("Pop",         "Available population",         int,    Descending, Default);
    c.Dist      = ADD_COLUMN("Distance",    "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.Inventory = ADD_COLUMN("Inventory",   "Planetary inventory",          String, Ascending, Default);
    c.ProdOrder = ADD_COLUMN("#",           "Production order for orders template", int, Ascending, Default);
    c.ProdPerc  = ADD_COLUMN("Eff",         "Production effectiveness",     int,    Descending, Default);
    c.Seen      = ADD_COLUMN("Seen",        "Last seen turn",               int,    Descending, Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Colonies, sorter);

    c.Notes        = ADD_COLUMN("Notes", "Notes", String, Ascending, Default);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(ColoniesGrid, nullptr, c.Object, c.Prod, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Colonies, ColoniesGrid);

    ColoniesGrid->Columns[c.Size]->DefaultCellStyle->Format = "F1";
    ColoniesGrid->Columns[c.Dist]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    ColoniesGrid->Columns[c.Inventory]->DefaultCellStyle->Font =
        gcnew System::Drawing::Font(L"Tahoma", 6.75F);

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(ColoniesGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ColoniesSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlRef         = ColoniesRef;
    filter->CtrlRefHome     = ColoniesRefHome;
    filter->CtrlRefEdit     = ColoniesRefEdit;
    filter->CtrlRefXYZ      = ColoniesRefXYZ;
    filter->CtrlRefColony   = ColoniesRefColony;
    filter->CtrlRefShip     = ColoniesRefShip;
    filter->CtrlGV          = TechGV;
    filter->CtrlShipAge     = ColoniesShipAge;
    filter->CtrlMaxMishap   = ColoniesMaxMishap;
    filter->CtrlMaxLSN      = ColoniesMaxLSN;
    filter->CtrlFiltOwnO    = ColoniesFiltOwnO;
    filter->CtrlFiltOwnN    = ColoniesFiltOwnN;
    filter->CtrlMiMaBalance = ColoniesMiMaBalanced;
    filter->CtrlNumRows     = ColoniesNumRows;

    // Store objects
    ColoniesGrid->Filter = filter;
    ColoniesGrid->Sorter = sorter;
}

void Form1::ColoniesUpdateControls()
{
    // Inhibit grid update
    ColoniesGrid->Filter->EnableUpdates = false;

    ColoniesGrid->Filter->GameData      = m_GameData;

    ColoniesRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ColoniesRefHome->DataSource     = m_RefListHomes;
    ColoniesRefColony->DataSource   = m_RefListColonies;
    ColoniesRefShip->DataSource     = m_RefListShips;

    ColoniesRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    ColoniesGrid->Filter->EnableUpdates = true;
    ColoniesRefHome->Text = GameData::Player->Name;
    ColoniesGrid->Filter->Reset();

    ColoniesGrid->Sorter->SetGroupBySpecies( ColoniesGroupByOwner->Checked );
}

void Form1::ColoniesSetup()
{
    ColoniesGrid->FullUpdateBegin();

    ColoniesColumns %c = m_ColoniesColumns;

    int gv  = Decimal::ToInt32(TechGV->Value);
    int age = Decimal::ToInt32(ColoniesShipAge->Value);
    Alien ^sp = GameData::Player;

    for each( Colony ^colony in m_GameData->GetColonies() )
    {
        if( ColoniesGrid->Filter->Filter(colony) )
            continue;

        DataGridViewRow ^row = ColoniesGrid->Rows[ ColoniesGrid->Rows->Add() ];
        DataGridViewCellCollection ^cells = row->Cells;
        cells[c.Object]->Value      = colony;
        cells[c.Owner]->Value       = colony->Owner == sp ? String::Format("* {0}", sp->Name) : colony->Owner->Name;
        cells[c.Name]->Value        = colony->Name;
        cells[c.Type]->Value        = FHStrings::PlTypeToString(colony->PlanetType);
        cells[c.Location]->Value    = colony->PrintLocation();
        if( colony->EconomicBase != -1 )
            cells[c.Size]->Value    = (double)colony->EconomicBase / 10;
        cells[c.Dist]->Value        = GridPrintDistance(colony->System, ColoniesGrid->Filter->RefSystem, gv, age);
        cells[c.Inventory]->Value   = colony->PrintInventory();
        if( colony->Planet )
            cells[c.LSN]->Value     = colony->Planet->LSN;

        if( colony->Owner == sp )
        {
            int prodCalculated = Calculators::ColonyProduction(
                colony,
                sp->TechLevelsAssumed[TECH_MI],
                sp->TechLevelsAssumed[TECH_MA],
                sp->TechLevelsAssumed[TECH_LS],
                m_GameData->GetFleetPercentCost() );

            cells[c.Prod]->Value    = prodCalculated;
            cells[c.ProdOrder]->Value = colony->ProductionOrder;
            cells[c.ProdPerc]->Value= 100 - colony->ProdPenalty;
            cells[c.Pop]->Value     = colony->AvailPop;

            if( colony->PlanetType == PLANET_HOME ||
                colony->PlanetType == PLANET_COLONY )
            {
                colony->CalculateBalance(ColoniesGrid->Filter->MiMaBalanced);
                cells[c.Balance]->Value = colony->PrintBalance();
            }
        }
        else
        {
            if( colony->LastSeen > 0 )
                cells[c.Seen]->Value = colony->LastSeen;
        }

        if( colony->Shipyards != -1 )
        {
            cells[c.Shipyards]->Value = colony->Shipyards;
        }

        String^ notes = gcnew String("");
        if( colony->Hidden )
        {
            if ( ! String::IsNullOrEmpty(notes) ) notes += ", ";
            notes += "Hidden";
        }

        if( colony->UnderSiege )
        {
            if ( ! String::IsNullOrEmpty(notes) ) notes += ", ";
            notes += "Under siege";
            cells[c.Prod]->Style->ForeColor = Color::Red;
            cells[c.Notes]->Style->ForeColor = Color::Red;
        }

        if( colony->RecoveryIU + colony->RecoveryAU > 0 )
        {
            if ( ! String::IsNullOrEmpty(notes) ) notes += ", ";
            notes += String::Format("Recovery: {0} IU, {1} AU", colony->RecoveryIU, colony->RecoveryAU);
            cells[c.Prod]->Style->ForeColor = Color::Red;
            cells[c.Notes]->Style->ForeColor = Color::Red;
        }

        cells[c.Notes]->Value = notes;

        for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
            plugin->AddRowData(row, ColoniesGrid->Filter, colony);
    }

    // Setup tooltips
    SetGridBgAndTooltip(ColoniesGrid);

    ColoniesGrid->FullUpdateEnd();
}

void Form1::ColoniesSetRef( int rowIndex )
{
    if( rowIndex != -1 )
    {
        int index = ColoniesGrid->Columns[0]->Index;
        Colony ^colony = safe_cast<Colony^>(ColoniesGrid->Rows[ rowIndex ]->Cells[index]->Value);
        ColoniesRefColony->Text = colony->PrintRefListEntry();
    }
}

void Form1::ColoniesFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = ColoniesGrid->Columns[0]->Index;
    Colony ^colony = safe_cast<Colony^>(ColoniesGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_ColoniesMenuRef = colony;

    menu->Items->Add(
        "Select ref: PL " + colony->PrintRefListEntry(),
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesFiltersReset_Click));

    if( colony->Owner == GameData::Player )
    {
        menu->Items->Add( gcnew ToolStripSeparator );

        // Production order adjustment
        if( colony->ProductionOrder > 0 )
        {
            menu->Items->Add( CreateCustomMenuItem(
                "Prod. Order: First",
                -1000000,
                gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
            menu->Items->Add( CreateCustomMenuItem(
                "Prod. Order: 1 Up",
                -1,
                gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
        }
        menu->Items->Add( CreateCustomMenuItem(
            "Prod. Order: 1 Down",
            1,
            gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
        menu->Items->Add( CreateCustomMenuItem(
            "Prod. Order: Last",
            1000000,
            gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );

        if( colony->CanProduce )
        {
            // Shipyard
            if( colony->GetMaxProductionBudget() > Calculators::ShipyardCost( GameData::Player->TechLevels[TECH_MA] ) )
            {
                menu->Items->Add( gcnew ToolStripSeparator );
                ToolStripMenuItem ^item = gcnew ToolStripMenuItem(
                    "Build Shipyard",
                    nullptr,
                    gcnew EventHandler(this, &Form1::ColoniesMenuProdShipyard));
                if( colony->OrderBuildShipyard )
                    item->Checked = true;
                menu->Items->Add( item );
            }
        }
    }

    // Ship jumps to this colony
    ToolStripMenuItem ^jumpMenu = ShipsMenuAddJumpsHere(
        colony->System,
        colony->PlanetNum );
    if( jumpMenu )
    {
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add( jumpMenu );
    }
}

void Form1::ColoniesMenuSelectRef(Object^, EventArgs ^e)
{
    ColoniesRefColony->Text = m_ColoniesMenuRef->PrintRefListEntry();
}

void Form1::ColoniesMenuProdOrderAdjust(int adjustment)
{
    int oldOrder = m_ColoniesMenuRef->ProductionOrder;
    int newOrder = Math::Max(1, oldOrder + adjustment);
    int lastOrder = 0;
    for each( Colony ^colony in GameData::Player->Colonies )
    {
        if( colony != m_ColoniesMenuRef )
        {
            if( colony->ProductionOrder >= newOrder &&
                colony->ProductionOrder < oldOrder )
            {
                ++colony->ProductionOrder;
            }
            else if( colony->ProductionOrder >= oldOrder &&
                colony->ProductionOrder <= newOrder )
            {
                --colony->ProductionOrder;
            }

            lastOrder = Math::Max(lastOrder, colony->ProductionOrder);
        }
    }

    newOrder = Math::Min(newOrder, lastOrder + 1);
    m_ColoniesMenuRef->ProductionOrder = newOrder;
    GameData::Player->SortColoniesByProdOrder();
    ColoniesGrid->Filter->Update();

    m_CommandMgr->SaveCommands();
}

void Form1::ColoniesMenuProdShipyard(Object^, EventArgs^)
{
    m_ColoniesMenuRef->OrderBuildShipyard = !m_ColoniesMenuRef->OrderBuildShipyard;
    m_CommandMgr->SaveCommands();
}

////////////////////////////////////////////////////////////////
// Ships

void Form1::ShipsInitControls()
{
    ShipsGridSorter ^sorter = gcnew ShipsGridSorter(ShipsGrid);

    // Add columns
    ShipsColumns %c = m_ShipsColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object    = ADD_COLUMN(nullptr,       nullptr,                    Ship,   None,       Default);
    c.Owner     = ADD_COLUMN("Owner",       "Ship owner",               String, Ascending,  Owner);
    c.Class     = ADD_COLUMN("Class",       "Ship class",               String, Ascending,  Type);
    c.Name      = ADD_COLUMN("Name",        "Ship name",                String, Ascending,  Default);
    c.Location  = ADD_COLUMN("Location",    "Current location",         String, Ascending,  Location);
    c.Age       = ADD_COLUMN("Age",         "Age",                      int,    Ascending,  Default);
    c.Cap       = ADD_COLUMN("Cap",         "Capacity",                 int,    Ascending,  Default);
    c.Dist      = ADD_COLUMN("Distance",    "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.Cargo     = ADD_COLUMN("Cargo",       "Cargo on board",           String, Ascending,  Default);
    c.Maint     = ADD_COLUMN("Maint",       "Maintenance cost",         double, Ascending,  Default);
    c.UpgCost   = ADD_COLUMN("Upg",         "Upgrade cost to age 0",    int,    Ascending,  Default);
    c.RecVal    = ADD_COLUMN("Rec",         "Recycle value",            int,    Descending, Default);
    c.Order     = ADD_COLUMN("Order",       "Order",                    String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Ships, sorter);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(ShipsGrid, nullptr, c.Object, c.Class, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Ships, ShipsGrid);

    ShipsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    ShipsGrid->Columns[c.Cargo]->DefaultCellStyle->Font =
        gcnew System::Drawing::Font(L"Tahoma", 6.75F);

    GridFilter ^filter = gcnew GridFilter(ShipsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ShipsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlRef         = ShipsRef;
    filter->CtrlRefHome     = ShipsRefHome;
    filter->CtrlRefEdit     = ShipsRefEdit;
    filter->CtrlRefXYZ      = ShipsRefXYZ;
    filter->CtrlRefColony   = ShipsRefColony;
    filter->CtrlRefShip     = ShipsRefShip;
    filter->CtrlGV          = TechGV;
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
    filter->CtrlNumRows     = ShipsNumRows;

    // Store objects
    ShipsGrid->Filter = filter;
    ShipsGrid->Sorter = sorter;
}

void Form1::ShipsUpdateControls()
{
    // Inhibit grid update
    ShipsGrid->Filter->EnableUpdates = false;

    ShipsGrid->Filter->GameData      = m_GameData;

    ShipsRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ShipsRefHome->DataSource     = m_RefListHomes;
    ShipsRefColony->DataSource   = m_RefListColonies;
    ShipsRefShip->DataSource     = m_RefListShips;

    ShipsRefHome->Text = GridFilter::s_CaptionHome;
    // Trigger grid update
    ShipsGrid->Filter->EnableUpdates = true;
    ShipsRefHome->Text = GameData::Player->Name;
    ShipsGrid->Filter->Reset();

    ShipsGrid->Sorter->SetGroupBySpecies( ColoniesGroupByOwner->Checked );
}

void Form1::ShipsSetup()
{
    ShipsGrid->FullUpdateBegin();

    ShipsColumns %c = m_ShipsColumns;

    Alien ^sp = GameData::Player;
    int gv = sp->TechLevelsAssumed[TECH_GV];
    int ml = sp->TechLevelsAssumed[TECH_ML];
    double discount = Calculators::ShipMaintenanceDiscount(ml);

    for each( Ship ^ship in m_GameData->GetShips() )
    {
        if( ShipsGrid->Filter->Filter(ship) )
            continue;

        DataGridViewRow ^row = ShipsGrid->Rows[ ShipsGrid->Rows->Add() ];
        DataGridViewCellCollection ^cells = row->Cells;
        cells[c.Object]->Value      = ship;
        cells[c.Owner]->Value       = ship->Owner == sp ? String::Format("* {0}", sp->Name) : ship->Owner->Name;
        cells[c.Class]->Value       = ship->PrintClass();
        cells[c.Name]->Value        = ship->Name;
        if( ship->EUToComplete > 0 )
            cells[c.Name]->Value    = ship->Name + " (incomplete)";
        cells[c.Location]->Value    = ship->PrintLocation();
        if( !ship->IsPirate )
            cells[c.Age]->Value     = ship->Age;
        cells[c.Cap]->Value         = ship->Capacity;
        cells[c.Dist]->Value        = GridPrintDistance(ship->System, ShipsGrid->Filter->RefSystem, gv, ship->Age);
        if( sp == ship->Owner )
        {
            cells[c.Cargo]->Value   = ship->PrintCargo();
            cells[c.Maint]->Value   = ship->GetMaintenanceCost() * discount;
            cells[c.UpgCost]->Value = ship->GetUpgradeCost();
            cells[c.RecVal]->Value  = ship->GetRecycleValue();
            if( ship->Command )
                cells[c.Order]->Value = ship->Command->Print();
            else if( ship->EUToComplete )
                cells[c.Order]->Value = ship->EUToComplete.ToString() + " EU to complete";
        }

        for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
            plugin->AddRowData(row, ShipsGrid->Filter, ship);
    }

    // Setup tooltips
    SetGridBgAndTooltip(ShipsGrid);

    ShipsGrid->FullUpdateEnd();
}

void Form1::ShipsSetRef( int rowIndex )
{
    if( rowIndex != -1 )
    {
        int index = ShipsGrid->Columns[0]->Index;
        Ship ^ship = safe_cast<Ship^>(ShipsGrid->Rows[ rowIndex ]->Cells[index]->Value);
        if( ship->Owner == GameData::Player )
            ShipsRefShip->Text = ship->PrintRefListEntry();
        else
            ShipsGrid->Filter->SetRefSystem(ship->System);
    }
}

void Form1::ShipsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = ShipsGrid->Columns[0]->Index;
    Ship ^ship = safe_cast<Ship^>(ShipsGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_ShipsMenuRef = ship;

    menu->Items->Add(
        String::Format("Select ref: {0} {1} (@ {2})",
            ship->PrintClass(),
            ship->Name,
            ship->System->PrintLocation() ),
        nullptr,
        gcnew EventHandler(this, &Form1::ShipsMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::ShipsFiltersReset_Click));

    // Ship orders
    if( ship->Owner == GameData::Player )
    {
        menu->Items->Add( gcnew ToolStripSeparator );

        bool prodOrderPossible = false;
        for each( Colony ^colony in ship->Owner->Colonies )
        {
            if( colony->System == ship->System &&
                colony->CanProduce )
            {
                prodOrderPossible = true;
                break;
            }
        }

        if( prodOrderPossible )
        {
            EventHandler1Arg<ShipOrderData^> ^handler =
                gcnew EventHandler1Arg<ShipOrderData^>(this, &Form1::ShipsMenuOrderSet);
            ToolStripMenuItem ^menuItem;

            if( ship->EUToComplete == 0 )
            {
                menuItem = CreateCustomMenuItem(
                    "Upgrade",
                    gcnew ShipOrderData(
                        ship,
                        gcnew Ship::Order(Ship::OrderType::Upgrade) ),
                     handler );
                if( ship->Command &&
                    ship->Command->Type == Ship::OrderType::Upgrade )
                {
                    menuItem->Checked = true;
                }
                menu->Items->Add( menuItem );
            }

            menuItem = CreateCustomMenuItem(
                "Recycle",
                gcnew ShipOrderData(
                    ship,
                    gcnew Ship::Order(Ship::OrderType::Recycle) ),
                 handler );
            if( ship->Command &&
                ship->Command->Type == Ship::OrderType::Recycle )
                menuItem->Checked = true;
            menu->Items->Add( menuItem );

            if( ship->System->HasWormhole )
            {
                menuItem = CreateCustomMenuItem(
                    "Enter Wormhole: " + ship->System->PrintWormholeTarget(),
                    gcnew ShipOrderData(
                        ship,
                        gcnew Ship::Order(Ship::OrderType::Wormhole) ),
                     handler );
                if( ship->Command &&
                    ship->Command->Type == Ship::OrderType::Wormhole )
                    menuItem->Checked = true;
                menu->Items->Add( menuItem );
            }
        }

        if( ship->CanJump )
        {
            ToolStripMenuItem ^jumpMenu = gcnew ToolStripMenuItem("Jump to:");
            bool anyJump = false;

            if( ship->Command &&
                ship->Command->Type == Ship::OrderType::Jump )
                jumpMenu->Checked = true;

            // Jump to ref system
            if( ShipsGrid->Filter->RefSystem != ship->System )
            {
                jumpMenu->DropDownItems->Add(
                    ShipsMenuCreateJumpItem(
                        ship,
                        ShipsGrid->Filter->RefSystem,
                        -1,
                        ShipsRef->Text ) );
                anyJump = true;
            }

            // Colonies
            for each( Colony ^colony in GameData::Player->Colonies )
            {
                if( colony->System != ship->System )
                {
                    jumpMenu->DropDownItems->Add(
                        ShipsMenuCreateJumpItem(
                            ship,
                            colony->System,
                            colony->PlanetNum,
                            "PL " + colony->Name ) );
                    anyJump = true;
                }
            }

            // Named planets
            bool anyPlanet = false;
            for each( PlanetName ^planet in m_GameData->GetPlanetNames() )
            {
                if( planet->System != ship->System )
                {
                    if( !anyPlanet && anyJump )
                        jumpMenu->DropDownItems->Add( gcnew ToolStripSeparator );

                    jumpMenu->DropDownItems->Add(
                        ShipsMenuCreateJumpItem(
                            ship,
                            planet->System,
                            planet->PlanetNum,
                            "PL " + planet->Name ) );
                    anyJump = true;
                    anyPlanet = true;
                }
            }

            if( anyJump )
                menu->Items->Add( jumpMenu );
        }

        // If ship already has command, add option to cancel it.
        if( ship->Command )
        {
            menu->Items->Add( CreateCustomMenuItem<ShipOrderData^>(
                "Cancel Command",
                gcnew ShipOrderData(ship, nullptr),
                gcnew EventHandler1Arg<ShipOrderData^>(this, &Form1::ShipsMenuOrderSet) ) );
        }
    }
}

ToolStripMenuItem^ Form1::ShipsMenuCreateJumpItem(
    Ship ^ship, StarSystem ^system, int planetNum, String ^text )
{
    double mishap = ship->System->CalcMishap(
        system,
        Decimal::ToInt32(TechGV->Value),
        ship->Age );

    String ^itemText;
    Ship::OrderType order = Ship::OrderType::Jump;
    if( ship->System->WormholeTarget == system )
    {
        order = Ship::OrderType::Wormhole;
        itemText = String::Format("{0}   (Wormhole)  From {1}",
            text,
            ship->PrintLocation() );
    }
    else
    {
        itemText = String::Format("{0}   {1:F2}% ({2} FS)  From {3}",
            text,
            mishap,
            ship->Cargo[INV_FS],
            ship->PrintLocation() );
    }

    ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
        itemText,
        gcnew ShipOrderData(ship, gcnew Ship::Order(order, system, planetNum)),
        gcnew EventHandler1Arg<ShipOrderData^>(this, &Form1::ShipsMenuOrderSet) );

    // Add checkbox
    if( ship->Command &&
        ship->Command->Type == order &&
        ship->Command->JumpTarget == system &&
        ship->Command->PlanetNum == planetNum )
    {
        menuItem->Checked = true;
    }

    return menuItem;
}

ToolStripMenuItem^ Form1::ShipsMenuAddJumpsHere(
    StarSystem ^system, int planetNum )
{
    ToolStripMenuItem ^jumpMenu = gcnew ToolStripMenuItem("Jump Here:");
    bool anyJump = false;

    for each( Ship ^ship in GameData::Player->Ships )
    {
        if( ship->CanJump == false )
            continue;

        if( ship->System != system )
        {
            ToolStripMenuItem ^menuItem = ShipsMenuCreateJumpItem(
                ship,
                system,
                planetNum,
                ship->PrintClassWithName() );
            if( menuItem->Checked )
                jumpMenu->Checked = true;
            jumpMenu->DropDownItems->Add( menuItem );
            anyJump = true;
        }
    }
    if( anyJump )
        return jumpMenu;
    return nullptr;
}

void Form1::ShipsMenuSelectRef(Object^, EventArgs ^e)
{
    if( m_ShipsMenuRef->Owner == GameData::Player )
        ShipsRefShip->Text = m_ShipsMenuRef->PrintRefListEntry();
    else
        ShipsGrid->Filter->SetRefSystem(m_ShipsMenuRef->System);
}

void Form1::ShipsMenuOrderSet(ShipOrderData ^data)
{
    data->A->Command = data->B;
    ShipsGrid->Filter->Update();
    m_CommandMgr->SaveCommands();
}

////////////////////////////////////////////////////////////////
// Aliens

void Form1::AliensInitControls()
{
    AliensGridSorter ^sorter = gcnew AliensGridSorter(AliensGrid);

    // Add columns
    AliensColumns %c = m_AliensColumns;

#define ADD_COLUMN(title, desc, type, sortOrder, customSortMode)    \
    sorter->AddColumn(title, desc, type::typeid, SortOrder::sortOrder, GridSorterBase::CustomSortMode::customSortMode)

    c.Object    = ADD_COLUMN(nullptr,       nullptr,                    Alien,  None,       Default);
    c.Name      = ADD_COLUMN("Name",        "Species name",             String, Ascending,  Default);
    c.Relation  = ADD_COLUMN("Relation",    "Current relation",         String, Ascending,  Relation);
    c.Home      = ADD_COLUMN("Home",        "Home planet location",     String, Ascending,  Location);
    c.Dist      = ADD_COLUMN("Dist",        "Home distance from your home system", double, Ascending, Default);
    c.TechLev   = ADD_COLUMN("Tech Levels", "Estimated technology levels", String, Ascending, Default);
    c.TC        = ADD_COLUMN("TC",          "Temperature class of their home planet", int,  Ascending,  Default);
    c.PC        = ADD_COLUMN("PC",          "Pressure class of their home planet",    int,    Ascending,  Default);
    c.Teach     = ADD_COLUMN("Teach",       "Teach orders",             String, Ascending,  Default);
    c.EMail     = ADD_COLUMN("EMail",       "Species email",            String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Aliens, sorter);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(AliensGrid, nullptr, c.Object, c.Relation, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Aliens, AliensGrid);

    AliensGrid->Columns[c.Dist]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;

    GridFilter ^filter = gcnew GridFilter(AliensGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::AliensSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);
    filter->Sorter = sorter;

    filter->CtrlFiltRelA    = AliensFiltRelA;
    filter->CtrlFiltRelE    = AliensFiltRelE;
    filter->CtrlFiltRelN    = AliensFiltRelN;
    filter->CtrlFiltRelP    = AliensFiltRelP;
    filter->CtrlNumRows     = AliensNumRows;

    // Store objects
    AliensGrid->Filter = filter;
    AliensGrid->Sorter = sorter;
}

void Form1::AliensUpdateControls()
{
    AliensGrid->Filter->EnableUpdates = true;
    AliensGrid->Filter->Reset();
}

void Form1::AliensSetup()
{
    AliensGrid->FullUpdateBegin();

    AliensColumns %c = m_AliensColumns;

    for each( Alien ^alien in m_GameData->GetAliens() )
    {
        if( AliensGrid->Filter->Filter(alien) )
            continue;

        DataGridViewRow ^row = AliensGrid->Rows[ AliensGrid->Rows->Add() ];
        DataGridViewCellCollection ^cells = row->Cells;
        cells[c.Object]->Value      = alien;
        cells[c.Name]->Value        = alien->Name;
        cells[c.Relation]->Value    = alien->PrintRelation();
        cells[c.Home]->Value        = alien->PrintHome();
        cells[c.TechLev]->Value     = alien->PrintTechLevels();
        if( alien->HomeSystem )
        {
            cells[c.Dist]->Value    = GameData::Player->HomeSystem->CalcDistance( alien->HomeSystem );
        }
        if( GameData::AtmReq->TempClass != -1 &&
            GameData::AtmReq->PressClass != -1 )
        {
            cells[c.TC]->Value      = GameData::AtmReq->TempClass;
            cells[c.PC]->Value      = GameData::AtmReq->PressClass;
        }
        cells[c.EMail]->Value       = alien->Email;

        if( alien->TeachOrders )
        {
            String ^teach = "";
            if( alien->TeachOrders & (1 << TECH_MI) ) teach += ", MI";
            if( alien->TeachOrders & (1 << TECH_MA) ) teach += ", MA";
            if( alien->TeachOrders & (1 << TECH_ML) ) teach += ", ML";
            if( alien->TeachOrders & (1 << TECH_GV) ) teach += ", GV";
            if( alien->TeachOrders & (1 << TECH_LS) ) teach += ", LS";
            if( alien->TeachOrders & (1 << TECH_BI) ) teach += ", BI";
            if( !String::IsNullOrEmpty(teach) )
                cells[c.Teach]->Value = teach->Substring(2);
        }

        for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
            plugin->AddRowData(row, AliensGrid->Filter, alien);
    }

    // Setup tooltips
    SetGridBgAndTooltip(AliensGrid);

    AliensGrid->FullUpdateEnd();
}

void Form1::AliensFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = AliensGrid->Columns[0]->Index;
    Alien ^alien = safe_cast<Alien^>(AliensGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_AliensMenuRef = alien;

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::AliensFiltersReset_Click));

    if( alien->Relation == SP_NEUTRAL ||
        alien->Relation == SP_ENEMY ||
        alien->Relation == SP_ALLY )
    {
        menu->Items->Add( gcnew ToolStripSeparator );

        ToolStripMenuItem ^teachMenu = gcnew ToolStripMenuItem("Teach");
        bool teachAny = false;

        if( alien->Relation != SP_NEUTRAL )
        {   // Declare neutrality
            ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
                "Declare Neutral",
                gcnew AlienRelationData(alien, SP_NEUTRAL),
                gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
            menu->Items->Add( menuItem );
        }
        if( alien->Relation != SP_ALLY )
        {   // Declare alliance
            ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
                "Declare Ally",
                gcnew AlienRelationData(alien, SP_ALLY),
                gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
            menu->Items->Add( menuItem );
        }
        if( alien->Relation != SP_ENEMY )
        {   // Declare enemy
            ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
                "Declare Enemy",
                gcnew AlienRelationData(alien, SP_ENEMY),
                gcnew EventHandler1Arg<AlienRelationData^>(this, &Form1::AliensMenuSetRelation) );
            menu->Items->Add( menuItem );
        }

        // Teach
        if( alien->TeachOrders )
        {
            teachMenu->DropDownItems->Add( "Cancel ALL",
                nullptr,
                gcnew EventHandler(this, &Form1::AliensMenuTeachCancel));
            teachAny = true;
        }
        if( alien->Relation != SP_ENEMY )
        {
            if( teachAny )
                teachMenu->DropDownItems->Add( gcnew ToolStripSeparator );

            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Mining", TECH_MI) );
            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Manufacturing", TECH_MA) );
            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Military", TECH_ML) );
            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Gravitics", TECH_GV) );
            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Life Support", TECH_LS) );
            teachMenu->DropDownItems->Add( AliensMenuCreateTeach("Biology", TECH_BI) );

            teachMenu->DropDownItems->Add( gcnew ToolStripSeparator );
            teachMenu->DropDownItems->Add( "Teach ALL",
                nullptr,
                gcnew EventHandler(this, &Form1::AliensMenuTeachAll));
            teachAny = true;
        }

        if( teachAny )
            menu->Items->Add( teachMenu );
    }
}

ToolStripMenuItem^ Form1::AliensMenuCreateTeach(String ^text, TechType tech)
{
    ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
        text,
        gcnew TeachData(m_AliensMenuRef, tech, GameData::Player->TechLevels[tech]),
        gcnew EventHandler1Arg<TeachData^>(this, &Form1::AliensMenuTeach) );

    return menuItem;
}

void Form1::AliensMenuTeach(TeachData ^data)
{
    Alien ^alien = data->A;
    TechType tech = (TechType)data->B;
    int level = data->C;

    for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
    {
        if( iCmd->GetType() == CommandType::Teach )
        {
            CmdTeach ^cmd = safe_cast<CmdTeach^>(iCmd);
            if( cmd->m_Alien == alien &&
                cmd->m_Tech == tech )
            {   // Cmd already exists
                return;
            }
        }
    }

    m_CommandMgr->AddCommand( gcnew CmdTeach(alien, tech, level) );

    alien->TeachOrders |= 1 << tech;
    AliensGrid->Filter->Update();
}

void Form1::AliensMenuTeachAll(Object^, EventArgs^)
{
    AliensGrid->Filter->EnableUpdates = false;

    TeachData ^data = gcnew TeachData(m_AliensMenuRef, TECH_MI, GameData::Player->TechLevels[TECH_MI]);
    AliensMenuTeach(data);

    data->B = TECH_MA;
    data->C = GameData::Player->TechLevels[TECH_MA];
    AliensMenuTeach(data);

    data->B = TECH_ML;
    data->C = GameData::Player->TechLevels[TECH_ML];
    AliensMenuTeach(data);

    data->B = TECH_GV;
    data->C = GameData::Player->TechLevels[TECH_GV];
    AliensMenuTeach(data);

    data->B = TECH_LS;
    data->C = GameData::Player->TechLevels[TECH_LS];
    AliensMenuTeach(data);

    data->B = TECH_BI;
    data->C = GameData::Player->TechLevels[TECH_BI];
    AliensMenuTeach(data);

    AliensGrid->Filter->EnableUpdates = true;
    AliensGrid->Filter->Update();
}

void Form1::AliensMenuTeachCancel(Object^, EventArgs^)
{
    bool removed = false;
    bool removedAny = false;
    do
    {
        removed = false;
        for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
        {
            if( iCmd->GetType() == CommandType::Teach )
            {
                CmdTeach ^cmd = safe_cast<CmdTeach^>(iCmd);
                if( cmd->m_Alien == m_AliensMenuRef )
                {
                    removed = removedAny = true;
                    m_CommandMgr->DelCommand(iCmd);
                    break;
                }
            }
        }
    } while( removed );
    
    if( removedAny )
    {
        m_CommandMgr->SaveCommands();
    }

    m_AliensMenuRef->TeachOrders = 0;
    AliensGrid->Filter->Update();
}

void Form1::AliensMenuSetRelation(AlienRelationData ^data)
{
    Alien ^alien = data->A;
    SPRelType rel = (SPRelType)data->B;

    bool addNew = true;

    // Modify existing relation command
    for each( ICommand ^iCmd in m_CommandMgr->GetCommands() )
    {
        if( iCmd->GetType() == CommandType::AlienRelation )
        {
            CmdAlienRelation ^cmd = safe_cast<CmdAlienRelation^>(iCmd);
            if( cmd->m_Alien == alien )
            {
                if( rel == alien->RelationOriginal )
                {
                    m_CommandMgr->DelCommand(iCmd);
                }
                else
                {
                    cmd->m_Relation = rel;
                    m_CommandMgr->SaveCommands();
                }
                addNew = false;
                break;
            }
        }
    }

    if( addNew )
    {
        // Add relation command
        m_CommandMgr->AddCommand( gcnew CmdAlienRelation(alien, rel) );
    }

    // Set relation
    alien->Relation = rel;

    // Cancel any teach commands for enemy species
    if( rel == SP_ENEMY && alien->TeachOrders )
        AliensMenuTeachCancel(nullptr, nullptr);

    AliensGrid->Filter->Update();
    // Update other grids to reflect new colors
    SystemsGrid->Filter->Update();
    PlanetsGrid->Filter->Update();
    ColoniesGrid->Filter->Update();
    ShipsGrid->Filter->Update();
}

void Form1::CopyOrdersTemplateToClipboard()
{
    Clipboard::SetText(OrderTemplate->Text, TextDataFormat::Text);
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
