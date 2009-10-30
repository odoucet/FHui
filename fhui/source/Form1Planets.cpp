#include "StdAfx.h"
#include "Form1.h"

#include "GridFilter.h"
#include "GridSorter.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

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
    c.Grav      = ADD_COLUMN("Grav",        "Gravitation",          double,     Ascending,  Default);
    c.LSN       = ADD_COLUMN("LSN",         "LSN",                  int,        Ascending,  Default);
    c.AlienLSN  = ADD_COLUMN("AlienLSN",    "Alien LSN",            int,        Ascending,  Default);
    c.Dist      = ADD_COLUMN("Distance",    "Distance to ref system and mishap chance [%]", String, Ascending, Distance);
    c.DistSec   = ADD_COLUMN("Dist Prev",   "Distance to previous ref system and mishap chance [%]", String, Ascending, DistanceSec);
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

    PlanetsGrid->Columns[c.Dist]->DefaultCellStyle->Alignment   = DataGridViewContentAlignment::MiddleRight;
    PlanetsGrid->Columns[c.DistSec]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    PlanetsGrid->Columns[c.Colonies]->DefaultCellStyle->Font = m_GridFontSmall;
    PlanetsGrid->Columns[c.Notes]->DefaultCellStyle->Font    = m_GridFontSmall;
    PlanetsGrid->Columns[c.Scan]->DefaultCellStyle->Font     = m_GridFontSmall;
    PlanetsGrid->Columns[c.Grav]->Visible = false;
    PlanetsGrid->Columns[c.AlienLSN]->Visible = false;
    PlanetsGrid->Columns[c.DistSec]->Visible = false;

    // Filter setup
    GridFilter ^filter = gcnew GridFilter(PlanetsGrid, m_bGridUpdateEnabled);
    filter->GridSetup += gcnew GridSetupHandler(this, &Form1::PlanetsFillGrid);
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
    filter->CtrlFiltColC    = PlanetsFiltColC;
    filter->CtrlFiltColN    = PlanetsFiltColN;
    filter->CtrlNumRows     = PlanetsNumRows;

    // Store objects
    PlanetsGrid->Filter = filter;
    PlanetsGrid->Sorter = sorter;

    filter->ResetControls(false);
}

void Form1::PlanetsUpdateControls()
{
    // Inhibit grid update
    PlanetsGrid->Filter->EnableUpdates  = false;

    m_PlanetsAlienLSN               = nullptr;

    PlanetsRefXYZ->DataSource       = m_RefListSystemsXYZ;
    PlanetsRefHome->DataSource      = m_RefListHomes;
    PlanetsRefColony->DataSource    = m_RefListColonies;
    PlanetsRefShip->DataSource      = m_RefListShips;

    // Enable grid update
    PlanetsGrid->Filter->EnableUpdates  = true;
}

void Form1::PlanetsFillGrid()
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
            if( planet->TempClass != -1 )
                cells[c.TC]->Value  = planet->TempClass;
            if( planet->PressClass != -1 )
                cells[c.PC]->Value  = planet->PressClass;
            cells[c.MD]->Value      = (double)planet->MiDiff / 100.0;
            cells[c.Grav]->Value    = (double)planet->Grav / 100.0;
            cells[c.LSN]->Value     = planet->LSN;
            cells[c.Dist]->Value    = GridPrintDistance(system, PlanetsGrid->Filter->RefSystem, gv, age);
            cells[c.DistSec]->Value = GridPrintDistance(system, PlanetsGrid->Filter->RefSystemPrev, gv, age);
            if( system->LastVisited != -1 )
                cells[c.Visited]->Value = system->LastVisited;
            cells[c.Scan]->Value    = system->PrintScanStatus();
            cells[c.Colonies]->Value= system->PrintColonies( planet->Number );

            String ^autoNote = "";
            if( planet->System->HomeSpecies != nullptr )
            {
                // A home planet exists in the system. Make a note about that
                autoNote = String::Format("SP {0} home system", planet->System->HomeSpecies->Name );
            }
            else if( (planet->Name == nullptr) && (planet->AlienName == nullptr) )
            {
                for each( Colony ^colony in planet->System->ColoniesOwned )
                {
                    if( ! String::IsNullOrEmpty(autoNote) )
                    {
                        autoNote += ", ";
                    }
                    autoNote += String::Format("PL {0}", colony->Name );
                }

                List<Alien^>^ present = gcnew List<Alien^>;
                for each( Colony ^colony in planet->System->ColoniesAlien )
                {
                    if( ! present->Contains(colony->Owner) )
                    {
                        present->Add(colony->Owner);
                    }
                }

                for each( Alien^ alien in present )
                {
                    if( ! String::IsNullOrEmpty(autoNote) )
                    {
                        autoNote += ", ";
                    }
                    autoNote += String::Format("SP {0}", alien->Name );
                }
                
                if( ! String::IsNullOrEmpty(autoNote) )
                {
                    autoNote = "In system: " + autoNote;
                }
            }

            String ^note = planet->PrintComment();
            if ( !String::IsNullOrEmpty(note) && !String::IsNullOrEmpty(autoNote) )
            {
                note += "; ";
            }
            cells[c.Notes]->Value = note + autoNote;

            if( m_PlanetsAlienLSN )
                cells[c.AlienLSN]->Value = Calculators::LSN(planet, m_PlanetsAlienLSN);

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
    else
    {
        Colony ^colony = GameData::GetColony(planet->Name);

        if( colony->IsDisband )
        {
            menu->Items->Add( gcnew ToolStripSeparator );
            menu->Items->Add(
                "Cancel Disband",
                nullptr,
                gcnew EventHandler(this, &Form1::PlanetsMenuRemoveNameCancel) );
        }
        else if( colony->EconomicBase <= 0 )
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

    menu->Items->Add( PlanetsMenuFillAlienLSN() );
}

ToolStripMenuItem^ Form1::PlanetsMenuFillAlienLSN()
{
    ToolStripMenuItem ^lsnMenu = gcnew ToolStripMenuItem("Show Alien LSN:");
    bool anyFound = false;

    for each( Alien ^alien in GameData::GetAliens() )
    {
        if( alien == GameData::Player ||
            alien->AtmReq->IsValid() == false )
            continue;

        lsnMenu->DropDownItems->Add( CreateCustomMenuItem(
            "SP " + alien->Name,
            alien,
            gcnew EventHandler1Arg<Alien^>(this, &Form1::PlanetsMenuShowAlienLSN) ) );
        anyFound = true;
    }

    if( !anyFound )
    {
        lsnMenu->DropDownItems->Add("< No alien atmospheric data >");
        lsnMenu->DropDownItems[0]->Enabled = false;
    }

    return lsnMenu;
}

void Form1::PlanetsMenuShowColonies(Object^, EventArgs^)
{
    PlanetsSelectColonies(m_PlanetsMenuRefRow);
}

void Form1::PlanetsMenuSelectRef(Object^, EventArgs^)
{
    if( !String::IsNullOrEmpty(m_PlanetsMenuRef->Name) )
    {
        for each( Colony ^colony in m_GameData->GetColonies() )
            if( colony->Owner == GameData::Player &&
                colony->Name == m_PlanetsMenuRef->Name )
            {
                PlanetsGrid->Filter->SetRefSystem(colony);
                return;
            }
    }

    PlanetsGrid->Filter->SetRefSystem(m_PlanetsMenuRef->System);
}

void Form1::PlanetsMenuAddNameStart(Object^, EventArgs^)
{
    PlanetsGrid->ClearSelection();
    PlanetsGrid->CurrentCell = PlanetsGrid[m_PlanetsColumns.Name, m_PlanetsMenuRefRow];
    PlanetsGrid->ReadOnly = false;
    PlanetsGrid->BeginEdit(true);
}

void Form1::PlanetsMenuAddName(DataGridViewCellEventArgs ^cell)
{
    PlanetsGrid->ReadOnly = true;

    Object ^value = PlanetsGrid[cell->ColumnIndex, cell->RowIndex]->Value;
    if( value == nullptr )
        return;

    String ^name = value->ToString()->Trim();
    if( String::IsNullOrEmpty(name) )
        return;

    // Check for duplicate name
    if( GameData::GetColony( name->ToLower() ) )
    {
        MessageBox::Show(
            this,
            "Planet named '" + name + "' already exists.",
            "Planet Name",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        return;
    }

    Colony ^colony = m_GameData->AddColony(
        GameData::Player,
        name,
        m_PlanetsMenuRef->System,
        m_PlanetsMenuRef->Number,
        false );

    colony->PlanetType = PLANET_COLONY;

    m_CommandMgr->AddCommand( colony, 
        gcnew CmdPlanetName( m_PlanetsMenuRef->System, m_PlanetsMenuRef->Number, name ) );

    SystemsGrid->Filter->Update();
    PlanetsGrid->Filter->Update();
    ColoniesGrid->Filter->Update();
}

void Form1::PlanetsMenuRemoveName(Object^, EventArgs^)
{
    Colony ^colony = GameData::GetColony( m_PlanetsMenuRef->Name );

    if( colony->IsNew )
    {   // Remove Name command
        CmdPlanetName ^cmdDup = gcnew CmdPlanetName(
            m_PlanetsMenuRef->System,
            m_PlanetsMenuRef->Number,
            m_PlanetsMenuRef->Name);

        // Delete the token colony
        GameData::DelColony( m_PlanetsMenuRef->Name );

        m_CommandMgr->DelCommand( colony, cmdDup );
        m_PlanetsMenuRef->Name = nullptr;
    }
    else
    {   // Add Disband command
        m_CommandMgr->AddCommand( colony, gcnew CmdDisband( m_PlanetsMenuRef->Name ) );
    }

    PlanetsGrid->Filter->Update();
    ColoniesGrid->Filter->Update();
}

void Form1::PlanetsMenuRemoveNameCancel(Object^, EventArgs^)
{
    // Remove Disband command
    m_CommandMgr->DelCommand(
        GameData::GetColony( m_PlanetsMenuRef->Name ),
        gcnew CmdDisband( m_PlanetsMenuRef->Name ) );

    PlanetsGrid->Filter->Update();
}

void Form1::PlanetsMenuShowAlienLSN(Alien ^alien)
{
    m_PlanetsAlienLSN = alien;
    PlanetsGrid->Columns[m_PlanetsColumns.AlienLSN]->Visible = true;
    PlanetsGrid->Filter->Update();
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
