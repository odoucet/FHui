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
        LoadOrders();
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
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::SystemsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = SystemsRef;
    filter->CtrlRefHome     = SystemsRefHome;
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

    m_SystemsFilter = filter;

    // -- planets
    filter = gcnew GridFilter(PlanetsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::PlanetsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = PlanetsRef;
    filter->CtrlRefHome     = PlanetsRefHome;
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

    m_PlanetsFilter = filter;

    // -- colonies
    filter = gcnew GridFilter(ColoniesGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ColoniesSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = ColoniesRef;
    filter->CtrlRefHome     = ColoniesRefHome;
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

    m_ColoniesFilter = filter;

    // -- ships
    filter = gcnew GridFilter(ShipsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ShipsSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlRef         = ShipsRef;
    filter->CtrlRefHome     = ShipsRefHome;
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

    m_ShipsFilter = filter;

    // -- aliens
    filter = gcnew GridFilter(AliensGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::AliensSetup);
    filter->GridException += gcnew GridExceptionHandler(this, &Form1::ShowException);

    filter->CtrlFiltRelA    = AliensFiltRelA;
    filter->CtrlFiltRelE    = AliensFiltRelE;
    filter->CtrlFiltRelN    = AliensFiltRelN;
    filter->CtrlFiltRelP    = AliensFiltRelP;

    m_AliensFilter = filter;
}

void Form1::InitData()
{
    m_RM = gcnew RegexMatcher;

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

    TechLevelsResetToCurrent();

    SystemsUpdateControls();
    PlanetsUpdateControls();
    ColoniesUpdateControls();
    ShipsUpdateControls();
    AliensUpdateControls();
}

void Form1::UpdateTabs()
{
    if( MenuTabs->SelectedIndex == TabIndex::Map )
    {
        MapDraw();
    }

    if( MenuTabs->SelectedIndex == TabIndex::Orders )
    {
        GenerateTemplate();
    }
}

void Form1::TechLevelsResetToCurrent()
{
    Alien ^sp = m_GameData->GetSpecies();

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
    Alien ^sp = m_GameData->GetSpecies();

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
    Alien ^sp = m_GameData->GetSpecies();

    sp->TechLevelsAssumed[TECH_MI] = Decimal::ToInt32(TechMI->Value);
    sp->TechLevelsAssumed[TECH_MA] = Decimal::ToInt32(TechMA->Value);
    sp->TechLevelsAssumed[TECH_ML] = Decimal::ToInt32(TechML->Value);
    sp->TechLevelsAssumed[TECH_GV] = Decimal::ToInt32(TechGV->Value);
    sp->TechLevelsAssumed[TECH_LS] = Decimal::ToInt32(TechLS->Value);
    sp->TechLevelsAssumed[TECH_BI] = Decimal::ToInt32(TechBI->Value);

    if( *m_bGridUpdateEnabled )
    {
        m_SystemsFilter->Update();
        m_PlanetsFilter->Update();
        m_ColoniesFilter->Update();
        m_ShipsFilter->Update();

        if( MenuTabs->SelectedIndex == TabIndex::Map )
            MapDraw();
        if( MenuTabs->SelectedIndex == TabIndex::Orders )
            GenerateTemplate();
    }
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

String^ Form1::GetDataDir(String ^suffix)
{
    String ^ret = suffix;
    if( !String::IsNullOrEmpty(DataDir) )
        ret = DataDir + "/" + ret;
    return ret;
}

void Form1::LoadGalaxy()
{
    String^ galaxyList = GetDataDir("galaxy_list.txt");

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
    String^ reportsDir = GetDataDir("reports");
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
    System::Windows::Forms::DialogResult result = MessageBox::Show(
        this,
        "Delete ALL FHUI Commands?",
        "Reload Turn",
        MessageBoxButtons::YesNo,
        MessageBoxIcon::Question,
        MessageBoxDefaultButton::Button1);
    if( result == System::Windows::Forms::DialogResult::Yes )
    {
        m_GameTurns->Remove( m_GameData->GetLastTurn() );
        DeleteCommands();

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
        UpdateTabs();
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

    LoadCommands();
}

int Form1::CheckReport(String ^fileName)
{
    Report ^report = gcnew Report(nullptr, m_RM); // turn scan mode

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
    Report ^report = gcnew Report(m_GameData, m_RM);

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

void Form1::LoadOrders()
{
    m_CmdFiles->Clear();

    try
    {
        String^ ordersDir = GetDataDir("orders");
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
    m_AllPlugins = gcnew List<IPluginBase^>;
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
                        m_AllPlugins->Add(plugin);
                        
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

////////////////////////////////////////////////////////////////
// Systems

void Form1::SystemsUpdateControls()
{
    // Inhibit grid update
    m_SystemsFilter->EnableUpdates  = false;

    m_SystemsFilter->GameData       = m_GameData;

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

    int gv  = Decimal::ToInt32(TechGV->Value);
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
        MenuTabs->SelectedIndex = TabIndex::Planets;
    }
}

void Form1::SystemsFillMenu(Windows::Forms::ContextMenuStrip ^menu, int rowIndex)
{
    int index = SystemsGrid->Columns[0]->Index;
    StarSystem ^system  = safe_cast<StarSystem^>(SystemsGrid->Rows[ rowIndex ]->Cells[index]->Value);
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
    m_SystemsFilter->SetRefSystem(m_SystemsMenuRef);
}

////////////////////////////////////////////////////////////////
// Planets

void Form1::PlanetsUpdateControls()
{
    // Inhibit grid update
    m_PlanetsFilter->EnableUpdates  = false;

    m_PlanetsFilter->GameData       = m_GameData;
    m_PlanetsFilter->DefaultLSN     = Math::Min(99, m_GameData->GetSpecies()->TechLevelsAssumed[TECH_LS]);

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

    int gv  = Decimal::ToInt32(TechGV->Value);
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
            row[colName]     = planet->GetNameWithOrders();
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
                String ^note = planet->PrintComment();
                if ( !String::IsNullOrEmpty(note) )
                {
                    note += "; ";
                }
                note += String::Format("SP {0} home system", planet->System->HomeSpecies->Name );
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
    m_PlanetsFilter->SetRefSystem(m_PlanetsMenuRef->System);
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
        for each( Planet ^planet in system->GetPlanets() )
            if( planet &&
                !String::IsNullOrEmpty(planet->Name) &&
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
        AddCommand( gcnew CmdPlanetName(
            m_PlanetsMenuRef->System,
            m_PlanetsMenuRef->Number,
            name ) );

        m_PlanetsMenuRef->AddName(name);
    }

    m_PlanetsFilter->Update();
}

void Form1::PlanetsMenuRemoveName(Object^, EventArgs^)
{
    if( m_PlanetsMenuRef->NameIsNew )
    {   // Remove Name command
        for each( ICommand ^iCmd in m_GameData->GetCommands() )
        {
            if( iCmd->GetType() == CommandType::Name )
            {
                CmdPlanetName ^cmd = safe_cast<CmdPlanetName^>(iCmd);
                if( cmd->m_System == m_PlanetsMenuRef->System &&
                    cmd->m_PlanetNum == m_PlanetsMenuRef->Number )
                {
                    DelCommand(iCmd);
                    break;
                }
            }
        }
    }
    else
    {   // Add Disband command
        AddCommand( gcnew CmdDisband( m_PlanetsMenuRef->Name ) );
    }

    m_PlanetsMenuRef->DelName();

    m_PlanetsFilter->Update();
}

void Form1::PlanetsMenuRemoveNameCancel(Object^, EventArgs^)
{
    // Remove Disband command
    for each( ICommand ^iCmd in m_GameData->GetCommands() )
    {
        if( iCmd->GetType() == CommandType::Disband )
        {
            CmdDisband ^cmd = safe_cast<CmdDisband^>(iCmd);
            if( cmd->m_Name == m_PlanetsMenuRef->Name )
            {
                DelCommand(iCmd);
                m_PlanetsMenuRef->NameIsDisband = false;
                m_PlanetsFilter->Update();
                return;
            }
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
    DataColumn ^colLocation     = dataTable->Columns->Add("Loc.",       String::typeid );
    DataColumn ^colSize         = dataTable->Columns->Add("Size",       double::typeid );
    DataColumn ^colSeen         = dataTable->Columns->Add("Seen",       int::typeid );
    DataColumn ^colProd         = dataTable->Columns->Add("Prod",       int::typeid );
    DataColumn ^colShipyards    = dataTable->Columns->Add("S-yards",    int::typeid );
    DataColumn ^colLSN          = dataTable->Columns->Add("LSN",        int::typeid );
    DataColumn ^colProdPerc     = dataTable->Columns->Add("Pr[%]",      int::typeid );
    DataColumn ^colBalance      = dataTable->Columns->Add("Balance",    String::typeid );
    DataColumn ^colPop          = dataTable->Columns->Add("Pop",        int::typeid );
    DataColumn ^colDist         = dataTable->Columns->Add("Dist",       double::typeid );
    DataColumn ^colMishap       = dataTable->Columns->Add("Mishap %",   String::typeid );
    DataColumn ^colInventory    = dataTable->Columns->Add("Inventory",  String::typeid );
    DataColumn ^colProdOrder    = dataTable->Columns->Add("Order",      int::typeid );

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Colonies, dataTable);

    int gv  = Decimal::ToInt32(TechGV->Value);
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
            row[colProdOrder] = colony->ProductionOrder;
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
        if( colony->Shipyards != -1 )
            row[colShipyards] = colony->Shipyards;

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
    Colony ^colony = safe_cast<Colony^>(ColoniesGrid->Rows[ rowIndex ]->Cells[index]->Value);
    m_ColoniesMenuRef = colony;

    menu->Items->Add(
        "Select ref: PL " + colony->PrintRefListEntry(m_GameData->GetSpecies()),
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuSelectRef) );

    menu->Items->Add(
        "Reset Filters",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesFiltersReset_Click));

    if( colony->Owner == m_GameData->GetSpecies() )
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
            if( colony->GetMaxProductionBudget() > Calculators::ShipyardCost( m_GameData->GetSpecies()->TechLevels[TECH_MA] ) )
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
    ColoniesRefColony->Text = m_ColoniesMenuRef->PrintRefListEntry(m_GameData->GetSpecies());
}

void Form1::ColoniesMenuProdOrderAdjust(int adjustment)
{
    int oldOrder = m_ColoniesMenuRef->ProductionOrder;
    int newOrder = Math::Max(1, oldOrder + adjustment);
    int lastOrder = 0;
    for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
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
    m_GameData->GetSpecies()->SortColoniesByProdOrder();
    m_ColoniesFilter->Update();

    SaveCommands();
}

void Form1::ColoniesMenuProdShipyard(Object^, EventArgs^)
{
    m_ColoniesMenuRef->OrderBuildShipyard = !m_ColoniesMenuRef->OrderBuildShipyard;
    SaveCommands();
}

////////////////////////////////////////////////////////////////
// Ships

void Form1::ShipsUpdateControls()
{
    // Inhibit grid update
    m_ShipsFilter->EnableUpdates = false;

    m_ShipsFilter->GameData      = m_GameData;

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
    DataColumn ^colOrder    = dataTable->Columns->Add("Order",      String::typeid );

    for each( IGridPlugin ^plugin in m_GridPlugins )
        plugin->AddColumns(GridType::Ships, dataTable);

    Alien ^sp = m_GameData->GetSpecies();
    int gv = sp->TechLevelsAssumed[TECH_GV];
    int ml = sp->TechLevelsAssumed[TECH_ML];
    double discount = (100.0 - (ml / 2)) / 100.0;

    for each( Ship ^ship in m_GameData->GetShips() )
    {
        if( m_ShipsFilter->Filter(ship) )
            continue;

        double distance = ship->System->CalcDistance(m_ShipsFilter->RefSystem);
        double mishap   = ship->System->CalcMishap(m_ShipsFilter->RefSystem, gv, ship->Age);

        DataRow^ row = dataTable->NewRow();
        row[colObject]      = ship;
        row[colOwner]       = ship->Owner == sp ? String::Format("* {0}", sp->Name) : ship->Owner->Name;
        row[colClass]       = ship->PrintClass();
        row[colName]        = ship->Name;
        row[colLocation]    = ship->PrintLocation();
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
            if( ship->Command )
                row[colOrder] = ship->Command->Print();
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

    // Ship orders
    if( m_ShipsMenuRef->Owner == m_GameData->GetSpecies() )
    {
        menu->Items->Add( gcnew ToolStripSeparator );

        bool prodOrderPossible = false;
        for each( Colony ^colony in m_ShipsMenuRef->Owner->Colonies )
        {
            if( colony->System == m_ShipsMenuRef->System &&
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

            ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
                "Upgrade",
                gcnew ShipOrderData(
                    m_ShipsMenuRef,
                    gcnew Ship::Order(Ship::OrderType::Upgrade) ),
                 handler );
            if( m_ShipsMenuRef->Command &&
                m_ShipsMenuRef->Command->Type == Ship::OrderType::Upgrade )
            {
                menuItem->Checked = true;
            }
            menu->Items->Add( menuItem );

            menuItem = CreateCustomMenuItem(
                "Recycle",
                gcnew ShipOrderData(
                    m_ShipsMenuRef,
                    gcnew Ship::Order(Ship::OrderType::Recycle) ),
                 handler );
            if( m_ShipsMenuRef->Command &&
                m_ShipsMenuRef->Command->Type == Ship::OrderType::Recycle )
                menuItem->Checked = true;
            menu->Items->Add( menuItem );
        }

        if( m_ShipsMenuRef->CanJump )
        {
            ToolStripMenuItem ^jumpMenu = gcnew ToolStripMenuItem("Jump to:");
            bool anyJump = false;

            if( m_ShipsMenuRef->Command &&
                m_ShipsMenuRef->Command->Type == Ship::OrderType::Jump )
                jumpMenu->Checked = true;

            // Jump to ref system
            if( m_ShipsFilter->RefSystem != m_ShipsMenuRef->System )
            {
                jumpMenu->DropDownItems->Add(
                    ShipsMenuCreateJumpItem(
                        m_ShipsMenuRef,
                        m_ShipsFilter->RefSystem,
                        -1,
                        ShipsRef->Text ) );
                anyJump = true;
            }

            // Colonies
            for each( Colony ^colony in m_GameData->GetSpecies()->Colonies )
            {
                if( colony->System != m_ShipsMenuRef->System )
                {
                    jumpMenu->DropDownItems->Add(
                        ShipsMenuCreateJumpItem(
                            m_ShipsMenuRef,
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
                if( planet->System != m_ShipsMenuRef->System )
                {
                    if( !anyPlanet && anyJump )
                        jumpMenu->DropDownItems->Add( gcnew ToolStripSeparator );

                    jumpMenu->DropDownItems->Add(
                        ShipsMenuCreateJumpItem(
                            m_ShipsMenuRef,
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
        if( m_ShipsMenuRef->Command )
        {
            menu->Items->Add( CreateCustomMenuItem<ShipOrderData^>(
                "Cancel Command",
                gcnew ShipOrderData(m_ShipsMenuRef, nullptr),
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

    String ^itemText = String::Format("{0}   {1:F2}% ({2} FS)  From {3}",
            text,
            mishap,
            ship->Cargo[INV_FS],
            ship->PrintLocation() );

    ToolStripMenuItem ^menuItem = CreateCustomMenuItem(
        itemText,
        gcnew ShipOrderData(ship, gcnew Ship::Order(Ship::OrderType::Jump, system, planetNum)),
        gcnew EventHandler1Arg<ShipOrderData^>(this, &Form1::ShipsMenuOrderSet) );

    // Add checkbox
    if( ship->Command &&
        ship->Command->Type == Ship::OrderType::Jump &&
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

    for each( Ship ^ship in m_GameData->GetSpecies()->Ships )
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
    if( m_ShipsMenuRef->Owner == m_GameData->GetSpecies() )
        ShipsRefShip->Text = m_ShipsMenuRef->PrintRefListEntry();
    else
        m_ShipsFilter->SetRefSystem(m_ShipsMenuRef->System);
}

void Form1::ShipsMenuOrderSet(ShipOrderData ^data)
{
    data->A->Command = data->B;
    m_ShipsFilter->Update();
    SaveCommands();
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
    DataColumn ^colTeach    = dataTable->Columns->Add("Teach",          String::typeid );
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
                row[colTeach] = teach->Substring(2);
        }

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
        gcnew TeachData(m_AliensMenuRef, tech, m_GameData->GetSpecies()->TechLevels[tech]),
        gcnew EventHandler1Arg<TeachData^>(this, &Form1::AliensMenuTeach) );

    return menuItem;
}

void Form1::AliensMenuTeach(TeachData ^data)
{
    Alien ^alien = data->A;
    TechType tech = (TechType)data->B;
    int level = data->C;

    for each( ICommand ^iCmd in m_GameData->GetCommands() )
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

    AddCommand( gcnew CmdTeach(alien, tech, level) );

    alien->TeachOrders |= 1 << tech;
    m_AliensFilter->Update();
}

void Form1::AliensMenuTeachAll(Object^, EventArgs^)
{
    m_AliensFilter->EnableUpdates = false;

    TeachData ^data = gcnew TeachData(m_AliensMenuRef, TECH_MI, m_GameData->GetSpecies()->TechLevels[TECH_MI]);
    AliensMenuTeach(data);

    data->B = TECH_MA;
    data->C = m_GameData->GetSpecies()->TechLevels[TECH_MA];
    AliensMenuTeach(data);

    data->B = TECH_ML;
    data->C = m_GameData->GetSpecies()->TechLevels[TECH_ML];
    AliensMenuTeach(data);

    data->B = TECH_GV;
    data->C = m_GameData->GetSpecies()->TechLevels[TECH_GV];
    AliensMenuTeach(data);

    data->B = TECH_LS;
    data->C = m_GameData->GetSpecies()->TechLevels[TECH_LS];
    AliensMenuTeach(data);

    data->B = TECH_BI;
    data->C = m_GameData->GetSpecies()->TechLevels[TECH_BI];
    AliensMenuTeach(data);

    m_AliensFilter->EnableUpdates = true;
    m_AliensFilter->Update();
}

void Form1::AliensMenuTeachCancel(Object^, EventArgs^)
{
    bool removed = false;
    bool removedAny = false;
    do
    {
        removed = false;
        for each( ICommand ^iCmd in m_GameData->GetCommands() )
        {
            if( iCmd->GetType() == CommandType::Teach )
            {
                CmdTeach ^cmd = safe_cast<CmdTeach^>(iCmd);
                if( cmd->m_Alien == m_AliensMenuRef )
                {
                    removed = removedAny = true;
                    m_GameData->DelCommand(iCmd);
                    break;
                }
            }
        }
    } while( removed );
    if( removedAny )
        SaveCommands();

    m_AliensMenuRef->TeachOrders = 0;
    m_AliensFilter->Update();
}

void Form1::AliensMenuSetRelation(AlienRelationData ^data)
{
    Alien ^alien = data->A;
    SPRelType rel = (SPRelType)data->B;

    if( alien->Relation == alien->RelationOriginal )
    {   // Add relation command
        AddCommand( gcnew CmdAlienRelation(alien, rel) );
    }
    else
    {   // Modify existing relation command
        for each( ICommand ^iCmd in m_GameData->GetCommands() )
        {
            if( iCmd->GetType() == CommandType::AlienRelation )
            {
                CmdAlienRelation ^cmd = safe_cast<CmdAlienRelation^>(iCmd);
                if( cmd->m_Alien == alien )
                {
                    cmd->m_Relation = rel;
                    break;
                }
            }
        }
    }

    if( rel == SP_ENEMY && alien->TeachOrders )
        AliensMenuTeachCancel(nullptr, nullptr);

    alien->Relation = rel;
    m_AliensFilter->Update();
    // Update other grids to reflect new colors
    m_SystemsFilter->Update();
    m_PlanetsFilter->Update();
    m_ColoniesFilter->Update();
    m_ShipsFilter->Update();
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
