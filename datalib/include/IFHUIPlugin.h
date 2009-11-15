#pragma once

using namespace System;
using namespace System::Data;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

#include "Commands.h"

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
ref class CommandManager;

// ---------------------------------------------------------
//
// Base class for all plugins
//
public interface class IPluginBase
{
public:
    void        SetTurn(int turn);
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
    // Add real commands (ICommand derived objects).
    // This is only called once at the begin of each turn
    // to generate orders.
    void        GenerateCommands(CommandManager^);

    // These functions are only good for adding comments to orders template.
    // All functions below are called each time orders template is refresed.
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

    void        SetColony(Colony ^colony, CommandPhase phase);

    void        EvalOrder(ICommand ^cmd);
    void        EvalOrderTransfer(ICommand ^cmd);

    void        UpdateEU(int eu);
    void        UpdatePop(int pop);
    void        UpdateInventory(ICommand ^cmd, InventoryType it);

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
