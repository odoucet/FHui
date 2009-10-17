#pragma once

using namespace System;
using namespace System::Collections::Generic;

#include "Commands.h"

namespace FHUI
{

ref class GameData;
ref class Colony;
ref class Ship;
ref class RegexMatcher;
ref class BudgetTracker;

public value struct OrdersDir
{
    static initonly String^ Folder   = "orders/FHUI.Data/";
    static initonly String^ Commands = "cmd_t{0}.txt";
};

public ref class TurnCommands
{
public:
    TurnCommands()
        : Commands(gcnew List<ICommand^>)
        , AutoEnabled(false)
        , AutoOrdersProduction(gcnew SortedList<Colony^, List<Pair<String^, int>^>^>)
    {
    }

    List<ICommand^>^                                 Commands;

    bool                                             AutoEnabled;
    SortedList<Colony^, List<Pair<String^, int>^>^>^ AutoOrdersProduction;
};

public ref class CommandManager
{
public:
    CommandManager(GameData^, String^);

    void        SelectTurn(int turn);

    void        LoadCommands();
    void        SaveCommands();
    void        DeleteCommands();

    String^     PrintCommandWithInfo(ICommand ^cmd, int indent);

    void        AddCommand(ICommand ^cmd);
    void        AddCommandDontSave(ICommand ^cmd);
    void        DelCommand(ICommand ^cmd);
    List<ICommand^>^ GetCommands() { return m_CommandData[m_CurrentTurn]->Commands; }

    // For production commands
    void        AddCommand(Colony ^colony, ICommand ^cmd);
    void        DelCommand(Colony ^colony, ICommand ^cmd);
    List<ICommand^>^ GetCommands(Colony ^colony);

    void        GenerateTemplate(System::Windows::Forms::RichTextBox^);

    void        RemoveGeneratedCommands(CommandOrigin origin, bool preserveScouting);

    property bool AutoEnabled
    {
        bool get() { return m_CommandData[m_CurrentTurn]->AutoEnabled; };
        void set(bool v) { m_CommandData[m_CurrentTurn]->AutoEnabled = v; };
    }

    void        SetAutoOrderProduction(Colony^, String^, int);

    List<Pair<String^, int>^>^  GetAutoOrdersProduction(Colony^);

private:
    bool        LoadCommandsColony(String ^line, Colony ^colony);
    bool        LoadCommandsShip(String ^line, Ship ^ship);
    void        SortCommands();

    ICommand^   CmdSetOrigin(ICommand ^cmd);
    String^     PrintCommandToFile(ICommand ^cmd);

    void        RemoveGeneratedCommandsFromList( List<ICommand^> ^orders, CommandOrigin origin, bool preserveScouting );
    void        AddPluginCommands();

    void        GenerateCombat();
    void        GeneratePreDeparture();
    void        GenerateJumps();
    void        GenerateProduction();
    void        GeneratePostArrival();
    void        GenerateStrikes();

    List<String^>^ PrintSystemStatus(StarSystem^ system, bool listIncomplete);

    void        GenerateCombatInfo(StarSystem^);
    void        GeneratePreDepartureInfo(StarSystem^);
    void        GenerateJumpInfo(Ship^);
    void        GenerateProductionRecycle(Colony ^colony);
    void        GenerateProductionUpgrade(Colony ^colony);

    List<String^>^          m_OrderList;
    BudgetTracker^          m_Budget;
    String^                 m_Path;
    GameData^               m_GameData;
    RegexMatcher^           m_RM;

    CommandOrigin           m_CmdOrigin;

    int                     m_CurrentTurn;
    SortedList<int, TurnCommands^>^ m_CommandData;
};

} // end namespace FHUI
