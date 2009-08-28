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

    void SelectTurn(int turn)
    {
        m_CurrentTurn = turn;
        if ( ! m_CommandData->ContainsKey(turn) )
        {
            m_CommandData[turn] = gcnew TurnCommands;
        }
    }

    static void GenerateTemplate(System::Windows::Forms::RichTextBox^);
    void LoadCommands();
    void SaveCommands();
    void DeleteCommands();
    static List<ICommand^>^ GetCommands() { return m_CommandData[m_CurrentTurn]->Commands; }
    void AddCommand(ICommand ^cmd);
    void AddCommandDontSave(ICommand ^cmd);
    void DelCommand(ICommand ^cmd);

    static List<String^>^ PrintSystemStatus(StarSystem^ system, bool listIncomplete);

    static property bool AutoEnabled
    {
        bool get() { return m_CommandData[m_CurrentTurn]->AutoEnabled; };
        void set(bool v) { m_CommandData[m_CurrentTurn]->AutoEnabled = v; };
    }

    void        SetAutoOrderPreDeparture(StarSystem^, String^);
    void        SetAutoOrderJumps(Ship^, String^);
    void        SetAutoOrderProduction(Colony^, String^, int);

    static List<String^>^  GetAutoOrdersPreDeparture(StarSystem^);
    static List<String^>^  GetAutoOrdersJumps(Ship^);
    static List<Pair<String^, int>^>^  GetAutoOrdersProduction(Colony^);

private:

    void        SortCommands();

    static void GenerateCombat();
    static void GeneratePreDeparture();
    static void GenerateJumps();
    static void GenerateProduction();
    static void GeneratePostArrival();
    static void GenerateStrikes();

    static void GenerateCombatInfo(StarSystem^);
    static void GeneratePreDepartureInfo(StarSystem^);
    static void GenerateJumpInfo(Ship^);
    static void GenerateScanOrders();
    static void GenerateProductionRecycle(Colony ^colony, BudgetTracker ^budget);
    static void GenerateProductionUpgrade(Colony ^colony, BudgetTracker ^budget);

    static List<String^>^           m_OrderList;
    static String^                  m_Path;
    static GameData^                m_GameData;
    RegexMatcher^                   m_RM;

    static int m_CurrentTurn;
    static SortedList<int, TurnCommands^>^ m_CommandData;
};

} // end namespace FHUI
