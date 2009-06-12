#pragma once

#include "enums.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;


public ref class AtmosphericReq
{
public:
    AtmosphericReq()
        : m_ReqGas(GAS_MAX)
        , m_ReqMin(0)
        , m_Neutral(gcnew array<bool>(GAS_MAX) )
        , m_Poisonous(gcnew array<bool>(GAS_MAX) )
    {}

    GasType             m_ReqGas;
    int                 m_ReqMin;
    int                 m_ReqMax;
    array<bool>        ^m_Neutral;
    array<bool>        ^m_Poisonous;
};

public ref class Alien
{
public:
    Alien(String ^name, int tour)
        : m_Name(name)
        , m_Relation(SP_NEUTRAL)
        , m_TourMet(tour)
    {}

    String         ^m_Name;
    SPRelType       m_Relation;
    int             m_TourMet;
};

public ref class Planet
{
public:
    Planet(int dia, float gv, int tc, int pc, float md)
        : m_Diameter(dia)
        , m_Grav(gv)
        , m_TempClass(tc)
        , m_PressClass(pc)
        , m_MiningDiff(md)
        , m_LSN(-1)
        , m_Atmosphere(gcnew array<int>(GAS_MAX))
        , m_Name(nullptr)
        , m_Comment(nullptr)
    {}

    int CalculateLSN(AtmosphericReq^, int tc, int pc);

    int         m_Diameter;
    float       m_Grav;
    int         m_TempClass;
    int         m_PressClass;
    float       m_MiningDiff;
    int         m_LSN;
    array<int> ^m_Atmosphere;

    String     ^m_Name;
    String     ^m_Comment;
};

public ref class StarSystem
{
public:
    StarSystem(int x, int y, int z, String ^type)
        : m_X(x)
        , m_Y(y)
        , m_Z(z)
        , m_Type(type)
        , m_TourScanned(-1)
        , m_Planets(gcnew array<Planet^>(0))
    {
    }

    int             m_X;
    int             m_Y;
    int             m_Z;
    String         ^m_Type;

    int             m_TourScanned;
    array<Planet^> ^m_Planets;
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
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);
    void            SetTechLevel(int tour, TechType, int, int);
    void            SetFleetCost(int tour, int, float);
    Alien^          AddAlien(int tour, String ^sp);
    void            SetAlienRelation(int tour, String ^sp, SPRelType);
    void            AddPlanetScan(int tour, int x, int y, int z, int plNum, Planet ^planet);

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
