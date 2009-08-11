#include "StdAfx.h"
#include "GridSorter.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

GridSorterBase::GridSorterBase(DataGridView ^grid)
    : m_Grid(grid)
    , m_RefSystem(nullptr)
    , m_SortColumn(-1)
    , m_SortOrder(SortOrder::None)
    , m_GroupBySpecies(false)
{
    m_DefaultSortOrder = gcnew array<SortOrder>(0);
    m_SortModes = gcnew SortedList<int, CustomSortMode>;
}

int GridSorterBase::AddColumn(String ^title, String ^description, Type ^type, SortOrder defaultSortOrder)
{
    return AddColumn(title, description, type, defaultSortOrder, CustomSortMode::Default);
}

int GridSorterBase::AddColumn(String ^title, String ^description, Type ^type, SortOrder defaultSortOrder, CustomSortMode sm)
{
    DataGridViewCell ^cell = gcnew DataGridViewTextBoxCell;
    cell->ValueType = type;

    DataGridViewColumn ^col = gcnew DataGridViewColumn;
    col->HeaderText = title;
    col->CellTemplate = cell;
    col->SortMode = DataGridViewColumnSortMode::Programmatic;

    int index = m_Grid->Columns->Add(col);
    m_Grid->Columns[index]->HeaderCell->ToolTipText = description;

    m_SortModes[index] = sm;
    StoreDefaultSortOrder(index, defaultSortOrder);

    return index;
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

int GridSorterBase::Compare( Object^ obj1, Object^ obj2 )
{
    DataGridViewRow ^r1 = safe_cast<DataGridViewRow^>( obj1 );
    DataGridViewRow ^r2 = safe_cast<DataGridViewRow^>( obj2 );

    IGridDataSrc ^o1 = safe_cast<IGridDataSrc^>( r1->Cells[0]->Value );
    IGridDataSrc ^o2 = safe_cast<IGridDataSrc^>( r2->Cells[0]->Value );

    // Group by species
    if( m_GroupBySpecies )
    {
        Alien ^owner1 = o1->GetFilterOwner();
        Alien ^owner2 = o2->GetFilterOwner();

        if(owner1 != owner2 )
        {
            // Regardless of sort direction, species grouping always
            // puts player on top, and other species alphabetically
            // (this is why always GetSortDirectionModifier() is used)
            if( owner1 == GameData::Player )
                return -1;
            else if( owner2 == GameData::Player )
                return 1;
            return owner1->Name->CompareTo(owner2->Name);
        }
    }

    int result = 0;

    // Is it a custom sort column?
    switch( m_SortModes[m_SortColumn] )
    {
    case CustomSortMode::Default:
        result = CompareDefault(r1, r2);
        break;

    case CustomSortMode::Owner:
        result = CompareOwner(o1, o2);
        break;

    case CustomSortMode::Type:
        result = CompareType(o1, o2);
        break;

    case CustomSortMode::Location:
        result = CompareLocation(o1, o2);
        break;

    case CustomSortMode::Distance:
        result = CompareDistance(o1, o2);
        break;

    default:
        throw gcnew FHUIDataImplException("Unsupported custom sort mode: " + m_SortModes[m_SortColumn].ToString() );
    }

    if( result == 0 )
        result = BackupCompare(r1, r2);

    return result * GetSortDirectionModifier();
}

int GridSorterBase::CompareDefault(DataGridViewRow ^r1, DataGridViewRow ^r2)
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
        result = BackupCompare(r1, r2);

    return result;
}

int GridSorterBase::CompareOwner(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    return o1->GetFilterOwner()->Name->CompareTo(o2->GetFilterOwner()->Name);
}

int GridSorterBase::CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    throw gcnew FHUIDataIntegrityException("Sorter: Type custom compare must be overriden!");
}

int GridSorterBase::CompareLocation(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    int p1, p2;
    int result = o1->GetFilterLocation(p1)->CompareLocation(o2->GetFilterLocation(p2));
    if( result == 0 )
        result = p1 - p2;
    return result;
}

int GridSorterBase::CompareDistance(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    if( m_RefSystem == nullptr )
        return 0;

    double distDiff =
        o1->GetFilterSystem()->CalcDistance(m_RefSystem) - o2->GetFilterSystem()->CalcDistance(m_RefSystem);

    if( distDiff == 0 )
        return 0;
    return distDiff < 0 ? -1 : 1;
}

////////////////////////////////////////////////////////////////

SystemsGridSorter::SystemsGridSorter(DataGridView ^grid)
    : GridSorterBase(grid)
{
}

int SystemsGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    StarSystem ^s1 = safe_cast<StarSystem^>( r1->Cells[0]->Value );
    StarSystem ^s2 = safe_cast<StarSystem^>( r2->Cells[0]->Value );

    int result = 0;

    // Step 1: by distance to ref system
    result = CompareDistance(s1, s2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by available LSN
    if( result == 0 )
        result = (s1->MinLSNAvail - s2->MinLSNAvail) * GetForcedDirectionModifier(SortOrder::Ascending);

    return result;
}

////////////////////////////////////////////////////////////////

ColoniesGridSorter::ColoniesGridSorter(DataGridView ^grid)
    : GridSorterBase(grid)
{
}
int ColoniesGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Colony ^c1 = safe_cast<Colony^>( r1->Cells[0]->Value );
    Colony ^c2 = safe_cast<Colony^>( r2->Cells[0]->Value );

    int result = 0;

    // Step 1: by distance to ref system
    result = CompareDistance(c1, c2) * GetForcedDirectionModifier(SortOrder::Ascending);

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

int ColoniesGridSorter::CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    // Make HOME first when descending order (this order is default)
    return ((Colony^)o1)->PlanetType - ((Colony^)o2)->PlanetType;
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
