#pragma once

#include "GameData.h"
#include <string>

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
    PHASE_ORDERS_TEMPLATE,
    PHASE_TECH_LEVELS,
};

public ref class Report
{
public:
    Report(GameData^);

    bool            IsValid();
    int             GetTour()       { return m_Tour; }
    String^         GetContent()    { return m_Content; }
    String^         GetSummary();

    bool            Parse(const std::string &line);

private:
    String^         GetSpeciesSummary();
    String^         GetAllTechsSummary();
    String^         GetTechSummary(TechType tech);
    String^         GetFleetSummary();
    String^         GetAliensSummary();

    void            StartLineAggregate(PhaseType, String ^s);
    String^         FinishLineAggregate();

    bool            MatchTech(String ^s, String ^techName, TechType tech);
    bool            MatchWithOutput(String ^s, String ^exp);
    bool            MatchAggregateList(String ^s, String ^prefix, String ^exp);

    String^         GetMatchResult(int arg)      { return m_TmpRegexResult[arg]; }
    int             GetMatchResultInt(int arg);
    float           GetMatchResultFloat(int arg);

    GameData       ^m_GameData;
    String         ^m_Content;
    int             m_Tour;

    PhaseType       m_Phase;
    array<String^> ^m_TmpRegexResult;

    bool            m_bParsingAggregate;
    String         ^m_StringAggregate;
};
