#pragma once

using namespace System::Collections::Generic;

namespace FHUI
{

ref class CommandManager;
ref class RegexMatcher;

private ref class ReportParser
{
public:
    ReportParser(GameData^, CommandManager^, String^, String^);

    property SortedList<int, String^>^ Reports
    {
        SortedList<int, String^>^ get() { return m_Reports; }
    }

    void        ScanReports();

private:
    void        LoadGalaxy();
    void        LoadReports( int turn );
    int         VerifyReport( String ^fileName );

    String^                         m_GalaxyPath;
    String^                         m_ReportPath;
    RegexMatcher^                   m_RM;
    GameData^                       m_GameData;
    CommandManager^                 m_CommandMgr;
    SortedList<int, String^>^       m_Reports;
    SortedList<String^, int>^       m_RepFiles;

};

} // end namespace FHUI
