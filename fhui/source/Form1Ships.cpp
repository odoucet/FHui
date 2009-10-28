#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

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
    c.DistSec   = ADD_COLUMN("Dist Prev",   "Distance to previous ref system and mishap chance [%]", String, Ascending, DistanceSec);
    c.Cargo     = ADD_COLUMN("Cargo",       "Cargo on board",           String, Ascending,  Default);
    c.Maint     = ADD_COLUMN("Maint",       "Maintenance cost",         double, Ascending,  Default);
    c.UpgCost   = ADD_COLUMN("Upg",         "Upgrade cost to age 0",    int,    Ascending,  Default);
    c.RecVal    = ADD_COLUMN("Rec",         "Recycle value",            int,    Descending, Default);
    c.JumpTarget= ADD_COLUMN("Jump",        "Jump target",              String, Ascending,  Default);
    c.Commands  = ADD_COLUMN("Cmds",        "Ship commands",            String, Ascending,  Default);

    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->AddColumns(GridType::Ships, sorter);
#undef ADD_COLUMN

    // Formatting
    ApplyDataAndFormat(ShipsGrid, nullptr, c.Object, c.Class, sorter);
    for each( IGridPlugin ^plugin in PluginManager::GridPlugins )
        plugin->GridFormat(GridType::Ships, ShipsGrid);

    ShipsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment     = DataGridViewContentAlignment::MiddleRight;
    ShipsGrid->Columns[c.DistSec]->DefaultCellStyle->Alignment  = DataGridViewContentAlignment::MiddleRight;
    ShipsGrid->Columns[c.Cargo]->DefaultCellStyle->Font         = m_GridFontSmall;
    ShipsGrid->Columns[c.JumpTarget]->DefaultCellStyle->Font    = m_GridFontSmall;
    ShipsGrid->Columns[c.Commands]->DefaultCellStyle->Font      = m_GridFontSmall;
    ShipsGrid->Columns[c.DistSec]->Visible = false;

    GridFilter ^filter = gcnew GridFilter(ShipsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ShipsFillGrid);
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

    filter->ResetControls(false);
}

void Form1::ShipsUpdateControls()
{
    // Inhibit grid update
    ShipsGrid->Filter->EnableUpdates = false;

    ShipsRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ShipsRefHome->DataSource     = m_RefListHomes;
    ShipsRefColony->DataSource   = m_RefListColonies;
    ShipsRefShip->DataSource     = m_RefListShips;

    ShipsGrid->Sorter->GroupBySpecies = ColoniesGroupByOwner->Checked;

    // Enable grid update
    ShipsGrid->Filter->EnableUpdates = true;
}

void Form1::ShipsFillGrid()
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
        if( ship->HadMishap )
            cells[c.Location]->Style->ForeColor = Color::Red;
        if( !ship->IsPirate )
            cells[c.Age]->Value     = ship->Age;
        cells[c.Cap]->Value         = ship->Capacity;
        cells[c.Dist]->Value        = GridPrintDistance(ship->System, ShipsGrid->Filter->RefSystem, gv, ship->Age);
        cells[c.DistSec]->Value     = GridPrintDistance(ship->System, ShipsGrid->Filter->RefSystemPrev, gv, ship->Age);
        if( sp == ship->Owner )
        {
            cells[c.Cargo]->Value   = ship->PrintCargo();
            cells[c.Maint]->Value   = Calculators::ShipMaintenanceCost(ship->Type, ship->Size, ship->SubLight) * discount;
            cells[c.UpgCost]->Value = ship->GetUpgradeCost();
            cells[c.RecVal]->Value  = ship->GetRecycleValue();

            ICommand ^cmd = ship->GetJumpCommand();
            if( cmd )
                cells[c.JumpTarget]->Value  = cmd->PrintForUI();
            cells[c.Commands]->Value        = ship->PrintCmdSummary();

            String ^cmdDetails = ship->PrintCmdDetails();
            cells[c.JumpTarget]->ToolTipText= cmdDetails;
            cells[c.Commands]->ToolTipText  = cmdDetails;

            if( ship->EUToComplete )
                cells[c.Commands]->Value    = ship->EUToComplete.ToString() + " EU to complete";
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

        menu->Items->Add( ShipsFillMenuCommands(CommandPhase::PreDeparture) );
        menu->Items->Add( ShipsFillMenuCommands(CommandPhase::Jump) );
        if( prodOrderPossible )
            menu->Items->Add( ShipsFillMenuCommands(CommandPhase::Production) );
        menu->Items->Add( ShipsFillMenuCommands(CommandPhase::PostArrival) );
    }
}

ToolStripMenuItem^ Form1::ShipsFillMenuCommands(CommandPhase phase)
{
    String ^title;
    switch( phase )
    {
    case CommandPhase::PreDeparture:
        title = "Pre-Departure";
        break;

    case CommandPhase::Jump:
        title = "Jumps";
        break;

    case CommandPhase::Production:
        title = "Production";
        break;

    case CommandPhase::PostArrival:
        title = "Post-Arrival";
        break;

    default:
        throw gcnew FHUIDataImplException("Unsupported command phase: " + ((int)phase).ToString());
    }

    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem(title + ":");

    bool anyCommand = false;
    for each( ICommand ^cmd in m_ShipsMenuRef->Commands )
    {
        if( cmd->GetPhase() == phase )
        {
            menu->DropDownItems->Add(
                ShipsFillMenuCommandsOptions(cmd) );
            anyCommand = true;
        }
    }
    if( !anyCommand )
    {
        menu->DropDownItems->Add( "< No " + title + " orders >" );
        menu->DropDownItems[0]->Enabled = false;
    }

    // Add new commands
    menu->DropDownItems->Add( gcnew ToolStripSeparator );
    switch( phase )
    {
    case CommandPhase::PreDeparture:
        menu->DropDownItems->Add( ShipsFillMenuPreDepartureNew() );
        break;

    case CommandPhase::Jump:
        menu->DropDownItems->Add( ShipsFillMenuJumpsNew() );
        break;

    case CommandPhase::Production:
        menu->DropDownItems->Add( ShipsFillMenuProductionNew() );
        break;

    case CommandPhase::PostArrival:
        menu->DropDownItems->Add( ShipsFillMenuPostArrivalNew() );
        break;
    }

    if( anyCommand )
    {
        menu->DropDownItems->Add( gcnew ToolStripSeparator );
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Cancel All",
            static_cast<ICommand^>(nullptr),
            gcnew EventHandler1Arg<ICommand^>(this, &Form1::ShipsMenuCommandDel) ) );
    }

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuPreDepartureNew()
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    if( ship->Cargo[INV_CU] > 0 ||
        ship->Cargo[INV_AU] > 0 ||
        ship->Cargo[INV_IU] > 0 )
    {
        bool suitablePlanet = false;
        for each( Planet ^planet in ship->System->Planets->Values )
        {
            if( String::IsNullOrEmpty(planet->Name) == false )
            {
                suitablePlanet = true;
                break;
            }
        }
        if( suitablePlanet )
        {
            menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
                "Unload",
                gcnew ShipCommandData(ship, gcnew ShipCmdUnload(ship)),
                gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) ) );
        }
    }

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuJumpsNew()
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    if( ship->System->HasWormhole )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Enter Wormhole to " + ship->System->PrintWormholeTarget(),
            gcnew ShipCommandData(ship, gcnew ShipCmdWormhole(ship, ship->System->GetWormholeTarget(), -1)),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) ) );
    }

    if( ship->CanJump )
    {
        ToolStripMenuItem ^jumpMenu = gcnew ToolStripMenuItem("Jump to:");
        bool anyJump = false;

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

        if( anyJump )
            menu->DropDownItems->Add( jumpMenu );
    }

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuProductionNew()
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    if( ship->EUToComplete == 0 )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Upgrade",
            gcnew ShipCommandData(ship, gcnew ShipCmdUpgrade(ship)),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) ) );
    }

    menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
        "Recycle",
        gcnew ShipCommandData(ship, gcnew ShipCmdRecycle(ship)),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuPostArrivalNew()
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
        "Scan",
        gcnew ShipCommandData(ship, gcnew ShipCmdScan(ship)),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuCommandsOptions(ICommand ^cmd)
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem( m_CommandMgr->PrintCommandWithInfo(cmd, 0) );

    ICommand ^cmdFirst = nullptr;
    ICommand ^cmdLast = nullptr;
    for each( ICommand ^iCmd in ship->Commands )
    {
        if( iCmd->GetPhase() == cmd->GetPhase() )
        {
            if( cmdFirst == nullptr )
                cmdFirst = iCmd;
            cmdLast = iCmd;
        }
    }

    bool needSeparator = false;
    if( cmd != cmdFirst )
    {
        needSeparator = true;
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Move up",
            gcnew MenuCommandUpDownData(ship->Commands, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveUp) ) );
    }
    if( cmd != cmdLast )
    {
        needSeparator = true;
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Move down",
            gcnew MenuCommandUpDownData(ship->Commands, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveDown) ) );
    }

    if( needSeparator )
        menu->DropDownItems->Add( gcnew ToolStripSeparator );

    // Edit command
    // none yet...

    // Cancel order
    menu->DropDownItems->Add( CreateCustomMenuItem(
        "Cancel",
        cmd,
        gcnew EventHandler1Arg<ICommand^>(this, &Form1::ShipsMenuCommandDel) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsMenuCreateJumpItem(
    Ship ^ship, StarSystem ^system, int planetNum, String ^text )
{
    double mishap = ship->System->CalcMishap(
        system,
        Decimal::ToInt32(TechGV->Value),
        ship->Age );

    String ^itemText;
    ICommand ^cmd;
    String ^cargo = String::IsNullOrEmpty( ship->PrintCargo() ) ? "" : "; " + ship->PrintCargo();
    if( ship->System->IsWormholeTarget(system) )
    {
        itemText = String::Format("{0}   (Wormhole) (A{2}{3}) From {1}",
            text,
            ship->Age,
            cargo,
            ship->PrintLocation() );
        cmd = gcnew ShipCmdWormhole(ship, system, planetNum);
    }
    else
    {
        itemText = String::Format("{0}   {1:F2}% (A{2}{3})  From {4}",
            text,
            mishap,
            ship->Age,
            cargo,
            ship->PrintLocation() );
        cmd = gcnew ShipCmdJump(ship, system, planetNum);
    }

    ToolStripMenuItem ^menuItem = CreateCustomMenuItem<ShipCommandData^>(
        itemText,
        gcnew ShipCommandData(ship, cmd),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandSet) );

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

void Form1::ShipsMenuCommandSet(ShipCommandData ^data)
{
    data->A->AddCommand( data->B );
    SystemsGrid->Filter->Update();
    ShipsGrid->Filter->Update();
    m_CommandMgr->SaveCommands();
}

void Form1::ShipsMenuCommandDel(ICommand ^cmd)
{
    m_ShipsMenuRef->Commands->Remove( cmd );
    SystemsGrid->Filter->Update();
    ShipsGrid->Filter->Update();
    m_CommandMgr->SaveCommands();
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
