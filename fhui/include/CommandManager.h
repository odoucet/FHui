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

private ref class CommandManager
{
public:
    CommandManager(GameData^, String^);

    // ==================================================
    // --- ORDER TEMPLATE ---
    void        GenerateTemplate(System::Windows::Forms::RichTextBox^);
    void        SaveCommands();
    void        DeleteCommands();
    Colony^     LoadCommands();
    void        AddCommand(ICommand ^cmd);
    void        DelCommand(ICommand ^cmd);

    List<String^>^ PrintSystemStatus(StarSystem^ system, bool listIncomplete);

private:

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

    String^                         m_Path;
    GameData^                       m_GameData;
    RegexMatcher^                   m_RM;
    List<String^>^                  m_OrderList;
};

} // end namespace FHUI
