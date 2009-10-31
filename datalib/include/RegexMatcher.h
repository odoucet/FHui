#pragma once

using namespace System;
using namespace System::Text::RegularExpressions;

namespace FHUI
{

public ref class RegexMatcher
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
    initonly Regex^     ExpCmdShip;
    initonly Regex^     ExpCmdShipJump;
    initonly Regex^     ExpCmdShipMove;
    initonly Regex^     ExpCmdShipUpg;
    initonly Regex^     ExpCmdShipRec;
    initonly Regex^     ExpCmdShipWormhole;
    initonly Regex^     ExpCmdShipUnload;
    initonly Regex^     ExpCmdShipScan;
    initonly Regex^     ExpCmdShipDeep;
    initonly Regex^     ExpCmdShipLand;
    initonly Regex^     ExpCmdShipOrbitPLName;
    initonly Regex^     ExpCmdShipOrbitPLNum;
    initonly Regex^     ExpCmdPLName;
    initonly Regex^     ExpCmdPLDisband;
    initonly Regex^     ExpCmdSPNeutral;
    initonly Regex^     ExpCmdSPAlly;
    initonly Regex^     ExpCmdSPEnemy;
    initonly Regex^     ExpCmdSPTeach;
    initonly Regex^     ExpCmdSPMsg;
    initonly Regex^     ExpCmdResearch;
    initonly Regex^     ExpCmdRecycle;
    initonly Regex^     ExpCmdEstimate;
    initonly Regex^     ExpCmdDevelopCS;
    initonly Regex^     ExpCmdDevelopC;
    initonly Regex^     ExpCmdDevelop;
    initonly Regex^     ExpCmdInstall;
    initonly Regex^     ExpCmdBuildIUAU;
    initonly Regex^     ExpCmdBuildShipTR;
    initonly Regex^     ExpCmdBuildShip;
    initonly Regex^     ExpCmdVisited;
    initonly Regex^     ExpCmdTransfer;
    initonly Regex^     ExpCmdCustom;

    initonly Regex^     ExpCmdTargetColony;
    initonly Regex^     ExpCmdTargetShip;

    initonly Regex^     ExpCmdShipJump_Obsolete;
    initonly Regex^     ExpCmdShipUpg_Obsolete;
    initonly Regex^     ExpCmdShipRec_Obsolete;
    initonly Regex^     ExpCmdShipWormhole_Obsolete;
};

} // end namespace FHUI
