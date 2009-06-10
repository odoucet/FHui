#pragma once

#include "enums.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;


public ref class AtmosphericReq
{
public:
    AtmosphericReq()
        : m_ReqGas(nullptr)
        , m_ReqMin(0)
        , m_Neutral(gcnew StringCollection)
        , m_Poisonous(gcnew StringCollection)
    {}

    String             ^m_ReqGas;
    int                 m_ReqMin;
    int                 m_ReqMax;
    StringCollection   ^m_Neutral;
    StringCollection   ^m_Poisonous;
};

public ref class Alien
{
public:
    Alien()
        : m_Name(gcnew String("???"))
        , m_Relation(SP_NEUTRAL)
        , m_TourMet(0)
    {}

    String         ^m_Name;
    SPRelType       m_Relation;
    int             m_TourMet;
};

public ref class GameData
{
public:
    GameData();

    String^         GetSpecies()                { return m_SpeciesName; }
    AtmosphericReq^ GetAtmosphere()             { return m_AtmReq; }
    void            GetTechLevel(TechType, int%, int%);
    void            GetFleetCost(int%, float%);
    Alien^          GetAlien(String ^sp);
    SortedList^     GetAliensAll()              { return m_Aliens; }

    // ------------------------------------------
    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(String ^gas, int, int);
    void            SetAtmosphereNeutral(String ^gas);
    void            SetAtmospherePoisonous(String ^gas);
    void            SetTechLevel(int tour, TechType, int, int);
    void            SetFleetCost(int tour, int, float);
    Alien^          AddAlien(int tour, String ^sp);
    void            SetAlienRelation(int tour, String ^sp, SPRelType);

    // ------------------------------------------
protected:
    int             TourAlign(int tour);

    // ------------------------------------------
    String         ^m_SpeciesName;
    AtmosphericReq ^m_AtmReq;
    array<int>     ^m_TechLevels;
    array<int>     ^m_TechLevelsTeach;
    int             m_FleetCost;
    float           m_FleetCostPercent;
    SortedList     ^m_Aliens;

    int             m_TourMax;
};
