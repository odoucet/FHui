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
        : m_Planets(gcnew array<Planet^>(0))
        , m_TourScanned(-1)
        , m_Distance(0)
        , m_Mishap(0)
    {
        X = x;
        Y = y;
        Z = z;
        Type = type;
    }

    property int        X;
    property int        Y;
    property int        Z;
    property String^    Type;
    property String^    NumPlanets
    {
        String^ get()
        {
            if( m_TourScanned == -1 )
                return "?";
            int minLSN = 99999;
            for each( Planet ^pl in m_Planets )
                minLSN = Math::Min(minLSN, pl->m_LSN == -1 ? 99999 : pl->m_LSN);
            return String::Format("{0} ({1})", m_Planets->Length, minLSN);
        }
    }
    property String^    ScanTour
    {
        String^ get()
        {
            if( m_TourScanned == -1 )       return "Not scanned";
            else if( m_TourScanned == 0 )   return "Received";
            else                            return String::Format("Scaned, {0}", m_TourScanned);
        }
    }
    property String^    Distance
    {
        String^ get() { return String::Format("{0:F2}", m_Distance); }
    }
    property String^    Mishap
    {
        String^ get() { return String::Format("{0:F2}% ({1:F2}%)", m_Mishap, m_Mishap * m_Mishap / 100.0); }
    }

    array<Planet^>     ^m_Planets;
    int                 m_TourScanned;
    double              m_Distance;
    double              m_Mishap;
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
    SortedList^     GetAliens()                 { return m_Aliens; }
    StarSystem^     GetStarSystem(int x, int y, int z);
    array<StarSystem^>^ GetStarSystems()        { return m_Systems; }

    // ------------------------------------------
    void            SetSpecies(String ^sp);
    void            SetAtmosphereReq(GasType gas, int, int);
    void            SetAtmosphereNeutral(GasType gas);
    void            SetAtmospherePoisonous(GasType gas);
    void            SetTechLevel(int tour, TechType, int, int);
    void            SetFleetCost(int tour, int, float);
    Alien^          AddAlien(int tour, String ^sp);
    void            SetAlienRelation(int tour, String ^sp, SPRelType);
    void            AddStarSystem(int x, int y, int z, String ^type);
    void            AddPlanetScan(int tour, int x, int y, int z, int plNum, Planet ^planet);

    // ------------------------------------------
protected:
    int             TourAlign(int tour);

    // ------------------------------------------
    String             ^m_SpeciesName;
    AtmosphericReq     ^m_AtmReq;
    array<int>         ^m_TechLevels;
    array<int>         ^m_TechLevelsTeach;
    int                 m_FleetCost;
    float               m_FleetCostPercent;
    SortedList         ^m_Aliens;
    array<StarSystem^> ^m_Systems;

    int                 m_TourMax;
};
