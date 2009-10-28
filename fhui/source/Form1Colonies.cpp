#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

#include "CmdResearch.h"
#include "CmdBuildShips.h"
#include "CmdBuildIuAu.h"
#include "CmdDevelopDlg.h"
#include "CmdCustomDlg.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

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
    c.ProdOrder = ADD_COLUMN("#",           "Production order for orders template", int, Ascending, Default);
    c.Budget    = ADD_COLUMN("$",           "Budget after production orders",String,Descending, Default);
    c.Shipyards = ADD_COLUMN("SY",          "Shipyards",                    int,    Descending, Default);
    c.MD        = ADD_COLUMN("MD",          "Mining Difficulty",            double, Ascending,  Default);
    c.Grav      = ADD_COLUMN("Grav",        "Gravitation",                  double, Ascending,  Default);
    c.LSN       = ADD_COLUMN("LSN",         "LSN",                          int,    Ascending,  Default);
    c.Balance   = ADD_COLUMN("IU/AU",       "IU/AU balance",                String, Descending, Default);
    c.Pop       = ADD_COLUMN("Pop",         "Available population",         int,    Descending, Default);
    c.Dist      = ADD_COLUMN("Distance",    "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.DistSec   = ADD_COLUMN("Dist Prev",   "Distance to previous ref system and mishap chance [%]", String, Ascending, DistanceSec);
    c.Inventory = ADD_COLUMN("Inventory",   "Planetary inventory",          String, Ascending, Default);
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
    ColoniesGrid->Columns[c.Inventory]->DefaultCellStyle->Font = m_GridFontSmall;
    ColoniesGrid->Columns[c.Notes]->DefaultCellStyle->Font     = m_GridFontSmall;
    ColoniesGrid->Columns[c.MD]->Visible = false;
    ColoniesGrid->Columns[c.Grav]->Visible = false;
    ColoniesGrid->Columns[c.DistSec]->Visible = false;

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(ColoniesGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::ColoniesFillGrid);
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
    filter->CtrlFiltRelA    = ColoniesFiltRelA;
    filter->CtrlFiltRelE    = ColoniesFiltRelE;
    filter->CtrlFiltRelN    = ColoniesFiltRelN;
    filter->CtrlMiMaBalance = ColoniesMiMaBalanced;
    filter->CtrlNumRows     = ColoniesNumRows;

    // Store objects
    ColoniesGrid->Filter = filter;
    ColoniesGrid->Sorter = sorter;

    filter->ResetControls(false);
}

void Form1::ColoniesUpdateControls()
{
    // Inhibit grid update
    ColoniesGrid->Filter->EnableUpdates = false;

    ColoniesRefXYZ->DataSource      = m_RefListSystemsXYZ;
    ColoniesRefHome->DataSource     = m_RefListHomes;
    ColoniesRefColony->DataSource   = m_RefListColonies;
    ColoniesRefShip->DataSource     = m_RefListShips;

    ColoniesGrid->Sorter->GroupBySpecies = ColoniesGroupByOwner->Checked;

    // Enable grid update
    ColoniesGrid->Filter->EnableUpdates = true;
}

void Form1::ColoniesFillGrid()
{
    m_CommandMgr->GenerateTemplate(nullptr);

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
        cells[c.Dist]->Value        = GridPrintDistance(colony->System, ColoniesGrid->Filter->RefSystem, gv, age);
        cells[c.DistSec]->Value     = GridPrintDistance(colony->System, ColoniesGrid->Filter->RefSystemPrev, gv, age);
        cells[c.Inventory]->Value   = colony->PrintInventory();
        if( colony->Planet )
        {
            cells[c.LSN]->Value     = colony->Planet->LSN;
            cells[c.MD]->Value      = (double)colony->Planet->MiDiff / 100.0;
            cells[c.Grav]->Value    = (double)colony->Planet->Grav / 100.0;
        }

        if( colony->EconomicBase > 0 )
        {
            cells[c.Size]->Value    = (double)colony->EconomicBase / 10.0;

            if( colony->Owner == sp )
            {
                int prodCalculated = Calculators::ColonyProduction(
                    colony,
                    sp->TechLevelsAssumed[TECH_MI],
                    sp->TechLevelsAssumed[TECH_MA],
                    sp->TechLevelsAssumed[TECH_LS],
                    m_GameData->GetFleetPercentCost() );

                cells[c.Prod]->Value        = prodCalculated;
                cells[c.ProdOrder]->Value   = colony->ProductionOrder;
                cells[c.ProdPerc]->Value    = 100 - Calculators::ProductionPenalty(colony->LSN, sp->TechLevelsAssumed[TECH_LS]);
                cells[c.Pop]->Value         = colony->AvailPop;

                if( colony->PlanetType == PLANET_HOME ||
                    colony->PlanetType == PLANET_COLONY )
                {
                    colony->CalculateBalance(ColoniesGrid->Filter->MiMaBalanced);
                    cells[c.Balance]->Value = colony->PrintBalance();
                }

                int prodSum;
                String ^orders = colony->PrintCmdDetails(m_CommandMgr, prodSum);
                cells[c.Budget]->Value = prodSum != 0
                    ? String::Format("{0} ({1}{2})", colony->Res->TotalEU, prodSum < 0 ? "+" : "", -prodSum)
                    :  colony->Res->TotalEU.ToString();
                if( colony->Res->TotalEU < 0 )
                    cells[c.Budget]->Style->ForeColor = Color::Red;

                cells[c.Prod]->ToolTipText      = orders;
                cells[c.ProdOrder]->ToolTipText = orders;
                cells[c.Budget]->ToolTipText    = orders;

                if( colony->Res->AvailEU < 0 )
                    cells[c.Prod]->Style->ForeColor = Color::Red;
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

        menu->Items->Add( ColoniesFillMenuCommands(CommandPhase::PreDeparture) );
        if( colony->CanProduce )
            menu->Items->Add( ColoniesFillMenuCommands(CommandPhase::Production) );
        menu->Items->Add( ColoniesFillMenuCommands(CommandPhase::PostArrival) );
        menu->Items->Add( ColoniesFillMenuAuto() );

        // Production order adjustment
        menu->Items->Add( gcnew ToolStripSeparator );
        menu->Items->Add( ColoniesFillMenuProductionOrder() );

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

ToolStripMenuItem^ Form1::ColoniesFillMenuAuto()
{
    Colony ^colony = m_ColoniesMenuRef;

    ToolStripMenuItem ^autoMenu = gcnew ToolStripMenuItem("Automation:");

    bool autoEnabled = m_CommandMgr->AutoEnabled;
    autoMenu->DropDownItems->Add(
        (autoEnabled ? "Disable" : "Enable") + " AUTO command",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuAutoToggle) );

    autoMenu->DropDownItems->Add( gcnew ToolStripSeparator );

    autoMenu->DropDownItems->Add(
        "Delete all production Auto commands",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuAutoDeleteAllProduction) );
    autoMenu->DropDownItems->Add(
        "Delete all non-scouting Auto commands",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuAutoDeleteAllNonScouting) );
    autoMenu->DropDownItems->Add(
        "Delete all Auto commands",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuAutoDeleteAll) );

    return autoMenu;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuProductionOrder()
{
    Colony ^colony = m_ColoniesMenuRef;

    ToolStripMenuItem ^prodOrder = gcnew ToolStripMenuItem("Prod. Order:");
    if( colony->ProductionOrder > 0 )
    {
        prodOrder->DropDownItems->Add( CreateCustomMenuItem(
            "Prod. Order: First",
            -1000000,
            gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
        prodOrder->DropDownItems->Add( CreateCustomMenuItem(
            "Prod. Order: 1 Up",
            -1,
            gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
    }
    prodOrder->DropDownItems->Add( CreateCustomMenuItem(
        "Prod. Order: 1 Down",
        1,
        gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );
    prodOrder->DropDownItems->Add( CreateCustomMenuItem(
        "Prod. Order: Last",
        1000000,
        gcnew EventHandler1Arg<int>(this, &Form1::ColoniesMenuProdOrderAdjust) ) );

    return prodOrder;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuCommands(CommandPhase phase)
{
    String ^title;
    switch( phase )
    {
    case CommandPhase::PreDeparture:
        title = "Pre-Departure";
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
    for each( ICommand ^cmd in m_ColoniesMenuRef->Commands )
    {
        if( cmd->GetPhase() == phase )
        {
            menu->DropDownItems->Add(
                ColoniesFillMenuCommandsOptions(cmd) );
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
        menu->DropDownItems->Add( ColoniesFillMenuPreDepartureNew() );
        break;

    case CommandPhase::Production:
        menu->DropDownItems->Add( ColoniesFillMenuProductionNew() );
        break;

    case CommandPhase::PostArrival:
        menu->DropDownItems->Add( ColoniesFillMenuPostArrivalNew() );
        break;
    }

    menu->DropDownItems->Add( gcnew ToolStripSeparator );
    if( anyCommand )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Cancel All " + CmdCustom::PhaseAsString(phase),
            phase,
            gcnew EventHandler1Arg<CommandPhase>(this, &Form1::ColoniesMenuCommandDelAll) ) );
    }
    menu->DropDownItems->Add( CreateCustomMenuItem(
        "Cancel ALL PL " + m_ColoniesMenuRef->Name + " orders",
        CommandPhase::Custom,
        gcnew EventHandler1Arg<CommandPhase>(this, &Form1::ColoniesMenuCommandDelAll) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuPreDepartureNew()
{
    Colony ^colony = m_ColoniesMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    // Custom command
    menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
        "Custom Order...",
        gcnew CustomCmdData(CommandPhase::PreDeparture, nullptr),
        gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ColoniesMenuCommandCustom) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuPostArrivalNew()
{
    Colony ^colony = m_ColoniesMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    // Custom command
    menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
        "Custom Order...",
        gcnew CustomCmdData(CommandPhase::PostArrival, nullptr),
        gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ColoniesMenuCommandCustom) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuProductionNew()
{
    Colony ^colony = m_ColoniesMenuRef;
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem("Add:");

    bool bShipyard = true;
    bool bHide = true;

    // Exclude commands that can't or shouldn't be duplicated
    for each( ICommand ^cmd in colony->Commands )
    {
        if( cmd->GetPhase() != CommandPhase::Production )
            continue;

        switch( cmd->GetCmdType() )
        {
        case CommandType::Shipyard:     bShipyard = false; break;
        case CommandType::Hide:         bHide = false; break;
        default:
            break;
        }
    }

    // Hide
    if( bHide )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Hide Colony",
            static_cast<ICommand^>(gcnew ProdCmdHide(colony)),
            gcnew EventHandler1Arg<ICommand^>(this, &Form1::ColoniesMenuCommandAdd) ) );
    }

    // Develop
    menu->DropDownItems->Add( CreateCustomMenuItem<ProdCmdDevelop^>(
        "Develop...",
        nullptr,
        gcnew EventHandler1Arg<ProdCmdDevelop^>(this, &Form1::ColoniesMenuProdCommandAddDevelop) ) );

    // Build CU/IU/AU
    menu->DropDownItems->Add( CreateCustomMenuItem<ProdCmdBuildIUAU^>(
        "Build CU/IU/AU...",
        nullptr,
        gcnew EventHandler1Arg<ProdCmdBuildIUAU^>(this, &Form1::ColoniesMenuProdCommandAddBuildIuAu) ) );

    // Research
    menu->DropDownItems->Add(
        "Research...",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuProdCommandAddResearch) );

    // Build Ship
    menu->DropDownItems->Add(
        "Build Ship...",
        nullptr,
        gcnew EventHandler(this, &Form1::ColoniesMenuProdCommandAddBuildShip) );

    // Shipyard
    if( bShipyard &&
        colony->GetMaxProductionBudget() > Calculators::ShipyardCost( GameData::Player->TechLevels[TECH_MA] ) )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Build Shipyard",
            static_cast<ICommand^>(gcnew ProdCmdShipyard),
            gcnew EventHandler1Arg<ICommand^>(this, &Form1::ColoniesMenuCommandAdd) ) );
    }

    // Estimate
    ToolStripMenuItem ^menuEstimate = gcnew ToolStripMenuItem("Estimate:");
    bool estimateAny = false;
    for each( Alien ^alien in GameData::GetAliens() )
    {
        if( alien->Relation == SP_ALLY ||
            alien->Relation == SP_NEUTRAL ||
            alien->Relation == SP_ENEMY )
        {
            estimateAny = true;

            menuEstimate->DropDownItems->Add( CreateCustomMenuItem(
                alien->Name,
                static_cast<ICommand^>(gcnew ProdCmdEstimate(alien)),
                gcnew EventHandler1Arg<ICommand^>(this, &Form1::ColoniesMenuCommandAdd) ) );
        }
    }
    if( estimateAny )
        menu->DropDownItems->Add(menuEstimate);

    // Custom command
    menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
        "Custom Order...",
        gcnew CustomCmdData(CommandPhase::Production, nullptr),
        gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ColoniesMenuCommandCustom) ) );

    return menu;
}

ToolStripMenuItem^ Form1::ColoniesFillMenuCommandsOptions(ICommand ^cmd)
{
    ToolStripMenuItem ^menu = gcnew ToolStripMenuItem( m_CommandMgr->PrintCommandWithInfo(cmd, 0) );

    ICommand ^cmdFirst = nullptr;
    ICommand ^cmdLast = nullptr;
    for each( ICommand ^iCmd in m_ColoniesMenuRef->Commands )
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
            gcnew MenuCommandUpDownData(m_ColoniesMenuRef->Commands, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveUp) ) );
    }
    if( cmd != cmdLast )
    {
        needSeparator = true;
        menu->DropDownItems->Add( CreateCustomMenuItem(
            "Move down",
            gcnew MenuCommandUpDownData(m_ColoniesMenuRef->Commands, cmd),
            gcnew EventHandler1Arg<MenuCommandUpDownData^>(this, &Form1::MenuCommandMoveDown) ) );
    }

    if( needSeparator )
        menu->DropDownItems->Add( gcnew ToolStripSeparator );

    // Edit command
    if( cmd->GetCmdType() == CommandType::Develop )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ProdCmdDevelop^>(
            "Edit...",
            safe_cast<ProdCmdDevelop^>(cmd),
            gcnew EventHandler1Arg<ProdCmdDevelop^>(this, &Form1::ColoniesMenuProdCommandAddDevelop) ) );
    }
    if( cmd->GetCmdType() == CommandType::BuildIuAu )
    {
        menu->DropDownItems->Add( CreateCustomMenuItem<ProdCmdBuildIUAU^>(
            "Edit...",
            safe_cast<ProdCmdBuildIUAU^>(cmd),
            gcnew EventHandler1Arg<ProdCmdBuildIUAU^>(this, &Form1::ColoniesMenuProdCommandAddBuildIuAu) ) );
    }
    if( cmd->GetCmdType() == CommandType::Custom )
    {
        CmdCustom ^cmdCustom = safe_cast<CmdCustom^>(cmd);
        menu->DropDownItems->Add( CreateCustomMenuItem<CustomCmdData^>(
            "Edit...",
            gcnew CustomCmdData(cmdCustom->GetPhase(), cmdCustom),
            gcnew EventHandler1Arg<CustomCmdData^>(this, &Form1::ColoniesMenuCommandCustom) ) );
    }

    // Cancel order
    menu->DropDownItems->Add( CreateCustomMenuItem(
        "Cancel",
        cmd,
        gcnew EventHandler1Arg<ICommand^>(this, &Form1::ColoniesMenuCommandDel) ) );

    return menu;
}

void Form1::ColoniesMenuSelectRef(Object^, EventArgs ^e)
{
    ColoniesGrid->Filter->SetRefSystem(m_ColoniesMenuRef);
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

void Form1::ColoniesMenuCommandAdd(ICommand ^cmd)
{
    if( cmd )
        m_CommandMgr->AddCommand(m_ColoniesMenuRef, cmd);
    else
        m_CommandMgr->SaveCommands();

    ColoniesGrid->Filter->Update();
    ShowGridContextMenu(ColoniesGrid, m_LastMenuEventArg);
}

void Form1::ColoniesMenuProdCommandAddResearch(Object^, EventArgs^)
{
    CmdResearch ^dlg = gcnew CmdResearch( m_ColoniesMenuRef->Res->AvailEU );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        for( int i = 0; i < TECH_MAX; ++i )
        {
            TechType tech = static_cast<TechType>(i);
            int res = dlg->GetAmount(tech);
            if( res > 0 )
            {
                ColoniesMenuCommandAdd(
                    gcnew ProdCmdResearch(tech, res) );
            }
        }
    }

    delete dlg;
}

void Form1::ColoniesMenuProdCommandAddDevelop(ProdCmdDevelop ^cmd)
{
    CmdDevelopDlg ^dlg = gcnew CmdDevelopDlg(
        m_ColoniesMenuRef, cmd );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        if( cmd == nullptr )
        {
            ColoniesMenuCommandAdd( dlg->GetCommand() );
        }
        else
        {
            *cmd = dlg->GetCommand();
            ColoniesMenuCommandAdd(nullptr);
        }
    }

    delete dlg;
}

void Form1::ColoniesMenuProdCommandAddBuildIuAu(ProdCmdBuildIUAU ^cmd)
{
    CmdBuildIuAu ^dlg = gcnew CmdBuildIuAu(m_ColoniesMenuRef, cmd);
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        ProdCmdBuildIUAU ^newCmd;
        if( cmd )
        {
            if( cmd->m_Unit == INV_CU )
                newCmd = dlg->GetCUCommand();
            else if( cmd->m_Unit == INV_IU )
                newCmd = dlg->GetIUCommand();
            else if( cmd->m_Unit == INV_AU )
                newCmd = dlg->GetAUCommand();
            else
                throw gcnew FHUIDataIntegrityException("Invalid Build CU/IU/AU inventory type: " + ((int)cmd->m_Unit).ToString());
            if( newCmd )
            {
                *cmd = newCmd;
                ColoniesMenuCommandAdd(nullptr);
            }
            else
                ColoniesMenuCommandDel(cmd);
        }
        else
        {

            newCmd = dlg->GetCUCommand();
            if( newCmd )
                ColoniesMenuCommandAdd( newCmd );

            newCmd = dlg->GetIUCommand();
            if( newCmd )
                ColoniesMenuCommandAdd( newCmd );

            newCmd = dlg->GetAUCommand();
            if( newCmd )
                ColoniesMenuCommandAdd( newCmd );
        }
    }

    delete dlg;
}

void Form1::ColoniesMenuProdCommandAddBuildShip(Object^, EventArgs^)
{
    CmdBuildShips ^dlg = gcnew CmdBuildShips(
        m_ColoniesMenuRef->Res->AvailEU,
        GameData::Player->TechLevels[TECH_MA] );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        ColoniesMenuCommandAdd( dlg->CreateCommand() );
    }

    delete dlg;
}

void Form1::ColoniesMenuCommandDel(ICommand ^cmd)
{
    m_CommandMgr->DelCommand(m_ColoniesMenuRef, cmd);

    ColoniesGrid->Filter->Update();

    if( m_ColoniesMenuRef->Commands->Count > 0 )
        ShowGridContextMenu(ColoniesGrid, m_LastMenuEventArg);
}

void Form1::ColoniesMenuCommandDelAll(CommandPhase phase)
{
    String ^phaseStr = "";
    if( phase != CommandPhase::Custom )
        phaseStr = " " + CmdCustom::PhaseAsString(phase);

    System::Windows::Forms::DialogResult result = MessageBox::Show(
        this,
        "Delete ALL" + phaseStr + " orders for PL " + m_ColoniesMenuRef->Name + "...\r\n"
        "Are you SURE? Undo is NOT possible...",
        "Delete All" + phaseStr,
        MessageBoxButtons::YesNo,
        MessageBoxIcon::Exclamation,
        MessageBoxDefaultButton::Button2);
    if( result != System::Windows::Forms::DialogResult::Yes )
        return;

    // Delete all confirmed
    if( phase == CommandPhase::Custom )
        m_ColoniesMenuRef->Commands->Clear();
    else
    {
        bool repeat;
        do
        {
            repeat = false;
            for each( ICommand ^cmd in m_ColoniesMenuRef->Commands )
            {
                if( cmd->GetPhase() == phase )
                {
                    m_ColoniesMenuRef->Commands->Remove(cmd);
                    repeat = true;
                    break;
                }
            }
        } while( repeat );
    }
    m_CommandMgr->SaveCommands();

    ColoniesGrid->Filter->Update();
}

void Form1::ColoniesMenuAutoToggle(Object^, EventArgs^)
{
    m_CommandMgr->AutoEnabled = ! m_CommandMgr->AutoEnabled;
}

void Form1::ColoniesMenuAutoDeleteAll(Object^, EventArgs^)
{
    m_CommandMgr->RemoveGeneratedCommands(CommandOrigin::Auto, false, false);
    m_CommandMgr->SaveCommands();
    ColoniesGrid->Filter->Update();
    ShowGridContextMenu(ColoniesGrid, m_LastMenuEventArg);
}

void Form1::ColoniesMenuAutoDeleteAllNonScouting(Object^, EventArgs^)
{
    m_CommandMgr->RemoveGeneratedCommands(CommandOrigin::Auto, false, true);
    m_CommandMgr->SaveCommands();
    UpdateAllGrids(false);
    ShowGridContextMenu(ColoniesGrid, m_LastMenuEventArg);
}

void Form1::ColoniesMenuAutoDeleteAllProduction(Object^, EventArgs^)
{
    m_CommandMgr->RemoveGeneratedCommands(CommandOrigin::Auto, true, true);
    m_CommandMgr->SaveCommands();
    UpdateAllGrids(false);
    ShowGridContextMenu(ColoniesGrid, m_LastMenuEventArg);
}

void Form1::ColoniesMenuCommandCustom(CustomCmdData ^data)
{
    CmdCustomDlg ^dlg = gcnew CmdCustomDlg( data->B );
    if( dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK )
    {
        CmdCustom ^cmd = dlg->GetCommand( data->A );
        if( data->B )
        {
            *data->B = cmd;
            ColoniesMenuCommandAdd( nullptr );
        }
        else
            ColoniesMenuCommandAdd( cmd );
    }

    delete dlg;
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
