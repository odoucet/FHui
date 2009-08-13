#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

namespace FHUI
{

////////////////////////////////////////////////////////////////

ref class GridSorterBase abstract : public IGridSorter
{
public:
    virtual void    SetSortColumn(int index);
    virtual void    SetGroupBySpecies(bool doGroup);
    virtual void    SetRefSystem(StarSystem ^refSystem);

    virtual int     Compare( Object^ o1, Object^ o2 );

    enum class CustomSortMode
    {
        Default,
        Owner,
        Type,
        Location,
        Distance,
        Relation,
    };

    virtual int     AddColumn(String ^title, String ^description, Type ^type, SortOrder defaultSortOrder);
    virtual int     AddColumn(String ^title, String ^description, Type ^type, SortOrder defaultSortOrder, CustomSortMode sortMode);

protected:
    GridSorterBase(DataGridView ^grid);

    virtual void    StoreDefaultSortOrder(int index, SortOrder defaultSortOrder);

    virtual int     GetSortDirectionModifier();
    virtual int     GetForcedDirectionModifier(SortOrder forcedOrder);

    // Used when normal compare methods are unable to resolve
    // rows order. Apply grid specific secondary sorting.
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) abstract;

    // Simple cell content compare for int/double/string cells
    virtual int     CompareDefault(DataGridViewRow ^r1, DataGridViewRow ^r2);

    // Custom comparers
    virtual int     CompareOwner(IGridDataSrc ^o1, IGridDataSrc ^o2);
    virtual int     CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2);    // must be overriden for proper usage
    virtual int     CompareLocation(IGridDataSrc ^o1, IGridDataSrc ^o2);
    virtual int     CompareDistance(IGridDataSrc ^o1, IGridDataSrc ^o2);
    virtual int     CompareRelation(IGridDataSrc ^o1, IGridDataSrc ^o2);

protected:
    DataGridView^   m_Grid;
    StarSystem^     m_RefSystem;

    int             m_SortColumn;
    SortOrder       m_SortOrder;

    bool            m_GroupBySpecies;

    array<SortOrder>^   m_DefaultSortOrder;
    SortedList<int, CustomSortMode>^ m_SortModes;
};

////////////////////////////////////////////////////////////////

ref class SystemsGridSorter : public GridSorterBase
{
public:
    SystemsGridSorter(DataGridView ^grid);

protected:
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
};

////////////////////////////////////////////////////////////////

ref class PlanetsGridSorter : public GridSorterBase
{
public:
    PlanetsGridSorter(DataGridView ^grid);

protected:
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
};

////////////////////////////////////////////////////////////////

ref class ColoniesGridSorter : public GridSorterBase
{
public:
    ColoniesGridSorter(DataGridView ^grid);

protected:
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
    virtual int     CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2) override;
};

////////////////////////////////////////////////////////////////

ref class ShipsGridSorter : public GridSorterBase
{
public:
    ShipsGridSorter(DataGridView ^grid);

protected:
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
    virtual int     CompareType(IGridDataSrc ^o1, IGridDataSrc ^o2) override;

    virtual int     GetShipClassValue(Ship ^s);
};

////////////////////////////////////////////////////////////////

ref class AliensGridSorter : public GridSorterBase
{
public:
    AliensGridSorter(DataGridView ^grid);

protected:
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
    virtual int     CompareRelation(IGridDataSrc ^o1, IGridDataSrc ^o2) override;
};

////////////////////////////////////////////////////////////////

} // end namespace FHUI
