#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

using namespace System::IO;

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

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
    c.DistSec  = ADD_COLUMN("Dist Prev",    "Distance to previous ref system and mishap chance [%]", String, Ascending, DistanceSec);
    c.Visited  = ADD_COLUMN("Vis",          "Last turn you visited this system", int, Descending, Default);
    c.Scan     = ADD_COLUMN("Scan",         "System scan source",   String,     Ascending,  Default);
    c.Wormhole = ADD_COLUMN("WH",           "Wormhole target",      String,     Ascending,  Default);
    c.Jumps    = ADD_COLUMN("Jmp",          "Ships jumping to this system", String, Ascending, Default);
    c.Colonies = ADD_COLUMN("Colonies",     "Summary of colonies and named planets", String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Systems, sorter);

    c.Notes    = ADD_COLUMN("Notes", "Notes", String, Ascending,  Default);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(SystemsGrid, nullptr, c.Object, c.LSNAvail, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Systems, SystemsGrid);

    SystemsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment   = DataGridViewContentAlignment::MiddleRight;
    SystemsGrid->Columns[c.DistSec]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;

    SystemsGrid->Columns[c.Colonies]->DefaultCellStyle->Font    = m_GridFontSmall;
    SystemsGrid->Columns[c.Jumps]->DefaultCellStyle->Font       = m_GridFontSmall;
    SystemsGrid->Columns[c.Scan]->DefaultCellStyle->Font        = m_GridFontSmall;
    SystemsGrid->Columns[c.Notes]->DefaultCellStyle->Font       = m_GridFontSmall;
    SystemsGrid->Columns[c.DistSec]->Visible = false;

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(SystemsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::SystemsFillGrid);
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
    filter->CtrlFiltScanK   = SystemsFiltScanK;
    filter->CtrlFiltScanU   = SystemsFiltScanU;
    filter->CtrlFiltColC    = SystemsFiltColC;
    filter->CtrlFiltColN    = SystemsFiltColN;
    filter->CtrlNumRows     = SystemsNumRows;

    // Store objects
    SystemsGrid->Filter = filter;
    SystemsGrid->Sorter = sorter;

    filter->ResetControls(false);
}

void Form1::SystemsUpdateControls()
{
    // Inhibit grid update
    SystemsGrid->Filter->EnableUpdates  = false;

    SystemsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    SystemsRefHome->DataSource      = m_RefListHomes;
    SystemsRefColony->DataSource    = m_RefListColonies;
    SystemsRefShip->DataSource      = m_RefListShips;

    SystemsRefHome->Text            = GameData::Player->Name;

    // Enable grid update
    SystemsGrid->Filter->EnableUpdates  = true;
}

void Form1::SystemsFillGrid()
{
    SystemsGrid->FullUpdateBegin();

    SystemsColumns %c = m_SystemsColumns;

    int gv  = Decimal::ToInt32(TechGV->Value);
    int age = Decimal::ToInt32(SystemsShipAge->Value);

    for each( StarSystem ^system in m_GameData->GetStarSystems() )
    {
        if( system->IsVoid ||   // Never show void systems on the grid
            SystemsGrid->Filter->Filter(system) )
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
        cells[c.DistSec]->Value     = GridPrintDistance(system, SystemsGrid->Filter->RefSystemPrev, gv, age);
        cells[c.Scan]->Value        = system->PrintScanStatus();
        if( system->HasWormhole )
            cells[c.Wormhole]->Value= system->PrintWormholeTarget();
        if( system->LastVisited != -1 )
            cells[c.Visited]->Value = system->LastVisited;
        cells[c.Colonies]->Value    = system->PrintColoniesAll();
        cells[c.Notes]->Value       = system->PrintComment();

        // Ship jumps
        String ^jumpsCell = "";
        String ^jumpsToolTip = "";
        int jumpsCnt = 0;
        for each( Ship ^ship in GameData::Player->Ships )
        {
            ICommand ^cmd = ship->GetJumpCommand();
            if( cmd == nullptr )
                continue;
            StarSystem ^jumpTarget = cmd->GetRefSystem();

            if( system == jumpTarget )
            {
                ++jumpsCnt;
                if( jumpsCnt > 1 )
                    jumpsCell = jumpsCnt.ToString() + " Ships";
                else
                    jumpsCell = ship->PrintClassWithName();

                double mishap = ship->System->CalcMishap(
                    jumpTarget,
                    Decimal::ToInt32(TechGV->Value),
                    ship->Age );
                
                jumpsToolTip += String::Format("{0} from {1}; {2:F2}%\r\n",
                    ship->PrintClassWithName(),
                    ship->PrintLocation(),
                    mishap );
            }
        }
        if( !String::IsNullOrEmpty(jumpsCell) )
        {
            cells[c.Jumps]->Value = jumpsCell;
            cells[c.Jumps]->ToolTipText = jumpsToolTip;
        }

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

    if( system->TurnScanned <= 0 && !system->IsMarkedVisited )
    {
        ToolStripMenuItem ^menuItem = gcnew ToolStripMenuItem(
            "Mark as Visited",
            nullptr,
            gcnew EventHandler(this, &Form1::SystemsMenuMarkVisited) );
        for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
        {
            if( cmd->GetCmdType() == CommandType::Visited &&
                cmd->GetRefSystem() == system )
            {
                menuItem->Checked = true;
                break;
            }
        }

        menu->Items->Add( menuItem );
    }

    // Export selected systems' scans
    menu->Items->Add( gcnew ToolStripSeparator );
    menu->Items->Add(
        "Export Scans...",
        nullptr,
        gcnew EventHandler(this, &Form1::SystemsMenuExportScans));
    if( SystemsGrid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::CellSelect )
        menu->Items[ menu->Items->Count - 1 ]->Enabled = false;
}

void Form1::SystemsMenuMarkVisited(Object^, EventArgs^)
{
    for each( ICommand ^cmd in m_CommandMgr->GetCommands() )
    {
        if( cmd->GetCmdType() == CommandType::Visited &&
            cmd->GetRefSystem() == m_SystemsMenuRef )
        {
            m_CommandMgr->DelCommand(cmd);
            return;
        }
    }

    m_CommandMgr->AddCommand( gcnew CmdVisited(m_SystemsMenuRef) );
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

void Form1::SystemsMenuExportScans(Object^, EventArgs^)
{
    List<StarSystem^> ^systems = gcnew List<StarSystem^>;

    for each( DataGridViewRow ^row in SystemsGrid->SelectedRows )
    {
        IGridDataSrc ^iDataSrc = safe_cast<IGridDataSrc^>(row->Cells[m_SystemsColumns.Object]->Value);
        StarSystem ^system = iDataSrc->GetFilterSystem();
        if( system->TurnScanned >= 0 )
            systems->Add( system );
    }

    if( systems->Count == 0 )
    {
        MessageBox::Show(
            this,
            "No scanned systems selected.",
            "Export Scans",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        return;
    }

    try
    {
        String^ filename = "scans_" + GameData::Player->Name + ".t" + GameData::CurrentTurn;
        StreamWriter ^sw = File::CreateText( GetDataDir(filename) );

        // Header to facilitate parsing the file with scans
        sw->WriteLine( "" );
        sw->WriteLine( "\t\t\tEVENT LOG FOR TURN 0" );
        sw->WriteLine( "" );

        for each( StarSystem ^system in systems )
        {
            sw->WriteLine( system->GenerateScan(true) );
        }
        sw->Close();
    }
    catch( SystemException^ e )
    {
        MessageBox::Show(
            this,
            "Error generating scan file: " + e->Message,
            "Export Scans",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
    }
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
