#pragma once

#include "Report.h"

using namespace System::Collections::Generic;

namespace FHUI
{

ref class CommandManager;
ref class RegexMatcher;

private ref class ReportParser
{
public:
    ReportParser(GameData^, CommandManager^, RegexMatcher^, String^, String^);

    property bool Verbose;
    property bool Stats;

    property SortedList<int, Report^>^ Reports
    {
        SortedList<int, Report^>^ get() { return m_Reports; }
    }

    void        ScanReports();

private:
    void        LoadGalaxy();
    void        LoadReports( int turn );

    String^                         m_GalaxyPath;
    String^                         m_ReportPath;
    RegexMatcher^                   m_RM;
    GameData^                       m_GameData;
    CommandManager^                 m_CommandMgr;
    SortedList<int, Report^>^       m_Reports;
    SortedList<String^, int>^       m_RepFiles;
};

} // end namespace FHUI
