#include "StdAfx.h"
#include "GridSorter.h"
#include "enums.h"

////////////////////////////////////////////////////////////////

namespace FHUI
{

////////////////////////////////////////////////////////////////

GridSorterBase::GridSorterBase(DataGridView ^grid)
    : m_Grid(grid)
    , m_RefSystem(nullptr)
    , m_RefSystemPrev(nullptr)
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
        // For the very first set, clone refSystem to prev too
        m_RefSystemPrev = m_RefSystem ? m_RefSystem : refSystem;
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

    // Summary row support: make it always first
    if( r1->Cells[0]->Value == nullptr )
        return -1;
    if( r2->Cells[0]->Value == nullptr )
        return 1;

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
        result = CompareDistance(m_RefSystem, o1, o2);
        break;

    case CustomSortMode::DistanceSec:
        result = CompareDistance(m_RefSystemPrev, o1, o2);
        break;

    case CustomSortMode::Relation:
        result = CompareRelation(o1, o2);
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
    StarSystem ^s1 = o1->GetFilterSystem();
    StarSystem ^s2 = o2->GetFilterSystem();

    // Detect wormholes
    if( s1 && s2 && s1->IsWormholeTarget( s2 ) )
        return 0;

    int result = 0;
    int p1, p2;
    s1 = o1->GetFilterLocation(p1);
    s2 = o2->GetFilterLocation(p2);
    if( s1 && s2 )
    {
        result = s1->CompareLocation(s2);
        if( result == 0 )
            result = p1 - p2;
    }
    else if( s1 )
        result = -1;
    else if ( s2 )
        result = 1;

    return result;
}

int GridSorterBase::CompareDistance(StarSystem ^refSystem, IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    if( refSystem == nullptr )
        return 0;

    double distDiff =
        o1->GetFilterSystem()->CalcDistance(refSystem) - o2->GetFilterSystem()->CalcDistance(refSystem);

    if( distDiff == 0 )
        return 0;
    return distDiff < 0 ? -1 : 1;
}

int GridSorterBase::CompareRelation(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    throw gcnew FHUIDataIntegrityException("Sorter: Relation custom compare must be overriden!");
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
    result = CompareDistance(m_RefSystem, s1, s2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by available LSN
    if( result == 0 )
        result = (s1->MinLSNAvail - s2->MinLSNAvail) * GetForcedDirectionModifier(SortOrder::Ascending);

    return result;
}

////////////////////////////////////////////////////////////////

PlanetsGridSorter::PlanetsGridSorter(DataGridView ^grid)
    : GridSorterBase(grid)
{
}

int PlanetsGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Planet ^p1 = safe_cast<Planet^>( r1->Cells[0]->Value );
    Planet ^p2 = safe_cast<Planet^>( r2->Cells[0]->Value );

    int result = 0;

    // Step 1: by distance to ref system
    result = CompareDistance(m_RefSystem, p1, p2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by available LSN
    if( result == 0 )
        result = (p1->LSN - p2->LSN) * GetForcedDirectionModifier(SortOrder::Ascending);

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
    result = CompareDistance(m_RefSystem, c1, c2) * GetForcedDirectionModifier(SortOrder::Ascending);

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

ShipsGridSorter::ShipsGridSorter(DataGridView ^grid)
    : GridSorterBase(grid)
{
}

int ShipsGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Ship ^s1 = safe_cast<Ship^>( r1->Cells[0]->Value );
    Ship ^s2 = safe_cast<Ship^>( r2->Cells[0]->Value );

    int result = 0;

    // Step 1: by type
    if( result == 0 )
        result = CompareType(s1, s2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by age
    if( result == 0 )
        result = (s1->Age - s2->Age) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 3: by distance to ref system
    //result = CompareDistance(m_RefSystem, s1, s2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 4: by name
    if( result == 0 )
        result = s1->Name->CompareTo(s2->Name) * GetForcedDirectionModifier(SortOrder::Ascending);

    return result;
}

int ShipsGridSorter::CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    Ship ^s1 = safe_cast<Ship^>( o1 );
    Ship ^s2 = safe_cast<Ship^>( o2 );

    return GetShipClassValue(s1) - GetShipClassValue(s2);
}

int ShipsGridSorter::GetShipClassValue(Ship ^s)
{
    // Bases first
    if( s->Type == SHIP_BAS )
        return 1;
    // TR last, by tonnage and equal tonnage separated from sublight
    if( s->Type == SHIP_TR )
        return 10000 + (10000 - 10 * s->Size) + (s->SubLight ? 1 : 0);

    // Big military ships first and equal tonnage separated from sublight
    return 100 + SHIP_MAX - s->Type + (s->SubLight ? 1 : 0);
}

////////////////////////////////////////////////////////////////

AliensGridSorter::AliensGridSorter(DataGridView ^grid)
    : GridSorterBase(grid)
{
}

int AliensGridSorter::BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2)
{
    Alien ^a1 = safe_cast<Alien^>( r1->Cells[0]->Value );
    Alien ^a2 = safe_cast<Alien^>( r2->Cells[0]->Value );

    int result = 0;

    // Step 1: by relation
    result = CompareRelation(a1, a2) * GetForcedDirectionModifier(SortOrder::Ascending);

    // Step 2: by home system distance
    if( result == 0 &&
        a1->HomeSystem &&
        a2->HomeSystem )
    {
        double dist1 = 999999.9;
        double dist2 = 999999.9;
        if( a1->HomeSystem )
            dist1 = GameData::Player->HomeSystem->CalcDistance( a1->HomeSystem );
        if( a2->HomeSystem )
            dist2 = GameData::Player->HomeSystem->CalcDistance( a2->HomeSystem );
        result = dist1 < dist2 ? -1 : 1;
    }

    // Step 3: by name
    if( result == 0 )
        result = a1->Name->CompareTo(a2->Name) * GetForcedDirectionModifier(SortOrder::Ascending);

    return result;
}

int AliensGridSorter::CompareRelation(IGridDataSrc ^o1, IGridDataSrc ^o2)
{
    Alien ^a1 = safe_cast<Alien^>( o1 );
    Alien ^a2 = safe_cast<Alien^>( o2 );

    return a1->Relation - a2->Relation;
}

////////////////////////////////////////////////////////////////

} // end namespace FHUI
