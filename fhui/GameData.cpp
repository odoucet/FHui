#include "StdAfx.h"
#include "GameData.h"

// ---------------------------------------------------------

int Planet::CalculateLSN(AtmosphericReq ^atmReq, int tc, int pc)
{
    int lsn =
        3 * Math::Abs(m_PressClass - pc) +
        3 * Math::Abs(m_TempClass - tc);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( m_Atmosphere[i] > 0 && atmReq->m_Poisonous[i] )
            lsn += 3;
    }

    if( m_Atmosphere[atmReq->m_ReqGas] < atmReq->m_ReqMin ||
        m_Atmosphere[atmReq->m_ReqGas] > atmReq->m_ReqMax )
    {
        lsn += 3;
    }

    return lsn;
}

// ---------------------------------------------------------

GameData::GameData(void)
{
    m_SpeciesName       = nullptr;
    m_AtmReq            = gcnew AtmosphericReq;
    m_TourMax           = 0;
    m_TechLevels        = gcnew array<int>(TECH_MAX){0};
    m_TechLevelsTeach   = gcnew array<int>(TECH_MAX){0};
    m_FleetCost         = 0;
    m_FleetCostPercent  = 0.0;
    m_Aliens            = gcnew SortedList;
}

// ---------------------------------------------------------

void GameData::GetTechLevel(TechType tech, int %lev, int %levTeach)
{
    lev = m_TechLevels[tech];
    levTeach = m_TechLevelsTeach[tech];
}

void GameData::GetFleetCost(int %cost, float %percent)
{
    cost = m_FleetCost;
    percent = m_FleetCostPercent;
}

Alien^ GameData::GetAlien(String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        return safe_cast<Alien^>
            ( m_Aliens->GetByIndex( m_Aliens->IndexOfKey(spKey) ) );
    }

    throw gcnew NullReferenceException(
        String::Format("Species not found: {0}", sp));
}

// ---------------------------------------------------------

int GameData::TourAlign(int tour)
{
    if( tour == 0 )
    {
        return TOUR_NOTES;
    }
    return tour;
}

void GameData::SetSpecies(String ^sp)
{
    if( m_SpeciesName == nullptr )
        m_SpeciesName = sp;

    if( String::Compare(m_SpeciesName, sp) != 0 )
        throw gcnew ArgumentException(
            String::Format("Reports for different species: {0} and {1}",
                m_SpeciesName, sp) );
}

Alien^ GameData::AddAlien(int tour, String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        Alien ^alien = safe_cast<Alien^>
            ( m_Aliens->GetByIndex( m_Aliens->IndexOfKey(spKey) ) );

        alien->m_TourMet = Math::Min(TourAlign(tour), alien->m_TourMet);

        // Reset relation if this is latest tour,
        // it will self tune while parsing Allies and Enemies.
        if( tour == m_TourMax )
            alien->m_Relation = SP_NEUTRAL;

        return alien;
    }

    Alien ^alien = gcnew Alien(sp, tour);
    m_Aliens->Add(spKey, alien);

    return alien;
}

void GameData::SetAlienRelation(int tour, String ^sp, SPRelType rel)
{
    Alien ^alien = AddAlien(tour, sp);
    alien->m_Relation = rel;
}

void GameData::SetTechLevel(int tour, TechType tech, int lev, int levTeach)
{
    if( tour >= m_TourMax )
    {
        m_TourMax = tour;

        m_TechLevels[tech] = lev;
        m_TechLevelsTeach[tech] = levTeach;
    }
}

void GameData::SetFleetCost(int tour, int cost, float percent)
{
    if( tour >= m_TourMax )
    {
        m_TourMax = tour;

        m_FleetCost = cost;
        m_FleetCostPercent = percent;
    }
}

void GameData::SetAtmosphereReq(GasType gas, int reqMin, int reqMax)
{
    bool atmCheck = true;

    if( (m_AtmReq->m_ReqGas != GAS_MAX && m_AtmReq->m_ReqGas != gas) ||
        m_AtmReq->m_Neutral[gas] ||
        m_AtmReq->m_Poisonous[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    m_AtmReq->m_ReqGas = gas;
    m_AtmReq->m_ReqMin = reqMin;
    m_AtmReq->m_ReqMax = reqMax;
}

void GameData::SetAtmosphereNeutral(GasType gas)
{
    if( m_AtmReq->m_ReqGas == gas ||
        m_AtmReq->m_Poisonous[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    m_AtmReq->m_Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    if( m_AtmReq->m_ReqGas == gas ||
        m_AtmReq->m_Neutral[gas] )
    {
        throw gcnew ArgumentException("Inconsistent atmospheric data in reports.");
    }

    m_AtmReq->m_Poisonous[gas] = true;
}

void GameData::AddPlanetScan(int tour, int x, int y, int z, int plNum, Planet ^planet)
{
    //...
}
