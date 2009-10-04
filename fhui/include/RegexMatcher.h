#pragma once

using namespace System;
using namespace System::Text::RegularExpressions;

namespace FHUI
{

private ref class RegexMatcher
{
public:
    RegexMatcher();

    bool        Match(String ^%s, String ^exp);
    bool        Match(String ^%s, Regex ^exp);
    bool        MatchList(String ^s, String ^prefix, String ^exp);

    int         GetResultInt(int arg);
    //float       GetResultFloat(int arg);

    property array<String^>^ Results { array<String^>^ get() { return m_Results; } }

private:
    array<String^>^     m_Results;

public:
    // --------------------------
    // Commands
    initonly Regex^     ExpCmdColony;
    initonly Regex^     ExpCmdShipJump;
    initonly Regex^     ExpCmdShipUpg;
    initonly Regex^     ExpCmdShipRec;
    initonly Regex^     ExpCmdShipWormhole;
    initonly Regex^     ExpCmdPLName;
    initonly Regex^     ExpCmdPLDisband;
    initonly Regex^     ExpCmdSPNeutral;
    initonly Regex^     ExpCmdSPAlly;
    initonly Regex^     ExpCmdSPEnemy;
    initonly Regex^     ExpCmdSPTeach;
    initonly Regex^     ExpCmdResearch;
    initonly Regex^     ExpCmdBuildIUAU;
    initonly Regex^     ExpCmdBuildShipTR;
    initonly Regex^     ExpCmdBuildShip;
};

} // end namespace FHUI
