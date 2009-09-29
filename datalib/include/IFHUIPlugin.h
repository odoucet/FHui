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
interface class ICommand;
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
    void        AddColumns(GridType, IGridSorter^);
    void        GridFormat(GridType, DataGridView^);
    void        AddMenuItems(GridType, DataGridView^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, StarSystem^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Planet^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Colony^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Ship^);
    void        AddRowData(DataGridViewRow^, IGridFilter^, Alien^);
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
    BudgetTracker(List<String^>^ orders, int euCarried);

    void        SetColony(Colony ^colony);

    void        EvalOrder(ICommand ^cmd);

    void        UpdateEU(int eu);
    void        UpdateCU(int pop);
    void        UpdateInventory(InventoryType it, int mod);

    int         GetAvailBudget()        { return Math::Min(m_BudgetAvail, m_BudgetTotal); }
    int         GetTotalBudget()        { return m_BudgetTotal; }

protected:
    void        BudgetTracker::AddComment(String ^comment);

    List<String^>^  m_Orders;
    List<String^>^  m_OrdersGlobal;
    Colony^         m_Colony;
    int             m_BudgetTotal;
    int             m_BudgetAvail;
};

// ---------------------------------------------------------

} // end namespace FHUIPlugin
