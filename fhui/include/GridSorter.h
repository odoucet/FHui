#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

namespace FHUI
{

////////////////////////////////////////////////////////////////

interface class IGridSorter : public System::Collections::IComparer
{
    void    SetSortColumn(int index);
    void    SetGroupBySpecies(bool doGroup);
    void    SetRefSystem(StarSystem ^refSystem);

    int     AddColumn(String ^title, Type ^type, SortOrder defaultSortOrder);
};

////////////////////////////////////////////////////////////////

ref class GridSorterBase abstract : public IGridSorter
{
public:
    virtual void    SetSortColumn(int index);
    virtual void    SetGroupBySpecies(bool doGroup);
    virtual void    SetRefSystem(StarSystem ^refSystem);

    virtual int     AddColumn(String ^title, Type ^type, SortOrder defaultSortOrder);

    virtual int     Compare( Object^ o1, Object^ o2 );

protected:
    GridSorterBase(DataGridView ^grid, Alien ^player);

    virtual void    StoreDefaultSortOrder(int index, SortOrder defaultSortOrder);

    virtual int     GetSortDirectionModifier();
    virtual int     GetForcedDirectionModifier(SortOrder forcedOrder);

    virtual int     AddColumnDefault(String ^title, Type ^type, SortOrder defaultSortOrder) abstract;

    // Initial compare function called when sorting the grid
    // Must handle groupping and custom sorting methods. Then it may fall to
    // DefaultCompare() or BackupCompare().
    virtual int     CustomCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) abstract;
    // Used when Custom and Default compare methods are unable to resolve
    // rows order. Use custom in grid compare secondary sorting.
    virtual int     BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) abstract;

    // Simple cell content compare for int/double/string cells
    virtual int     DefaultCompare(DataGridViewRow ^r1, DataGridViewRow ^r2);

protected:
    DataGridView^   m_Grid;
    Alien^          m_Player;
    StarSystem^     m_RefSystem;

    int             m_SortColumn;
    SortOrder       m_SortOrder;

    bool            m_GroupBySpecies;

    array<SortOrder>^   m_DefaultSortOrder;
};

////////////////////////////////////////////////////////////////

ref class ColoniesGridSorter : public GridSorterBase
{
public:
    ColoniesGridSorter(DataGridView ^grid, Alien ^player);

    enum class CustomSortMode
    {
        Default,
        Owner,
        Type,
        Location,
        Distance,
    };

    virtual int AddColumnDefault(String ^title, Type ^type, SortOrder defaultSortOrder) override;
    virtual int AddColumn(String ^title, Type ^type, SortOrder defaultSortOrder, CustomSortMode sm);

protected:
    virtual int CustomCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;
    virtual int BackupCompare(DataGridViewRow ^r1, DataGridViewRow ^r2) override;

    // Compare colonies by distance to ref system
    virtual int     DistanceCompare(Colony ^c1, Colony ^c2);

protected:
    SortedList<int, CustomSortMode>^ m_SortModes;
};

////////////////////////////////////////////////////////////////

} // end namespace FHUI
