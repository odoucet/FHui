#include "StdAfx.h"
#include "Form1.h"
#include "BuildInfo.h"

#include "GridFilter.h"
#include "GridSorter.h"

#include "ReportParser.h"

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
    InitializeComponent();
    InitPlugins();
    InitControls();
    InitGameData();
}

void Form1::InitControls()
{
    m_GridToolTip       = gcnew ToolTip;
    m_GridFontSmall     = gcnew System::Drawing::Font(L"Tahoma", 6.75F);
    m_GridFontSummary   = gcnew System::Drawing::Font(L"Tahoma", 8.0F, FontStyle::Italic);

    TextAbout->Text = GetAboutText();

    m_bGridUpdateEnabled = gcnew bool(false);

    SystemsInitControls();
    PlanetsInitControls();
    ColoniesInitControls();
    ShipsInitControls();
    AliensInitControls();

    LoadUISettings();
    ApplyUISettings();

    m_bUISaveEnabled    = true;
}

void Form1::ApplyUISettings()
{
    SystemsSelMode->Checked     = ( SystemsGrid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect );
    PlanetsSelMode->Checked     = ( PlanetsGrid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect );
    ColoniesSelMode->Checked    = ( ColoniesGrid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect );
    ShipsSelMode->Checked       = ( ShipsGrid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect );
}

void Form1::InitPlugins()
{
    m_PluginMgr = gcnew PluginManager(Application::StartupPath);

    if( EnablePlugins )
    {
        m_PluginMgr->LoadPlugins();
    }
}

void Form1::InitGameData()
{
    try
    {
        InitData();
        LoadOrders();
    }
    catch( Exception ^e )
    {
        Summary->Text = "Failed to initialize application.";
        ShowException(e);
    }
}

void Form1::InitData()
{
    m_bHadException = false;
    *m_bGridUpdateEnabled = false;

    m_OrderFiles = gcnew SortedList<String^, String^>;
    m_RepTurnNrData = nullptr;

    m_GameData      = gcnew GameData;
    m_CommandMgr    = gcnew CommandManager(m_GameData, GetDataDir(OrdersDir::Folder));
    m_ReportParser  = gcnew ReportParser(m_GameData, m_CommandMgr, m_RegexMatcher, GetDataDir("galaxy_list.txt"), GetDataDir("reports"));

    m_ReportParser->Verbose = Verbose;
    m_ReportParser->Stats = Stats;

    m_ReportParser->ScanReports();

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
        m_RefListColonies->Add( colony->PrintRefListEntry() );
    // then alien
    for each( Alien ^alien in GameData::GetAliens() )
        for each( Colony ^colony in alien->Colonies )
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
    UpdateAllGrids(true);
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
                MessageBoxButtons::OK/*Cancel*/,
                MessageBoxIcon::Question,
                MessageBoxDefaultButton::Button2);
            //if( result == System::Windows::Forms::DialogResult::OK )
            //{
            //    MapDraw();
            //}
            //else
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
        UpdateAllGrids(false);

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
    MenuTabs->SelectedTab = MenuTabs->TabPages[ TabIndex::Reports ];

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
        ret = DataDir + "\\" + ret;
    return ret;
}

void Form1::ShowReloadMenu()
{
    // Create menu
    Windows::Forms::ContextMenuStrip ^menu = gcnew Windows::Forms::ContextMenuStrip;

    menu->Items->Add( CreateCustomMenuItem(
        "Reload All (reset ALL orders)",
        true,
        gcnew EventHandler1Arg<bool>(this, &Form1::TurnReload) ) );

    menu->Items->Add( CreateCustomMenuItem(
        "Reload Reports (DON'T reset orders)",
        false,
        gcnew EventHandler1Arg<bool>(this, &Form1::TurnReload) ) );

    //menu->Items->Add(
    //    "Reload Auto Orders from current turn",
    //    nullptr,
    //    gcnew EventHandler(this, &Form1::ReloadAutoCommands) );

    if( m_PluginMgr->OrderPlugins->Count > 0 )
    {
        menu->Items->Add(
            "Recreate Plugin orders",
            nullptr,
            gcnew EventHandler(this, &Form1::RecreatePluginCommands) );
    }

    // Show menu
    Rectangle r = TurnReloadBtn->DisplayRectangle;
    menu->Show(TurnReloadBtn, r.Left + r.Width / 2, r.Top + r.Height / 2);
}

void Form1::TurnReload(bool resetCommands)
{
    if( resetCommands )
    {
        System::Windows::Forms::DialogResult result = MessageBox::Show(
            this,
            String::Format("Delete ALL FHUI Commands for TURN {0}?", m_GameData->CurrentTurn),
            "Reload Turn",
            MessageBoxButtons::YesNo,
            MessageBoxIcon::Question,
            MessageBoxDefaultButton::Button1);

        if( result == System::Windows::Forms::DialogResult::Yes )
            m_CommandMgr->DeleteCommands();
        else
            return;
    }

    // Bring up the first tab
    MenuTabs->SelectedTab = MenuTabs->TabPages[ TabIndex::Reports ];

    // Purge all grids
    SystemsGrid->Rows->Clear();
    PlanetsGrid->Rows->Clear();
    ColoniesGrid->Rows->Clear();
    ShipsGrid->Rows->Clear();
    AliensGrid->Rows->Clear();

    InitGameData();
}

void Form1::ReloadAutoCommands(Object^, EventArgs^)
{
}

void Form1::RecreatePluginCommands(Object^, EventArgs^)
{
    m_CommandMgr->RemoveGeneratedCommands(CommandOrigin::Plugin, false, false);
    m_CommandMgr->AddPluginCommands();
    m_CommandMgr->SaveCommands();

    UpdateAllGrids(false);
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
        m_CommandMgr->SelectTurn(turn);
        m_PluginMgr->UpdatePlugins();

        // Refresh grids and others
        UpdateControls();

        // Display summary
        Summary->Text = m_GameData->GetSummary();

        this->Text = String::Format("[SP {0}, Turn {1}] Far Horizons User Interface, build {2}",
            GameData::Player->Name,
            GameData::CurrentTurn,
            BuildInfo::Version );

        // Show report for selected turn
        if( m_bHadException == false &&
            RepModeReports->Checked )
        {
            RepTurnNr->Text = "Turn " + turn.ToString();
        }

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

        RepText->Text = m_ReportParser->Reports[key]->GetText();
    }
    else if( RepModeCommands->Checked )
    {
        RepText->Text = m_OrderFiles[ RepTurnNr->SelectedItem->ToString() ];
    }
}

void Form1::LoadOrders()
{
    m_OrderFiles->Clear();

    String^ ordersDir = GetDataDir("orders");
    DirectoryInfo ^dir = gcnew DirectoryInfo(ordersDir);

    if( dir->Exists )
    {
        for each( FileInfo ^f in dir->GetFiles() )
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

    RepModeCommands->Enabled = m_OrderFiles->Count > 0;
}

////////////////////////////////////////////////////////////////
// GUI misc

void Form1::UpdateAllGrids( bool setRefSystems )
{
    if( setRefSystems )
    {
        SystemsGrid->Filter->Update( SystemsRefHome );
        PlanetsGrid->Filter->Update( PlanetsRefHome );
        ColoniesGrid->Filter->Update( ColoniesRefHome );
        ShipsGrid->Filter->Update( ShipsRefHome );
    }
    else
    {
        SystemsGrid->Filter->Update();
        PlanetsGrid->Filter->Update();
        ColoniesGrid->Filter->Update();
        ShipsGrid->Filter->Update();
    }
    AliensGrid->Filter->Update();

    UpdateTabs();
}

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
            sorter->SortColumn = sortBy;
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

    SaveUISettings();
}

void Form1::UpdateSelectionMode(DblBufDGV ^grid, Object ^sender)
{
    CheckBox ^cb = safe_cast<CheckBox^>(sender);

    System::Windows::Forms::DataGridViewSelectionMode selMode;

    if( cb->Checked )
    {
        selMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
        cb->Text = "Select Rows";
    }
    else
    {
        selMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
        cb->Text = "Select Cells";
    }
    grid->Filter->OnGridSelectionChanged();

    if( grid->SelectionMode != selMode )
    {
        grid->SelectionMode = selMode;
        SaveUISettings();
    }
}

Color Form1::GetAlienColor(Alien ^sp)
{
    if( sp == nullptr )
        return Color::White;

    switch( sp->Relation )
    {
    case SP_PLAYER:     return Color::FromArgb(210, 245, 245);
    case SP_NEUTRAL:    return sp->TurnMet > 0 ? Color::FromArgb(245, 245, 195) : Color::FromArgb(235, 235, 190);
    case SP_ALLY:       return Color::FromArgb(220, 245, 200);
    case SP_ENEMY:      return sp->TurnMet > 0 ? Color::FromArgb(245, 210, 210) : Color::FromArgb(235, 205, 205);
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
            Color rowColor = GetAlienColor( iDataSrc->GetAlienForBgColor() );

            // Special ROW coloring rules
            if( grid == PlanetsGrid )
            {
                if( rowColor == Color::White )
                {
                    StarSystem ^system = iDataSrc->GetFilterSystem();
                    // Mark empty planet in alien home system with brighter alien color
                    if( system->HomeSpecies )
                    {
                        rowColor = GetAlienColor( system->HomeSpecies );
                        rowColor = Color::FromArgb(
                            0xff,
                            Math::Min(255, rowColor.R + 25),
                            Math::Min(255, rowColor.G + 25),
                            Math::Min(255, rowColor.B + 25) );
                    }
                }
            }

            for each( DataGridViewCell ^cell in row->Cells )
            {
                Color cellColor(rowColor);

                // Special CELL coloring rules
                if( grid == SystemsGrid )
                {
                    StarSystem ^system = iDataSrc->GetFilterSystem();
                    // Mark wormhole terminus'
                    if( cell->ColumnIndex == m_SystemsColumns.Wormhole )
                    {
                        if( system->HasWormhole &&
                            system->WormholeTargetId != -1 )
                            cellColor = system->WormholeColor;
                    }
                }

                if( String::IsNullOrEmpty(cell->ToolTipText) )
                    cell->ToolTipText = tooltip;
                cell->Style->BackColor = cellColor;
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

    // Remember if menu respawn needed
    m_LastMenuEventArg = e;
    m_LastMenuGrid = safe_cast<DblBufDGV^>(grid);

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
    if( from->IsWormholeTarget(to) )
    {
        return "Wormhole";
    }

    double distance = from->CalcDistance(to);
    double mishap = from->CalcMishap(to, gv, age);

    return String::Format("{0:F1}  ({1:F1}%)", distance, mishap);
}

void Form1::MenuCommandMoveUp(MenuCommandUpDownData ^data)
{
    List<ICommand^> ^commands = data->A;
    ICommand ^cmd = data->B;

    int i = commands->IndexOf(cmd);
    if( i > 0 )
    {
        int newPos = i - 1;
        while( newPos && commands[newPos]->GetPhase() != cmd->GetPhase() )
            --newPos;
        commands->RemoveAt(i);
        commands->Insert(newPos, cmd);
        m_CommandMgr->SaveCommands();
        m_LastMenuGrid->Filter->Update();
    }
}

void Form1::MenuCommandMoveDown(MenuCommandUpDownData ^data)
{
    List<ICommand^> ^commands = data->A;
    ICommand ^cmd = data->B;

    int i = commands->IndexOf(cmd);
    if( i < ( commands->Count - 1) )
    {
        int newPos = i + 1;
        while( newPos < commands->Count &&
                commands[newPos]->GetPhase() != cmd->GetPhase() )
            ++newPos;
        commands->RemoveAt(i);
        commands->Insert(newPos, cmd);
        m_CommandMgr->SaveCommands();
        m_LastMenuGrid->Filter->Update();
    }
}

////////////////////////////////////////////////////////////////
// Orders

void Form1::CopyOrdersTemplateToClipboard()
{
    Clipboard::SetText(OrderTemplate->Text, TextDataFormat::Text);
}

void Form1::SaveOrdersTemplateToFile()
{
    try
    {
        String^ filename = String::Format("orders\\FHUI_orders_t{0:000}.txt", GameData::CurrentTurn);
        StreamWriter ^sw = File::CreateText( GetDataDir(filename) );

        for each( String ^line in OrderTemplate->Lines )
            sw->WriteLine(line);

        sw->Close();
    }
    catch( SystemException^ e )
    {
        MessageBox::Show(
            this,
            "Error generating orders template file: " + e->Message,
            "Save orders",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
