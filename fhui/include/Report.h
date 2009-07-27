#pragma once

#include "enums.h"

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
    float       GetResultFloat(int arg);

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
    initonly Regex^     ExpCmdPLName;
    initonly Regex^     ExpCmdPLDisband;
    initonly Regex^     ExpCmdSPNeutral;
    initonly Regex^     ExpCmdSPAlly;
    initonly Regex^     ExpCmdSPEnemy;
    initonly Regex^     ExpCmdSPTeach;
};

enum PhaseType
{
    PHASE_FILE_DETECT,
    PHASE_GLOBAL,
    PHASE_ORDERS_COMBAT,
    PHASE_ORDERS_PRE_DEP,
    PHASE_ORDERS_JUMP,
    PHASE_ORDERS_PROD,
    PHASE_ORDERS_POST_ARRIVAL,
    PHASE_ORDERS_STRIKE,
    PHASE_MESSAGE,
    PHASE_SPECIES_MET,
    PHASE_SPECIES_ALLIES,
    PHASE_SPECIES_ENEMIES,
    PHASE_SYSTEM_SCAN,
    PHASE_ALIEN_ESTIMATE,
    PHASE_COLONY,
    PHASE_COLONY_INVENTORY,
    PHASE_COLONY_SHIPS,
    PHASE_OTHER_PLANETS_SHIPS,
    PHASE_ALIENS_REPORT,
    PHASE_ORDERS_TEMPLATE,
    PHASE_TECH_LEVELS,
};

#define AGGREGATE_LINES_MAX  -1

private ref class Report
{
public:
    Report(GameData^, RegexMatcher^);

    bool            IsValid();
    int             GetTurn()       { return m_Turn; }
    String^         GetContent()    { return m_Content; }

    bool            Parse(String ^s);
    int             GetLineCount()  { return m_LineCnt; }

private:
    void            StartLineAggregate(PhaseType, String ^s, int aggrMaxLines);
    String^         FinishLineAggregate(bool resetPhase);

    bool            MatchSectionEnd(String ^s);
    bool            MatchTech(String ^s, String ^techName, TechType tech);
    bool            MatchSystemScanStart(String ^s);
    void            MatchPlanetScan(String ^s);
    void            MatchColonyScan(String ^s);
    void            MatchColonyInventoryScan(String ^s);
    void            MatchColonyShipsScan(String ^s);
    void            MatchOtherPlanetsShipsScan(String ^s);
    void            MatchShipScan(String ^s, bool bColony);
    void            MatchAliensReport(String ^s);

    GameData       ^m_GameData;
    RegexMatcher   ^m_RM;
    String         ^m_Content;
    int             m_Turn;
    int             m_LineCnt;

    PhaseType       m_Phase;
    PhaseType       m_PhasePreAggregate;

    bool            m_bParsingAggregate;
    String         ^m_StringAggregate;
    int             m_AggregateMaxLines;

    int             m_ScanX;
    int             m_ScanY;
    int             m_ScanZ;
    bool            m_ScanHasPlanets;
    Alien          ^m_ScanAlien;
    Colony         ^m_ScanColony;
    Ship           ^m_ScanShip;
    int             m_PirateShipsCnt;

    Alien          ^m_EstimateAlien;
};

} // end namespace FHUI
