#pragma once

#include "GameData.h"

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
    PHASE_SPECIES_MET,
    PHASE_SPECIES_ALLIES,
    PHASE_SPECIES_ENEMIES,
    PHASE_SYSTEM_SCAN,
    PHASE_COLONY,
    PHASE_COLONY_INVENTORY,
    PHASE_COLONY_SHIPS,
    PHASE_ORDERS_TEMPLATE,
    PHASE_TECH_LEVELS,
};

public ref class Report
{
public:
    Report(GameData^);

    bool            IsValid();
    int             GetTurn()       { return m_Turn; }
    String^         GetContent()    { return m_Content; }

    bool            Parse(String ^s);

private:
    void            StartLineAggregate(PhaseType, String ^s);
    String^         FinishLineAggregate();

    bool            MatchSectionEnd(String ^s);
    bool            MatchWithOutput(String ^%s, String ^exp);
    bool            MatchAggregateList(String ^s, String ^prefix, String ^exp);
    bool            MatchTech(String ^s, String ^techName, TechType tech);
    bool            MatchSystemScanStart(String ^s);
    void            MatchPlanetScan(String ^s);
    void            MatchColonyScan(String ^s);
    void            MatchColonyInventoryScan(String ^s);
    void            MatchColonyShipsScan(String ^s);

    String^         GetMatchResult(int arg)      { return m_TmpRegexResult[arg]; }
    int             GetMatchResultInt(int arg);
    float           GetMatchResultFloat(int arg);

    GameData       ^m_GameData;
    String         ^m_Content;
    int             m_Turn;

    PhaseType       m_Phase;
    array<String^> ^m_TmpRegexResult;

    bool            m_bParsingAggregate;
    String         ^m_StringAggregate;

    int             m_ScanX;
    int             m_ScanY;
    int             m_ScanZ;
    bool            m_ScanHasPlanets;
    Alien          ^m_ScanHome;
    Colony         ^m_ScanColony;
};
