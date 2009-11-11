#pragma once

#include "enums.h"

#define GET_LINE() GetLine(__FUNCTION__, __LINE__)
#define GET_NON_EMPTY_LINE() GetNonEmptyLine(__FUNCTION__, __LINE__)
#define EXPECTED_EMPTY_LINE() ExpectedEmptyLine(__FUNCTION__, __LINE__)
#define GET_MERGED_LINES() GetMergedLines(__FUNCTION__, __LINE__)

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
    PHASE_NONE = 0,
    PHASE_GLOBAL,
    PHASE_LOG_USER, 
    PHASE_LOG_COMBAT,
    PHASE_LOG_PRE_DEP,
    PHASE_LOG_JUMP,
    PHASE_LOG_PROD,
    PHASE_LOG_POST_ARRIVAL,
    PHASE_LOG_STRIKE,
    PHASE_LOG_OTHER,
    PHASE_SPECIES_STATUS,
    PHASE_COLONIES,
    PHASE_TEMPLATE,
    PHASE_MAX
};

private ref class Report
{
public:
    Report(GameData^, CommandManager^, RegexMatcher^, bool);

    int             GetTurn()       { return m_Turn; }
    int             GetLineCount()  { return m_Content->Count; }
    String^         GetText();

    bool            Verify(String ^filename);
    void            Parse(String ^filename);

private:

    bool            ParseInternal(String^ s);

    String^         GetLine();
    String^         GetLine( String^ function, int line );
    void            ExpectedEmptyLine( String^ srcFun, int srcLine );
    String^         GetNonEmptyLine( String^ srcFun, int srcLine );
    String^         GetMergedLines( String^ srcFun, int srcLine );

    bool            MatchPhaseNone(String ^s);
    bool            MatchPhaseLogUser(String ^s);
    bool            MatchPhaseLogCombat(String ^s);
    bool            MatchPhaseLogPreDep(String ^s);
    bool            MatchPhaseLogJump(String ^s);
    bool            MatchPhaseLogProd(String ^s);
    bool            MatchPhaseLogPostArr(String ^s);
    bool            MatchPhaseLogStrike(String ^s);
    bool            MatchPhaseLogOther(String ^s);
    bool            MatchPhaseSpeciesStatus(String ^s);
    bool            MatchPhaseColonies(String ^s);
    bool            MatchPhaseTemplate(String ^s);

    bool            MatchSectionEnd(String ^s);
    bool            MatchSystemScan(String ^s);
    bool            MatchPlanetScan(String ^s, StarSystem^ system);
    bool            MatchAlienEstimate(String ^s);
    bool            MatchMessageReceived(String ^s);
    bool            MatchMessageSent(String ^s);
    bool            MatchTechLevels(String ^s);
    bool            MatchTech(String ^s, String ^techName, TechType tech);
    bool            MatchAtmReq(String ^s);
    bool            MatchSpeciesMet(String ^s);
    bool            MatchAllies(String ^s);
    bool            MatchEnemies(String ^s);
    bool            MatchColonyInfo(String ^s);
    bool            MatchColonyInventory(String ^s, Colony ^colony);
    bool            MatchColonyShips(String ^s, Colony ^colony);
    bool            MatchShipInfo(String ^s, StarSystem^ system, Colony ^colony);
    bool            MatchOtherPlanetsShips(String ^s);
    bool            MatchAliens(String ^s);
    bool            MatchTemplateEntry(String ^s);

    void            MatchAlienInfo(String ^s, Alien ^alien);

    GameData^       m_GameData;
    CommandManager^ m_CommandMgr;
    RegexMatcher^   m_RM;
    int             m_Turn;

    bool            m_Verbose;

    PhaseType       m_Phase;

    int             m_PirateShipsCnt;
    CommandPhase    m_TemplatePhase;
    Colony^         m_TemplateColony;
    Alien^          m_EstimateAlien;

    StreamReader^   m_Input;

    String^         PhaseToString(PhaseType phase);

    List<String^>^  m_Content;

};

} // end namespace FHUI
