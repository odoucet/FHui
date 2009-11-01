#include "StdAfx.h"
#include "Form1.h"
#include "enums.h"

using namespace System::IO;

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

void Form1::SaveUISettings()
{
    if( m_bUISaveEnabled == false )
        return;

    String ^path = GetDataDir("fhui.ini");

    // Create stream
    StreamWriter ^sw = File::CreateText( path );

    // Header
    sw->WriteLine("; FHUI generated file. Please don't edit.");

    List<String^>^ settings = gcnew List<String^>;

    SaveUIGrid(settings, SystemsGrid, "Systems");
    SaveUIGrid(settings, PlanetsGrid, "Planets");
    SaveUIGrid(settings, ColoniesGrid, "Colonies");
    SaveUIGrid(settings, ShipsGrid, "Ships");
    SaveUIGrid(settings, AliensGrid, "Aliens");

    // Write to stream
    for each( String ^entry in settings )
    {
        sw->WriteLine(entry);
    }
    sw->Close();
}

void Form1::SaveUIGrid(List<String^> ^settings, DblBufDGV ^grid, String ^tab)
{
    settings->Add("");
    settings->Add("TAB " + tab);
    settings->Add("Select "
        + ( grid->SelectionMode == System::Windows::Forms::DataGridViewSelectionMode::CellSelect
            ? "cells" : "rows"));

    if( grid == ColoniesGrid )
        settings->Add("Summary " + (ColoniesSummaryRow->Checked ? "on" : "off"));

    for each( DataGridViewColumn ^column in grid->Columns )
    {
        if( column->Index == 0 )
            continue;

        String ^entry = String::Format("{0}{1}{2}",
            grid->Sorter->SortColumn == column->Index
                ? (grid->Sorter->SortColumnOrder == SortOrder::Ascending ? "*^" : "*v")
                : "",
            column->Visible ? "+" : "-",
            column->HeaderText );

        settings->Add( entry );
    }
    settings->Add("END");
}

void Form1::LoadUISettings()
{
    // Open file
    StreamReader ^sr;
    String ^path = GetDataDir("fhui.ini");
    FileInfo ^fileInfo = gcnew FileInfo(path);

    if( !fileInfo->Exists )
    {
        return;
    }

    sr = File::OpenText( path );

    String ^line;
    DblBufDGV ^grid = nullptr;
    int sortIndex;
    bool sortAscending;

    while( (line = sr->ReadLine()) != nullptr ) 
    {
        line = line->Trim();
        if( String::IsNullOrEmpty(line) ||
            line[0] == ';' )
            continue;

        if( line == "END" )
        {
            if( grid && sortIndex != -1 )
            {
                grid->Sorter->SortColumn = sortIndex;
                if( sortAscending && grid->Sorter->SortColumnOrder != SortOrder::Ascending )
                    grid->Sorter->SortColumn = sortIndex;
                else if( !sortAscending && grid->Sorter->SortColumnOrder == SortOrder::Ascending )
                    grid->Sorter->SortColumn = sortIndex;
            }

            grid = nullptr;
            continue;
        }

        if( grid == nullptr )
        {
            if( line->Length < 5 ||
                line->Substring(0, 4) != "TAB " )
                continue;
            String ^tab = line->Substring(4)->Trim();
            if( tab == "Systems" )
                grid = SystemsGrid;
            else if( tab == "Planets" )
                grid = PlanetsGrid;
            else if( tab == "Colonies" )
                grid = ColoniesGrid;
            else if( tab == "Ships" )
                grid = ShipsGrid;
            else if( tab == "Aliens" )
                grid = AliensGrid;
            sortIndex = -1;
            sortAscending = true;
            continue;
        }

        if( line == "Select cells" )
        {
            grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::CellSelect;
            continue;
        }
        if( line == "Select rows" )
        {
            grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            continue;
        }
        if( line == "Summary on" )
        {
            if( grid == ColoniesGrid )
                ColoniesSummaryRow->Checked = true;
        }
        if( line == "Summary off" )
        {
            if( grid == ColoniesGrid )
                ColoniesSummaryRow->Checked = false;
        }

        bool sort = false;
        if( line[0] == L'*' )
        {
            sort = true;
            line = line->Substring(1);
            if( String::IsNullOrEmpty(line) )
                continue;
            if( line[0] == L'^' )
                sortAscending = true;
            else
                sortAscending = false;
            line = line->Substring(1);
            if( String::IsNullOrEmpty(line) )
                continue;
        }
        bool visible = line[0] == L'+';
        line = line->Substring(1);
        if( String::IsNullOrEmpty(line) )
            continue;

        int index = -1;
        for each( DataGridViewColumn ^column in grid->Columns )
        {
            if( column->HeaderText == line )
            {
                index = column->Index;
                break;
            }
        }
        if( index != -1 )
        {
            if( sort )
                sortIndex = index;
            grid->Columns[index]->Visible = visible;
        }
    }
}

////////////////////////////////////////////////////////////////

void Form1::UtilTabSelected()
{
    UtilProdPenaltyLS->Value = GameData::Player->TechLevelsAssumed[TECH_LS];
}

void Form1::UtilUpdateAll()
{
    UtilUpdateTRInfo();
    UtilUpdateResearch();
    UtilUpdateProdPenalty();
}

void Form1::UtilUpdateTRInfo()
{
    int trSize = Decimal::ToInt32(UtilTRSize->Value);
    double maintenance = ((trSize * 100.0) / 25.0) *
        Calculators::ShipMaintenanceDiscount(GameData::Player->TechLevelsAssumed[TECH_ML]);

    UtilTRInfoCap->Text = Calculators::TransportCapacity(trSize).ToString();
    UtilTrInfoMaint->Text = maintenance.ToString("F1");
    UtilTrInfoMA->Text = (2 * trSize).ToString();
    UtilTrInfoMA->ForeColor =
        ( (2 * trSize) > GameData::Player->TechLevels[TECH_MA] )
            ? Color::Red : Color::Black;
    UtilTrInfoCost->Text = (trSize * 100).ToString();
}

void Form1::UtilUpdateResearch()
{
    int from = Decimal::ToInt32( UtilResFrom->Value );
    int to = Decimal::ToInt32( UtilResTo->Value );
    if( from >= to )
    {
        to = from + 1;
        UtilResTo->Value = to;
    }

    UtilResInfoGuided->Text     = Calculators::ResearchCost(from, to, true).ToString();
    UtilResInfoAverage->Text    = Calculators::ResearchCost(from, to, false).ToString();
    UtilResInfoGuaranteed->Text = Calculators::ResearchCost(from, from + (to - from) * 2, false).ToString();
}

void Form1::UtilUpdateProdPenalty()
{
    int lsn = Decimal::ToInt32( UtilProdPenaltyLSN->Value );
    int ls = Decimal::ToInt32( UtilProdPenaltyLS->Value );
    int penalty = Calculators::ProductionPenalty(lsn, ls);
    if( penalty == -1 )
        UtilProdPenalty->Text = "N/A";
    else
        UtilProdPenalty->Text = penalty.ToString() + "%";
}

} // end namespace FHUI
