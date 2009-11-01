#pragma once

#include "enums.h"

using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Text::RegularExpressions;

namespace FHUI
{

ref class RegexMatcher;
ref class CommandManager;

enum PhaseType
{
    PHASE_INIT = 0,
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
    PHASE_MAX
};

#define AGGREGATE_LINES_MAX  -1

private ref class Report
{
public:
    Report(GameData^, CommandManager^, RegexMatcher^, bool);

    bool            IsValid();
    int             GetTurn()       { return m_Turn; }
    int             GetLineCount()  { return m_Content->Count; }
    String^         GetText();

    bool            Verify(String ^filename);
    void            Parse(String ^filename);

private:

    bool            ParseInternal(String^ s);

    String^         GetLine();

    bool            MatchPhaseGlobal(String ^s);
    bool            MatchPhaseMessage(String ^s);
    bool            MatchPhaseSpeciesMet(String ^s);
    bool            MatchPhaseSpeciesAllies(String ^s);
    bool            MatchPhaseSpeciesEnemies(String ^s);
    bool            MatchPhaseOrdersCombat(String ^s);
    bool            MatchPhaseOrdersPreDep(String ^s);
    bool            MatchPhaseOrdersJump(String ^s);
    bool            MatchPhaseOrdersProd(String ^s);
    bool            MatchPhaseOrdersPostArr(String ^s);
    bool            MatchPhaseOrdersStrike(String ^s);
    bool            MatchPhaseTechLevels(String ^s);
    bool            MatchPhaseAlienEstimate(String ^s);
    bool            MatchPhaseSystemScan(String ^s);
    bool            MatchPhaseColony(String ^s);
    bool            MatchPhaseColonyInventory(String ^s);
    bool            MatchPhaseColonyShips(String ^s);
    bool            MatchPhaseOtherPlanetsShips(String ^s);
    bool            MatchPhaseAliensReport(String ^s);
    bool            MatchPhaseOrdersTemplate(String ^s);

    bool            m_Verbose;

    void            StartLineAggregate(PhaseType, String ^s, int aggrMaxLines);
    String^         FinishLineAggregate(bool resetPhase);

    bool            MatchSectionEnd(String ^s);
    bool            MatchTech(String ^s, String ^techName, TechType tech);
    bool            MatchMessageSent(String ^s);
    bool            MatchSystemScanStart(String ^s);
    void            MatchPlanetScan(String ^s);
    void            MatchColonyScan(String ^s);
    void            MatchColonyInventoryScan(String ^s);
    void            MatchColonyShipsScan(String ^s);
    void            MatchOtherPlanetsShipsScan(String ^s);
    void            MatchShipScan(String ^s, bool bColony);
    void            MatchAliensReport(String ^s);
    void            MatchOrdersTemplate(String ^s);
    void            MatchAlienInfo(String ^s, Alien ^alien);

    GameData^       m_GameData;
    CommandManager^ m_CommandMgr;
    RegexMatcher^   m_RM;
    List<String^>^  m_Content;
    int             m_Turn;

    PhaseType       m_Phase;
    PhaseType       m_PhasePreScan;
    PhaseType       m_PhasePreAggregate;

    bool            m_bParsingAggregate;
    String^         m_StringAggregate;
    int             m_AggregateMaxLines;

    int             m_ScanX;
    int             m_ScanY;
    int             m_ScanZ;
    bool            m_ScanHasPlanets;
    StarSystem^     m_ScanSystem;
    Alien^          m_ScanAlien;
    String^         m_ScanMessage;
    Colony^         m_ScanColony;
    Ship^           m_ScanShip;
    int             m_PirateShipsCnt;

    CommandPhase    m_TemplatePhase;

    Colony^         m_ColonyProduction;
    Alien^          m_EstimateAlien;

    StreamReader^   m_Input;

    String^         PhaseToString(PhaseType phase);
};

} // end namespace FHUI
