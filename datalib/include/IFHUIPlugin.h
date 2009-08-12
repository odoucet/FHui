#pragma once

using namespace System;
using namespace System::Data;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

namespace FHUI
{

ref class StarSystem;
ref class Planet;
ref class Colony;
ref class Alien;
ref class Ship;
ref class GameData;
interface class IGridFilter;
interface class IGridSorter;

ref class BudgetTracker;

// ---------------------------------------------------------
//
// Base class for all plugins
//
public interface class IPluginBase
{
public:
    void        SetGameData(GameData^);
};

// ---------------------------------------------------------

public enum class GridType
{
    Systems,
    Planets,
    Colonies,
    Ships,
    Aliens,
};

// ---------------------------------------------------------
// Grid Plugin - add new colums to data grids
public interface class IGridPlugin : public IPluginBase
{
public:
    void        AddColumns(GridType, DataTable^);
    void        AddColumns(GridType, IGridSorter^);
    void        GridFormat(GridType, DataGridView^);
    void        AddMenuItems(GridType, DataGridView^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, StarSystem^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Planet^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Colony^);
    void        AddRowData(DataRow^, IGridFilter^, Ship^);
    void        AddRowData(DataRow^, IGridFilter^, Alien^);
};

// ---------------------------------------------------------
// Orders plugin
public interface class IOrdersPlugin : public IPluginBase
{
public:
    void        GenerateCombat(List<String^>^, StarSystem^);
    void        GeneratePreDeparture(List<String^>^, StarSystem^);
    void        GenerateJumps(List<String^>^, Ship^);
    void        GenerateProduction(List<String^>^, Colony^, BudgetTracker^);
    void        GeneratePostArrival(List<String^>^, Ship^);
    void        GenerateStrikes(List<String^>^, StarSystem^);
};

// ---------------------------------------------------------

public ref class BudgetTracker
{
public:
    BudgetTracker(List<String^> ^orders, int euCarried);

    void        SetColony(Colony^);

    void        Recycle(int eu);
    void        Spend(int eu);
    void        UsePopulation(int pop);

    int         GetAvailPopulation()    { return m_PopAvail; }
    int         GetAvailBudget()        { return Math::Min(m_BudgetAvail, m_BudgetTotal); }
    int         GetTotalBudget()        { return m_BudgetTotal; }

protected:
    List<String^>^  m_Orders;
    int             m_BudgetTotal;
    int             m_BudgetAvail;
    int             m_PopAvail;
};

// ---------------------------------------------------------

} // end namespace FHUIPlugin
