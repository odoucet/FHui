#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace FHUI
{

ref class RegexMatcher;

private value struct OrdersDir
{
    static initonly String^ Folder   = "orders/FHUI.Data/";
    static initonly String^ Commands = "cmd_t{0}.txt";
};

private ref class TurnCommands
{
public:
    TurnCommands()
        : Commands(gcnew List<ICommand^>)
        , AutoEnabled(false)
        , AutoOrdersPreDeparture(gcnew SortedList<StarSystem^, List<String^>^>)
        , AutoOrdersJumps(gcnew SortedList<Ship^, List<String^>^>)
        , AutoOrdersProduction(gcnew SortedList<Colony^, List<Pair<String^, int>^>^>)
    {
    }

    List<ICommand^>^                                 Commands;

    bool                                             AutoEnabled;
    SortedList<StarSystem^, List<String^>^>^         AutoOrdersPreDeparture;
    SortedList<Ship^, List<String^>^>^               AutoOrdersJumps;
    SortedList<Colony^, List<Pair<String^, int>^>^>^ AutoOrdersProduction;
};

private ref class CommandManager
{
public:
    CommandManager(GameData^, String^);

    void        SelectTurn(int turn);
    void        GenerateTemplate(System::Windows::Forms::RichTextBox^);

    void        LoadCommands();
    void        SaveCommands();
    void        DeleteCommands();

    void        AddCommand(ICommand ^cmd);
    void        DelCommand(ICommand ^cmd);
    List<ICommand^>^ GetCommands() { return m_CommandData[m_CurrentTurn]->Commands; }

    // For production commands
    void        AddCommand(Colony^, ICommandProd ^cmd);
    void        DelCommand(Colony^, ICommandProd ^cmd);
    List<ICommandProd^>^ GetCommands(Colony ^colony) { return colony->Orders; }

    List<String^>^ PrintSystemStatus(StarSystem^ system, bool listIncomplete);

    property bool AutoEnabled
    {
        bool get() { return m_CommandData[m_CurrentTurn]->AutoEnabled; };
        void set(bool v) { m_CommandData[m_CurrentTurn]->AutoEnabled = v; };
    }

    void        SetAutoOrderPreDeparture(StarSystem^, String^);
    void        SetAutoOrderJumps(Ship^, String^);
    void        SetAutoOrderProduction(Colony^, String^, int);

    List<String^>^  GetAutoOrdersPreDeparture(StarSystem^);
    List<String^>^  GetAutoOrdersJumps(Ship^);
    List<Pair<String^, int>^>^  GetAutoOrdersProduction(Colony^);

private:

    void        AddCommandDontSave(ICommand ^cmd);
    void        SortCommands();

    void        GenerateCombat();
    void        GeneratePreDeparture();
    void        GenerateJumps();
    void        GenerateProduction();
    void        GeneratePostArrival();
    void        GenerateStrikes();

    void        GenerateCombatInfo(StarSystem^);
    void        GeneratePreDepartureInfo(StarSystem^);
    void        GenerateJumpInfo(Ship^);
    void        GenerateScanOrders();
    void        GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget);
    void        GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget);

    List<String^>^          m_OrderList;
    String^                 m_Path;
    GameData^               m_GameData;
    RegexMatcher^           m_RM;

    int                     m_CurrentTurn;
    SortedList<int, TurnCommands^>^ m_CommandData;
};

} // end namespace FHUI
