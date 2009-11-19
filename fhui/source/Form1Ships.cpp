#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

#include "CmdCustomDlg.h"
#include "CmdTransferDlg.h"

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
    filter->GridFill += gcnew GridFillHandler(this, &Form1::ShipsFillGrid);
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
    m_CommandMgr->GenerateTemplate(nullptr);

    ShipsGrid->FullUpdateBegin();

    ShipsColumns %c = m_ShipsColumns;

    Alien ^sp = GameData::Player;
    int gv = sp->TechLevelsAssumed[TECH_GV];
    int ml = sp->TechLevelsAssumed[TECH_ML];
    double discount = Calculators::ShipMaintenanceDiscount(ml);

    for each( Alien ^alien in GameData::GetAliens() )
    {
        for each( Ship ^ship in alien->Ships )
        {
            if( ShipsGrid->Filter->Filter(ship) )
                continue;

            DataGridViewRow ^row = ShipsGrid->Rows[ ShipsGrid->Rows->Add() ];
            DataGridViewCellCollection ^cells = row->Cells;
            cells[c.Object]->Value      = ship;
            cells[c.Owner]->Value       = ship->Owner == sp ? String::Format("* {0}", sp->Name) : ship->Owner->Name;
            cells[c.Class]->Value       = ship->PrintClass();
            cells[c.Name]->Value        = ship->Name;
            if( ship->BuiltThisTurn )
                cells[c.Name]->Value    = ship->Name + " (new)";
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
                cells[c.Cargo]->Value   = ship->PrintCargo(false);
                cells[c.Maint]->Value   = Calculators::ShipMaintenanceCost(ship->Type, ship->Size, ship->SubLight) * discount;
                cells[c.UpgCost]->Value = ship->GetUpgradeCost();
                cells[c.RecVal]->Value  = ship->GetRecycleValue();

                ICommand ^cmd = ship->GetJumpCommand();
                if( cmd )
                {
                    cells[c.JumpTarget]->Value  = cmd->PrintForUI();
                    if( cmd->GetCmdType() == CommandType::Jump &&
                        cmd->GetRefSystem() == nullptr )
                    {
                        cells[c.JumpTarget]->Style->ForeColor = Color::Red;
                    }
                }
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

        if( ship->BuiltThisTurn == false )
        {
            menu->Items->Add( ShipsFillMenuCommands(CommandPhase::PreDeparture) );
            menu->Items->Add( ShipsFillMenuCommands(CommandPhase::Jump) );
            // Can't issue ship related production commands on a ship that made a jump
            if( prodOrderPossible &&
                ship->GetJumpCommand() == nullptr )
            {
                menu->Items->Add( ShipsFillMenuCommands(CommandPhase::Production) );
            }
        }

        ICommand ^cmd = ship->GetProdCommand();
        if( cmd == nullptr ||
            cmd->GetCmdType() != CommandType::RecycleShip )
        {
            menu->Items->Add( ShipsFillMenuCommands(CommandPhase::PostArrival) );
        }
    }
}

ToolStripMenuItem^ Form1::ShipsFillMenuCommands(CommandPhase phase)
{
    String ^title;
    switch( phase )
    {
    case CommandPhase::PreDeparture:
    case CommandPhase::Jump:
    case CommandPhase::Production:
    case CommandPhase::PostArrival:
        title = CmdCustom::PhaseAsString(phase);
        break;

    default:
        throw gcnew FHUIDataImplException("Unsupported command phase: " + ((int)phase).ToString());
    }

    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem(title + ":");
    Ship ^ship = m_ShipsMenuRef;

    bool anyCommand = false;
    bool needSeparator = false;

    // Transfer commands
    StarSystem ^system = ship->System;
    if( phase == CommandPhase::PostArrival )
        system = ship->GetPostArrivalSystem();
    if( system )
    {
        for each( ICommand ^cmd in system->GetTransfers(m_ShipsMenuRef) )
        {
            if( cmd->GetPhase() == phase )
            {
                menu->DropDownItems->Add(
                    ShipsFillMenuCommandsOptions(cmd) );
                anyCommand = true;
            }
        }
        if( anyCommand )
            needSeparator = true;
    }
    // Ship commands
    for each( ICommand ^cmd in m_ShipsMenuRef->Commands )
    {
        if( cmd->GetPhase() == phase )
        {
            if( needSeparator )
            {
                needSeparator = false;
                menu->DropDownItems->Add( gcnew ToolStripSeparator() );
            }
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

    menu->DropDownItems->Add( gcnew ToolStripSeparator );
    if( anyCommand )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Cancel All " + CmdCustom::PhaseAsString(phase),
            phase,
            gcnew EventHandler1Arg<CommandPhase>(this, &Form1::ShipsMenuCommandDelAll) ) );
    }
    menu->DropDownItems->Add( CreateCustomMenuItem(
        "Cancel ALL " + m_ShipsMenuRef->PrintClassWithName() + " orders",
        CommandPhase::Custom,
        gcnew EventHandler1Arg<CommandPhase>(this, &Form1::ShipsMenuCommandDelAll) ) );

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
                gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
        }
    }

    if( ship->System->Planets->Count > 0 )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Deep",
            gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PreDeparture, gcnew ShipCmdDeep(ship))),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );

        // Orbit
        ToolStripMenuItem ^menuOrbit = gcnew ToolStripMenuItem("Orbit:");
        for each( Planet ^planet in ship->System->Planets->Values )
        {
            menuOrbit->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
                "PL " + (String::IsNullOrEmpty(planet->Name) ? "#" + planet->Number.ToString() : planet->Name),
                gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PreDeparture, gcnew ShipCmdOrbit(ship, planet))),
                gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
        }
        menu->DropDownItems->Add( menuOrbit );

        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Land",
            gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PreDeparture, gcnew ShipCmdLand(ship))),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
    }

    // Transfer
    if( ship->System->IsTransferPossible(CommandPhase::PreDeparture, nullptr, ship) )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<CmdTransfer^>(
            "Transfer from " + ship->PrintClassWithName() + "...",
            gcnew CmdTransfer(CommandPhase::PreDeparture, INV_MAX, 0, nullptr, ship, nullptr, nullptr),
            gcnew EventHandler1Arg<CmdTransfer^>(this, &Form1::ShipsMenuProdCommandAddTransfer) ) );
        menu->DropDownItems->Add( CreateCustomMenuItem<CmdTransfer^>(
            "Transfer to " + ship->PrintClassWithName() + "...",
            gcnew CmdTransfer(CommandPhase::PreDeparture, INV_MAX, 0, nullptr, nullptr, nullptr, ship),
            gcnew EventHandler1Arg<CmdTransfer^>(this, &Form1::ShipsMenuProdCommandAddTransfer) ) );
    }

    // Custom command
    if( menu->DropDownItems->Count > 0 )
        menu->DropDownItems->Add( gcnew ToolStripSeparator );
    menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
        "Custom Order...",
        gcnew CustomCmdData(CommandPhase::PreDeparture, nullptr),
        gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ShipsMenuCommandCustom) ) );

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
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
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
                ShipsRef->Text,
                false ) );
            anyJump = true;
        }

        // Colonies
        GameData::Player->Colonies->Sort( gcnew Colony::JumpsComparer(ship->System) );
        for each( Colony ^colony in GameData::Player->Colonies )
        {
            if( colony->System != ship->System )
            {
                jumpMenu->DropDownItems->Add(
                    ShipsMenuCreateJumpItem(
                    ship,
                    colony->System,
                    colony->PlanetNum,
                    "PL " + colony->Name,
                    false ) );
                anyJump = true;
            }
        }

        if( anyJump )
            menu->DropDownItems->Add( jumpMenu );
    }

    // Move
    ToolStripMenuItem ^menuMove = gcnew ToolStripMenuItem("Move:");
    for( int x = -1; x < 2; ++x )
    {
        for( int y = -1; y < 2; ++y )
        {
            for( int z = -1; z < 2; ++z )
            {
                if( ( x == 0 && y == 0 && z != 0 ) ||
                    ( x == 0 && y != 0 && z == 0 ) ||
                    ( x != 0 && y == 0 && z == 0 ) )
                {
                    int mx = ship->System->X + x;
                    int my = ship->System->Y + y;
                    int mz = ship->System->Z + z;
                    menuMove->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
                        String::Format("<{0} {1} {2}>", mx, my, mz),
                        gcnew ShipCommandData(ship, gcnew ShipCmdMove(ship, mx, my, mz)),
                        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
                }
            }
        }
    }
    menu->DropDownItems->Add( menuMove );

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
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
    }

    menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
        "Recycle",
        gcnew ShipCommandData(ship, gcnew ShipCmdRecycle(ship)),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuPostArrivalNew()
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
        "Scan",
        gcnew ShipCommandData(ship, gcnew ShipCmdScan(ship)),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );

    StarSystem ^system = ship->System;
    ICommand ^jumpCmd = ship->GetJumpCommand();
    if( jumpCmd && jumpCmd->GetRefSystem() )
        system = jumpCmd->GetRefSystem();

    if( system->Planets->Count > 0 )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Deep",
            gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PostArrival, gcnew ShipCmdDeep(ship))),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );

        // Orbit
        ToolStripMenuItem ^menuOrbit = gcnew ToolStripMenuItem("Orbit:");
        for each( Planet ^planet in system->Planets->Values )
        {
            menuOrbit->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
                "PL " + (String::IsNullOrEmpty(planet->Name) ? "#" + planet->Number.ToString() : planet->Name),
                gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PostArrival, gcnew ShipCmdOrbit(ship, planet))),
                gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
        }
        menu->DropDownItems->Add( menuOrbit );

        menu->DropDownItems->Add( CreateCustomMenuItem<ShipCommandData^>(
            "Land",
            gcnew ShipCommandData(ship, gcnew CmdPhaseWrapper(CommandPhase::PostArrival, gcnew ShipCmdLand(ship))),
            gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) ) );
    }

    // Transfer
    StarSystem ^postArrivalSystem = ship->GetPostArrivalSystem();
    if( postArrivalSystem &&
        postArrivalSystem->IsTransferPossible(CommandPhase::PostArrival, nullptr, ship) )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<CmdTransfer^>(
            "Transfer from " + ship->PrintClassWithName() + "...",
            gcnew CmdTransfer(CommandPhase::PostArrival, INV_MAX, 0, nullptr, ship, nullptr, nullptr),
            gcnew EventHandler1Arg<CmdTransfer^>(this, &Form1::ShipsMenuProdCommandAddTransfer) ) );
        menu->DropDownItems->Add( CreateCustomMenuItem<CmdTransfer^>(
            "Transfer to " + ship->PrintClassWithName() + "...",
            gcnew CmdTransfer(CommandPhase::PostArrival, INV_MAX, 0, nullptr, nullptr, nullptr, ship),
            gcnew EventHandler1Arg<CmdTransfer^>(this, &Form1::ShipsMenuProdCommandAddTransfer) ) );
    }

    // Custom order
    menu->DropDownItems->Add( gcnew ToolStripSeparator );
    menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
        "Custom Order...",
        gcnew CustomCmdData(CommandPhase::PostArrival, nullptr),
        gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ShipsMenuCommandCustom) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsFillMenuCommandsOptions(ICommand ^cmd)
{
    Ship ^ship = m_ShipsMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem( m_CommandMgr->PrintCommandWithInfo(cmd, 0) );

    List<ICommand^> ^cmdList = ship->Commands;
    if( cmd->GetCmdType() == CommandType::Transfer )
        cmdList = ship->System->Transfers;

    ICommand ^cmdFirst = nullptr;
    ICommand ^cmdLast = nullptr;
    for each( ICommand ^iCmd in cmdList )
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
            gcnew MenuCommandUpDownData(cmdList, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveUp) ) );
    }
    if( cmd != cmdLast )
    {
        needSeparator = true;
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Move down",
            gcnew MenuCommandUpDownData(cmdList, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveDown) ) );
    }

    if( needSeparator )
        menu->DropDownItems->Add( gcnew ToolStripSeparator );

    // Edit command
    if( cmd->GetCmdType() == CommandType::Transfer )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<CmdTransfer^>(
            "Edit...",
            safe_cast<CmdTransfer^>(cmd),
            gcnew EventHandler1Arg<CmdTransfer^>(this, &Form1::ShipsMenuProdCommandAddTransfer) ) );
    }
    if( cmd->GetCmdType() == CommandType::Custom )
    {
        CmdCustom ^cmdCustom = safe_cast<CmdCustom^>(cmd);
        menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
            "Edit...",
            gcnew CustomCmdData(cmdCustom->GetPhase(), cmdCustom),
            gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ShipsMenuCommandCustom) ) );
    }
    else if( cmd->GetPhase() != CommandPhase::Jump )
    {   // Edit as custom
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Edit as Custom...",
            cmd,
            gcnew EventHandler1Arg<ICommand^>(this, &Form1::ShipsMenuCommandEditAsCustom) ) );
    }

    // Cancel order
    menu->DropDownItems->Add( CreateCustomMenuItem(
        "Cancel",
        cmd,
        gcnew EventHandler1Arg<ICommand^>(this, &Form1::ShipsMenuCommandDel) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ShipsMenuCreateJumpItem(
    Ship ^ship, StarSystem ^system, int planetNum, String ^text, bool addFromWhere )
{
    GameData::EvalPreDepartureInventory(ship->System, nullptr, false);

    double mishap = ship->System->CalcMishap(
        system,
        Decimal::ToInt32(TechGV->Value),
        ship->Age );

    String ^itemText;
    ICommand ^cmd;
    String ^cargo = GameData::PrintInventory(ship->CargoPreDeparture);
    if( !String::IsNullOrEmpty(cargo) )
        cargo = "; " + cargo;
    if( ship->System->IsWormholeTarget(system) )
    {
        itemText = String::Format("{0}   (Wormhole) (A{1}{2})",
            text,
            ship->Age,
            cargo);
        cmd = gcnew ShipCmdWormhole(ship, system, planetNum);
    }
    else
    {
        itemText = String::Format("{0}   {1:F2}% (A{2}{3})",
            text,
            mishap,
            ship->Age,
            cargo );
        cmd = gcnew ShipCmdJump(ship, system, planetNum);
    }

    if( addFromWhere )
    {
        itemText += " From " + ship->PrintLocation();

        ICommand ^cmdJumpCurrent = ship->GetJumpCommand();
        if( cmdJumpCurrent )
            itemText += "; (Current: " + cmdJumpCurrent->PrintForUI() + ")";
    }

    ToolStripMenuItem ^menuItem = CreateCustomMenuItem<ShipCommandData^>(
        itemText,
        gcnew ShipCommandData(ship, cmd),
        gcnew EventHandler1Arg<ShipCommandData^>(this, &Form1::ShipsMenuCommandAdd) );

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
                ship->PrintClassWithName(),
                true );
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

void Form1::ShipsMenuCommandAdd(ShipCommandData ^data)
{
    if( data )
        data->A->AddCommand( data->B );

    m_CommandMgr->SaveCommands();

    SystemsGrid->MarkForUpdate();
    ShipsGrid->MarkForUpdate();
    ColoniesGrid->MarkForUpdate();
}

void Form1::ShipsMenuCommandDel(ICommand ^cmd)
{
    if( cmd->GetCmdType() == CommandType::Transfer )
        cmd->GetRefSystem()->Transfers->Remove( cmd );
    else
        m_ShipsMenuRef->DelCommand( cmd );

    m_CommandMgr->SaveCommands();

    SystemsGrid->MarkForUpdate();
    ShipsGrid->MarkForUpdate();
    if( cmd->GetPhase() == CommandPhase::Production )
        ColoniesGrid->MarkForUpdate();

    if( m_ShipsMenuRef->Commands->Count > 0 )
        ShowGridContextMenu(ShipsGrid, m_LastMenuEventArg);
}

void Form1::ShipsMenuCommandDelAll(CommandPhase phase)
{
    String ^phaseStr = "";
    if( phase != CommandPhase::Custom )
        phaseStr = " " + CmdCustom::PhaseAsString(phase);

    System::Windows::Forms::DialogResult result = MessageBox::Show(
        this,
        "Delete ALL" + phaseStr + " orders for " + m_ShipsMenuRef->PrintClassWithName() + "...\r\n"
        "Are you SURE? Undo is NOT possible...",
        "Delete All" + phaseStr,
        MessageBoxButtons::YesNo,
        MessageBoxIcon::Exclamation,
        MessageBoxDefaultButton::Button2);
    if( result != System::Windows::Forms::DialogResult::Yes )
        return;

    // Delete all confirmed
    if( phase == CommandPhase::Custom )
    {
        ShipsMenuCommandDelAllTransfers(CommandPhase::PreDeparture);
        ShipsMenuCommandDelAllTransfers(CommandPhase::PostArrival);
        while( m_ShipsMenuRef->Commands->Count > 0 )
            m_ShipsMenuRef->DelCommand( m_ShipsMenuRef->Commands[0] );
    }
    else
    {
        bool repeat;
        do
        {
            repeat = false;
            for each( ICommand ^cmd in m_ShipsMenuRef->Commands )
            {
                if( cmd->GetPhase() == phase )
                {
                    m_ShipsMenuRef->DelCommand(cmd);
                    repeat = true;
                    break;
                }
            }
        } while( repeat );

        if( phase == CommandPhase::PreDeparture )
            ShipsMenuCommandDelAllTransfers(CommandPhase::PreDeparture);
        if( phase == CommandPhase::PostArrival )
            ShipsMenuCommandDelAllTransfers(CommandPhase::PostArrival);
    }

    m_CommandMgr->SaveCommands();

    SystemsGrid->MarkForUpdate();
    ColoniesGrid->MarkForUpdate();
    ShipsGrid->MarkForUpdate();
}

void Form1::ShipsMenuCommandDelAllTransfers(CommandPhase phase)
{
    StarSystem ^system = m_ShipsMenuRef->System;
    if( phase == CommandPhase::PostArrival )
        system = m_ShipsMenuRef->GetPostArrivalSystem();
    if( system )
    {
        for each( ICommand ^cmd in system->GetTransfers(m_ShipsMenuRef) )
        {
            if( cmd->GetPhase() == phase )
                system->Transfers->Remove(cmd);
        }
    }
}

void Form1::ShipsMenuProdCommandAddTransfer(CmdTransfer ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::PreDeparture )
        GameData::EvalPreDepartureInventory(cmd->GetRefSystem(), cmd, true);
    else if( cmd->GetPhase() == CommandPhase::PostArrival )
        GameData::EvalPostArrivalInventory(cmd->GetRefSystem(), cmd);
    else
        throw gcnew FHUIDataIntegrityException("Invalid phase for transfer command!");

    CmdTransferDlg ^dlg = gcnew CmdTransferDlg( cmd );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        if( cmd->m_Type != INV_MAX )
        {
            int amount = dlg->GetAmount(cmd->m_Type);
            if( amount != cmd->m_Amount )
            {
                if( amount <= 0 )
                    ShipsMenuCommandDel(cmd);
                else
                {
                    cmd->m_Amount = amount;
                    cmd->Origin = CommandOrigin::GUI;
                    ShipsMenuCommandAdd(nullptr);
                }
            }
        }
        else
        {
            for( int i = INV_RM; i < INV_MAX; ++i )
            {
                InventoryType inv = static_cast<InventoryType>(i);
                cmd = dlg->GetCommand(inv);
                if( cmd )
                    ShipsMenuCommandAdd( gcnew ShipCommandData(m_ShipsMenuRef, cmd) );
            }
        }
    }

    delete dlg;
}

void Form1::ShipsMenuCommandCustom(CustomCmdData ^data)
{
    CmdCustomDlg ^dlg = gcnew CmdCustomDlg( data->B );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        CmdCustom ^cmd = dlg->GetCommand( data->A );
        if( data->B )
        {
            *data->B = cmd;
            ShipsMenuCommandAdd( nullptr );
        }
        else
            ShipsMenuCommandAdd( gcnew ShipCommandData(m_ShipsMenuRef, cmd) );
    }

    delete dlg;
}

void Form1::ShipsMenuCommandEditAsCustom(ICommand ^cmd)
{
    CmdCustom ^customCmd = gcnew CmdCustom(
        cmd->GetPhase(),
        cmd->Print(),
        cmd->GetEUCost());
    CmdCustomDlg ^dlg = gcnew CmdCustomDlg( customCmd );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        customCmd = dlg->GetCommand( cmd->GetPhase() );

        m_ShipsMenuRef->Commands->Insert(
            m_ShipsMenuRef->Commands->IndexOf(cmd),
            customCmd );
        m_ShipsMenuRef->DelCommand( cmd );
        ShipsMenuCommandAdd( nullptr );
    }

    delete dlg;
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
