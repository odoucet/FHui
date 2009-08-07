#include "StdAfx.h"
#include "GridSorter.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

GridSorterBase::GridSorterBase(DataGridView ^grid, Alien ^player)
    : m_Grid(grid)
    , m_Player(player)
    , m_RefSystem(nullptr)
    , m_SortColumn(-1)
    , m_SortOrder(SortOrder::None)
    , m_GroupBySpecies(false)
{
    m_DefaultSortOrder = gcnew array<SortOrder>(0);
}

int GridSorterBase::AddColumn(String ^title, Type ^type, SortOrder defaultSortOrder)
{
    return AddColumnDefault(title, type, defaultSortOrder);
}

void GridSorterBase::SetRefSystem(StarSystem ^refSystem)
{
    if( m_RefSystem != refSystem )
    {
        m_RefSystem = refSystem;
        m_Grid->Sort( this );
    }
}

void GridSorterBase::StoreDefaultSortOrder(int index, SortOrder defaultSortOrder)
{
    if( m_DefaultSortOrder->Length <= index )
        Array::Resize(m_DefaultSortOrder, index + 10);
    m_DefaultSortOrder[index] = defaultSortOrder;
}

void GridSorterBase::SetSortColumn(int index)
{
    int prevColumn = m_SortColumn;

    if( index != m_SortColumn )
    {
        m_SortColumn = index;
        m_SortOrder  = m_DefaultSortOrder[index];
    }
    else
    {
        if( m_SortOrder == SortOrder::Ascending )
            m_SortOrder = SortOrder::Descending;
        else
            m_SortOrder = SortOrder::Ascending;
    }

    if( prevColumn != m_SortColumn &&
        prevColumn != -1 )
    {
        m_Grid->Columns[prevColumn]->HeaderCell->SortGlyphDirection = SortOrder::None;
    }

    m_Grid->Columns[m_SortColumn]->HeaderCell->SortGlyphDirection = m_SortOrder;
    m_Grid->Sort( this );
}

void GridSorterBase::SetGroupBySpecies(bool doGroup)
{
    if( doGroup != m_GroupBySpecies )
    {
        m_GroupBySpecies = doGroup;
        if( m_SortColumn != -1 )
            m_Grid->Sort( this );
    }
}

int GridSorterBase::Compare( Object^ o1, Object^ o2 )
{
    return GetSortDirectionModifier() *
        CustomCompare( (DataGridViewRow^)o1, (DataGridViewRow^)o2 );
}

int GridSorterBase::GetSortDirectionModifier()
{
    return m_SortOrder == SortOrder::Ascending ? 1 : -1;
}

int GridSorterBase::GetForcedDirectionModifier(SortOrder forcedOrder)
{
    if( forcedOrder == m_SortOrder )
        return 1;
    return -1;
}

int GridSorterBase::DefaultCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    int result = 0;
    Object ^v1 = r1->Cells[m_SortColumn]->Value;
    Object ^v2 = r2->Cells[m_SortColumn]->Value;

    // If one of values is missing, set result and return
    if( v1 == nullptr )
    {
        if( v2 != nullptr )
            result = 1 * GetSortDirectionModifier();
    }
    else if( v2 == nullptr )
        result = -1 * GetSortDirectionModifier();
    if( result != 0 )
        return result;

    // If both values are missing, use backup compare
    if( v1 == nullptr && v2 == nullptr )
        return BackupCompare(r1, r2);

    // Default cell content compare
    Type ^type = r1->Cells[m_SortColumn]->ValueType;

    if( type == int::typeid )
    {
        Int32 ^i1 = (Int32^)v1;
        Int32 ^i2 = (Int32^)v2;
        result = i1->CompareTo(i2);
    }
    else if( type == double::typeid )
    {
        Double ^d1 = (Double^)v1;
        Double ^d2 = (Double^)v2;
        result = d1->CompareTo(d2);
    }
    else if( type == String::typeid )
    {
        String ^s1 = (String^)v1;
        String ^s2 = (String^)v2;
        result = s1->CompareTo(s2);
    }
    else
        throw gcnew FHUIDataIntegrityException("Default column compare type invalid: " + type->ToString());

    if( result == 0 )
        return BackupCompare(r1, r2);

    return result;
}

////////////////////////////////////////////////////////////////

ColoniesGridSorter::ColoniesGridSorter(DataGridView ^grid, Alien ^player)
    : GridSorterBase(grid, player)
{
    m_SortModes = gcnew SortedList<int, CustomSortMode>;
}

int ColoniesGridSorter::AddColumnDefault(String ^title, Type ^type, SortOrder defaultSortOrder)
{
    return AddColumn(title, type, defaultSortOrder, CustomSortMode::Default);
}

int ColoniesGridSorter::AddColumn(String ^title, Type ^type, SortOrder defaultSortOrder, CustomSortMode sm)
{
    DataGridViewCell ^cell = gcnew DataGridViewTextBoxCell;
    cell->ValueType = type;

    DataGridViewColumn ^col = gcnew DataGridViewColumn;
    col->HeaderText = title;
    col->CellTemplate = cell;
    col->SortMode = DataGridViewColumnSortMode::Programmatic;

    int index = m_Grid->Columns->Add(col);

    m_SortModes[index] = sm;
    StoreDefaultSortOrder(index, defaultSortOrder);

    return index;
}

int ColoniesGridSorter::CustomCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Colony ^c1 = safe_cast<Colony^>( ((DataGridViewRow^)r1)->Cells[0]->Value );
    Colony ^c2 = safe_cast<Colony^>( ((DataGridViewRow^)r2)->Cells[0]->Value );

    // Group by species
    if( m_GroupBySpecies )
    {
        if( c1->Owner != c2->Owner )
        {
            // Regardless of sort direction, species grouping always
            // puts player on top, and other species alphabetically
            // (this is why always GetSortDirectionModifier() is used)
            if( c1->Owner == m_Player )
                return -1 * GetSortDirectionModifier();
            else if( c2->Owner == m_Player )
                return 1 * GetSortDirectionModifier();
            return c1->Owner->Name->CompareTo(c2->Owner->Name) * GetSortDirectionModifier();
        }
    }

    int result = 0;

    // Is it a custom sort column?
    switch( m_SortModes[m_SortColumn] )
    {
    case CustomSortMode::Default:
        return DefaultCompare(r1, r2);

    case CustomSortMode::Owner:
        result = c1->Owner->Name->CompareTo(c2->Owner->Name);
        break;

    case CustomSortMode::Type:
        result = c1->PlanetType - c2->PlanetType;   // Make HOME first when descending order (this order is default)
        break;

    case CustomSortMode::Location:
        result = c1->System->CompareLocation(c2->System);
        if( result == 0 )
            result = c1->PlanetNum - c2->PlanetNum;
        break;

    case CustomSortMode::Distance:
        result = DistanceCompare(c1, c2);
        break;
    }

    if( result == 0 )
        return BackupCompare(r1, r2);

    return result;
}

int ColoniesGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Colony ^c1 = safe_cast<Colony^>( ((DataGridViewRow^)r1)->Cells[0]->Value );
    Colony ^c2 = safe_cast<Colony^>( ((DataGridViewRow^)r2)->Cells[0]->Value );

    int result = 0;

    // Step 1: by distance to ref system
    result = DistanceCompare(c1, c2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by production capacity
    if( result == 0 )
        result = (c1->EUAvail - c2->EUAvail) * GetForcedDirectionModifier(SortOrder::Descending);

    // Step 3: by production capacity
    if( result == 0 )
        result = (c1->EconomicBase - c2->EconomicBase) * GetForcedDirectionModifier(SortOrder::Descending);

    // Step 4: by name
    if( result == 0 )
        result = c1->Name->CompareTo(c2->Name) * GetForcedDirectionModifier(SortOrder::Ascending);

    return result;
}

int ColoniesGridSorter::DistanceCompare(Colony ^c1, Colony ^c2)
{
    if( m_RefSystem == nullptr )
        return 0;

    double distDiff =
        c1->System->CalcDistance(m_RefSystem) - c2->System->CalcDistance(m_RefSystem);

    if( distDiff == 0 )
        return 0;
    return distDiff < 0 ? -1 : 1;
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
