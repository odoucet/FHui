#include "StdAfx.h"
#include "Report.h"

namespace FHUI
{

bool Report::MatchSectionEnd(String ^s)
{
    if( Regex("^(\\*\\s)+\\*\\s*$").Match(s)->Success )
        return true;
    return false;
}

bool Report::MatchMessageSent(String ^s)
{
    if( m_RM->Match(s, "^A message was sent to SP ([^,;]+)\\.$") )
    {
        m_GameData->AddAlien(m_RM->Results[0])->LastMessageSentTurn = m_GameData->CurrentTurn - 1;
        return true;
    }
    return false;
}

bool Report::MatchSystemScanStart(String ^s)
{
    if( m_RM->Match(s, "^Coordinates:\\s+[Xx]\\s+=\\s+(\\d+)\\s+[Yy]\\s+=\\s+(\\d+)\\s+[Zz]\\s+=\\s+(\\d+)") )
    {
        m_PhasePreScan = m_Phase;
        m_Phase = PHASE_SYSTEM_SCAN;
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);
        m_ScanHasPlanets = false;
        m_ScanSystem = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ, false);

        // Set the home system
        if( m_ScanAlien )
            m_ScanAlien->HomeSystem = m_ScanSystem;

        return true;
    }
    return false;
}

void Report::MatchPlanetScan(String ^s)
{
    if( m_RM->Match(s, "^This star system is the terminus of a natural wormhole\\.$") )
    {
        m_ScanSystem->HasWormhole = true;
    }
    else if( m_RM->Match(s, "^Wormhole to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$") )
    {
        m_ScanSystem->SetWormhole( GameData::GetSystemId(
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2) ) );
    }
    else if( s == "Wormhole to Unknown System" )
    {
        m_ScanSystem->HasWormhole = true;
    }
    //                          0:plNum   1:dia    2:gv            3:tc       4:pc    5:mining diff
    else if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\.(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.(\\d+)\\s+") )
    {
        int plNum = m_RM->GetResultInt(0);
        Planet ^planet = gcnew Planet(
            m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ, false),
            plNum,
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2) * 100 + m_RM->GetResultInt(3),
            m_RM->GetResultInt(4),
            m_RM->GetResultInt(5),
            m_RM->GetResultInt(6) * 100 + m_RM->GetResultInt(7) );

        // Try reading LSN
        if( m_RM->Match(s, "(\\d+)\\s+") )
        {
            planet->LSN = m_RM->GetResultInt(0);

            if( m_ScanAlien && planet->LSN == 0 )
            {
                m_ScanAlien->HomePlanet = plNum;
                m_ScanAlien->AtmReq->TempClass  = planet->TempClass;
                m_ScanAlien->AtmReq->PressClass = planet->PressClass;
            }
        }
        else if( m_RM->Match(s, "[x?\\-]+\\s+") )
        {
            // just skip from input
        }
        else
            throw gcnew FHUIParsingException("Report contains invalid planetary scan (LSN)");

        // Scan gases
        if( m_RM->Match(s, "^No atmosphere\\s*") )
        {
            // just skip from input
        }
        else
        {
            bool bGasMatched = false;
            while( m_RM->Match(s, ",?(\\w+)\\((\\d+)%\\)") )
            {
                bGasMatched = true;
                int gas = FHStrings::GasFromString(m_RM->Results[0]);
                planet->Atmosphere[gas] = m_RM->GetResultInt(1);
            }
            if( !bGasMatched )
                throw gcnew FHUIParsingException("Report contains invalid planetary scan (atmosphere)");
        }

        if( !String::IsNullOrEmpty(s) )
            planet->Comment = s;

        m_GameData->AddPlanetScan( m_ScanSystem, planet );
        m_ScanHasPlanets = true;
    }
}

bool Report::MatchTech(String ^s, String ^techName, TechType tech)
{
    String ^e2 = String::Format("{0} = (\\d+)/(\\d+)", techName);
    String ^e1 = String::Format("{0} = (\\d+)", techName);
    if( m_RM->Match(s, e2) )
    {
        m_GameData->SetTechLevel(
            GameData::Player,
            tech,
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1));
        return true;
    }
    if( m_RM->Match(s, e1) )
    {
        m_GameData->SetTechLevel(
            GameData::Player,
            tech,
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(0));
        return true;
    }
    return false;
}

void Report::MatchColonyScan(String ^s)
{
    if( m_bParsingAggregate )
        s = FinishLineAggregate(false);

    if( m_ScanColony == nullptr )
    {
        // Initial colony data
        PlanetType planetType = PLANET_HOME;
        if( m_RM->Match(s, "^(HOME|COLONY)\\s+PLANET: PL\\s+") )
        {
            if( m_RM->Results[0][0] == 'C' )
                planetType = PLANET_COLONY;
        }
        else if( m_RM->Match(s, "^(MINING|RESORT)\\s+COLONY: PL\\s+") )
        {
            if( m_RM->Results[0][0] == 'M' )
                planetType = PLANET_COLONY_MINING;
            else
                planetType = PLANET_COLONY_RESORT;
        }
        else
            throw gcnew FHUIParsingException(
                String::Format("Unknown colony type in report: {0}", s) );

        if( m_RM->Match(s, "([^,;]+)\\s+Coordinates: x = (\\d+), y = (\\d+), z = (\\d+), planet number (\\d+)") )
        {
            String ^plName = m_RM->Results[0];
            plName = plName->TrimEnd(' ');

            StarSystem ^system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                m_RM->GetResultInt(3),
                false );

            int plNum = m_RM->GetResultInt(4);
 
            m_ScanColony = m_GameData->AddColony(
                GameData::Player,
                plName,
                system,
                plNum,
                true );

            if( m_ScanColony == nullptr )
            {
                m_Phase = m_PhasePreAggregate;
                return;
            }

            m_ScanColony->PlanetType = planetType;
        }
        else
            throw gcnew FHUIParsingException(
                String::Format("Unable to parse colony coordinates: {0}", s) );

        return;
    }

    // Remaining entries
    switch( m_ScanColony->PlanetType )
    {
    case PLANET_HOME:
    case PLANET_COLONY:
        if( m_RM->Match(s, "^Total available for spending this turn = (\\d+) - (\\d+) = \\d+") )
        {
            m_ScanColony->Inventory[INV_RM] += (m_ScanColony->RMProduced - m_ScanColony->RMUsed);
            if( m_ScanColony->Inventory[INV_RM] < 0 )
                m_ScanColony->Inventory[INV_RM] = 0;

            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_ScanColony->EUProd );
        }
        break;

    case PLANET_COLONY_MINING:
        if( m_RM->Match(s, "^This mining colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->MaBase = 0;
            m_ScanColony->Shipyards = 0;
            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_ScanColony->EUProd );
        }
        break;

    case PLANET_COLONY_RESORT:
        if( m_RM->Match(s, "^This resort colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
        {
            m_ScanColony->MiBase = 0;
            m_ScanColony->Shipyards = 0;
            m_ScanColony->EUProd  = m_RM->GetResultInt(0);
            m_ScanColony->EUFleet = m_RM->GetResultInt(1);
            m_GameData->AddTurnProducedEU( m_ScanColony->EUProd );
        }
        break;
    }

    if( m_RM->Match(s, "^Economic efficiency = (\\d+)%") )
    {
        m_ScanColony->EconomicEff = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^IMPORTANT!  This planet is actively hiding from alien observation!") )
    {
        m_ScanColony->Hidden = true;
    }
    else if( m_RM->Match(s, "^WARNING!  This planet is currently under siege and will remain") )
    {
        m_ScanColony->UnderSiege = true;
    }
    else if ( m_RM->Match(s, "(\\d+) IUs and (\\d+) AUs will have to be installed for complete recovery.") )
    {
        m_ScanColony->RecoveryIU = m_RM->GetResultInt(0);
        m_ScanColony->RecoveryAU = m_RM->GetResultInt(1);
    }
    else if( m_RM->Match(s, "^Production penalty = (\\d+)% \\(LSN = (\\d+)\\)") )
    {
        m_ScanColony->ProdPenalty = m_RM->GetResultInt(0);
        m_ScanColony->LSN = m_RM->GetResultInt(1);
    }
    else if( m_RM->Match(s, "^Mining base = (\\d+)\\.(\\d+) \\(MI = \\d+, MD = (\\d+)\\.(\\d+)\\)") )
    {
        m_ScanColony->MiBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
        m_ScanColony->MiDiff = m_RM->GetResultInt(2) * 100 + m_RM->GetResultInt(3);
    }
    else if( m_RM->Match(s, "^(\\d+) raw material units will be produced this turn\\.$") )
    {
        m_ScanColony->RMProduced = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Raw Material Units \\(RM,C1\\) carried over from last turn = (\\d+)") )
    {
        m_ScanColony->Inventory[INV_RM] = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Manufacturing base = (\\d+)\\.(\\d+) \\(") )
    {
        m_ScanColony->MaBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
    }
    else if( m_RM->Match(s, "^Production capacity this turn will be (\\d+)\\.$") )
    {
        m_ScanColony->RMUsed = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Shipyard capacity = (\\d+)") )
    {
        m_ScanColony->Shipyards = m_RM->GetResultInt(0);
    }
    else if( m_RM->Match(s, "^Available population units = (\\d+)") )
    {
        m_ScanColony->AvailPop = m_RM->GetResultInt(0);
    }
    else if( Regex("^Planetary inventory:").Match(s)->Success )
        m_Phase = PHASE_COLONY_INVENTORY;
    else if( Regex("^Ships at PL [^,;]+:").Match(s)->Success )
        m_Phase = PHASE_COLONY_SHIPS;
}

void Report::MatchColonyInventoryScan(String ^s)
{
    if( m_RM->Match(s, "^[\\w\\s.-]+\\(PD,C3\\) = (\\d+) (warship equivalence = \\d+ tons)") )
        m_ScanColony->Inventory[INV_PD] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SU,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(CU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_CU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(IU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_IU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(AU,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_AU] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FS,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FS] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FD,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FD] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(DR,C1\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_DR] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FM,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FM] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FJ,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_FJ] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GW,C100\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GW] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GT,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GT] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(JP,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_JP] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(TP,C100\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_TP] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU1,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU1] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU2,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU2] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU3,C15\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU3] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU4,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU4] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU5,C25\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU5] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU6,C30\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU6] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU7,C35\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU7] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU8,C40\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU8] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU9,C45\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_GU9] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG1,C5\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG1] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG2,C10\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG2] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG3,C15\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG3] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG4,C20\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG4] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG5,C25\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG5] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG6,C30\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG6] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG7,C35\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG7] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG8,C40\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG8] = m_RM->GetResultInt(0);
    else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG9,C45\\) = (\\d+)") )
        m_ScanColony->Inventory[INV_SG9] = m_RM->GetResultInt(0);
}

void Report::MatchColonyShipsScan(String ^s)
{
    MatchShipScan(s, true);
}

void Report::MatchShipScan(String ^s, bool bColony)
{
    ShipType type   = SHIP_MAX;
    int size        = 0;
    bool subLight   = false;

    if( m_RM->Match(s, "^BAS\\s+") )
        type = SHIP_BAS;
    else if( m_RM->Match(s, "^TR(\\d+)([Ss]?)\\s+") ) 
    {
        type = SHIP_TR;
        size = m_RM->GetResultInt(0);
        subLight = 0 == String::Compare( m_RM->Results[1]->ToLower(), "s" );
    }
    else
    {
        try
        {
            if( m_RM->Match(s, "^([A-Za-z]{2})([Ss]?)\\s+") )
            {
                type = FHStrings::ShipFromString( m_RM->Results[0] );
                subLight = 0 == String::Compare( m_RM->Results[1]->ToLower(), "s" );
            }
            else
                return;
        }
        catch( FHUIParsingException^ )
        {
            return;
        }
    }

    bool bMatch     = false;
    bool bInFD      = false;
    bool bIncomplete= false;
    String ^name    = nullptr;
    int age         = 0;
    String ^loc     = nullptr;
    int capacity    = 0;
    int planetNum   = -1;
    ShipLocType location = SHIP_LOC_DEEP_SPACE;

    if( m_RM->Match(s, "^\\?\\?\\? \\(([DOLdol])") )
    {
        bMatch  = true;
        bInFD   = true;
        name    = String::Format("??? ({0})", ++m_PirateShipsCnt);
        loc     = m_RM->Results[0]->ToLower();
    }
    else if( m_RM->Match(s, "^([^,;]+) \\(") )
    {
        name    = m_RM->Results[0];
        if( m_RM->Match(s, "^C") )
        {
            bMatch  = true;
            age     = 0;
            bIncomplete = true;
        }
        else if( m_RM->Match(s, "^A(\\d+),([DOLdol])") )
        {
            bMatch  = true;
            age     = m_RM->GetResultInt(0);
            loc     = m_RM->Results[1]->ToLower();
        }
    }

    if( bMatch )
    {
        StarSystem ^system;
        if( bColony )
            system = m_ScanColony->System;
        else
        {
            // When reading other ships and planets,
            // ship may be outside of any system just in deep, empty space
            // add this location temporarily to star system list
            system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ, true);
        }
        system->LastVisited = m_Turn;

        if( bIncomplete )
        {
            location = SHIP_LOC_LANDED;
            if( bColony )
                planetNum = m_ScanColony->PlanetNum;
            else
                throw gcnew FHUIParsingException("Ship under construction outside colony???");
        }
        else
        {
            switch( loc[0] )
            {
            case 'd':
                location = SHIP_LOC_DEEP_SPACE;
                break;
            case 'o':
                location = SHIP_LOC_ORBIT;
                break;
            case 'l':
                location = SHIP_LOC_LANDED;
                break;
            default:
                throw gcnew FHUIParsingException("Invalid ship location: " + loc[0]);
            }

            if( location != SHIP_LOC_DEEP_SPACE )
            {
                if( m_RM->Match(s, "^(\\d+)") )
                    planetNum = m_RM->GetResultInt(0);
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' location.", name));
            }
        }

        if( type == SHIP_BAS )
        {
            if( m_RM->Match(s, "^,(\\d+) tons") )
                size = m_RM->GetResultInt(0);
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse starbase '{0}' size.", name));
        }

        if( bColony )
        {
            if( m_RM->Match(s, "^\\)\\s+(\\d+)\\s*") )
                capacity = m_RM->GetResultInt(0);
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse ship '{0}' capacity.", name));
        }
        else
        {
            if( !m_RM->Match(s, "^\\)\\s*") )
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse ship '{0}'.", name));
        }

        if( m_Phase != PHASE_ALIENS_REPORT )
        {
            m_ScanShip = m_GameData->AddShip( GameData::Player, type, name, subLight, system);
            if( m_ScanShip == nullptr )
                return;

            if( bIncomplete )
            {
                if( m_RM->Match(s, "^Left to pay = (\\d+)$") )
                {
                    int amount = m_RM->GetResultInt(0);
                    m_ScanShip->EUToComplete = amount;
                    m_ScanShip->CanJump = false;
                }
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' completion cost.", name));
            }

            while( !String::IsNullOrEmpty(s) )
            {
                if( m_RM->Match(s, "^,?\\s*(\\d+)\\s+(\\w+)\\s*") )
                {
                    int amount = m_RM->GetResultInt(0);
                    InventoryType inv = FHStrings::InvFromString( m_RM->Results[1] );
                    m_ScanShip->Cargo[inv] = amount;
                }
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' inventory.", name));
            }
        }
        else
        {   // In aliens report phase read owning species
            if( m_RM->Match(s, "^SP ([^,;]+)\\s*$") )
            {
                String ^spName = m_RM->Results[0];
                Alien ^sp = m_GameData->AddAlien(spName);
                m_ScanShip = m_GameData->AddShip( sp, type, name, subLight, system );

                if( bInFD )
                {
                    sp->Relation  = SP_PIRATE;
                    if( m_ScanShip )
                        m_ScanShip->IsPirate = true;
                }
            }
        }
    }

    m_ScanShip->Age         = age;
    m_ScanShip->Size        = size;
    m_ScanShip->Location    = location;
    m_ScanShip->PlanetNum   = planetNum;
    m_ScanShip->CalculateCapacity();
    if( capacity != 0 &&
        capacity != m_ScanShip->Capacity )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Calculated ship '{0}' capacity ({1}) doesn't match report ({2}).",
                name, m_ScanShip->Capacity, capacity) );
    }
}

void Report::MatchOtherPlanetsShipsScan(String ^s)
{
    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+#(\\d+)\\s+PL\\s+([^,]+)(,?)") )
    {
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);

        StarSystem^ system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ, false);

        int plNum = m_RM->GetResultInt(3);
        String^ plName = m_RM->Results[4];

        // TODO: Not sure if system is under observation when no CUs are present on the planet
        bool isObserver = ( m_RM->Results[5]->Length > 0 );

        // Treat as colony of size 0
        Colony^ colony = m_GameData->AddColony(
            GameData::Player,
            plName,
            system,
            plNum,
            isObserver );

        colony->PlanetType = PLANET_COLONY;
        colony->LSN = system->Planets[plNum]->LSN;
        colony->MiDiff = system->Planets[plNum]->MiDiff;
        colony->MiBase = 0;
        colony->MaBase = 0;
        colony->EconomicEff = 0;

        while( !String::IsNullOrEmpty(s) )
        {
            if( m_RM->Match(s, "^,?\\s+(\\d+)\\s+(\\w+)\\s*") )
            {
                int amount = m_RM->GetResultInt(0);
                InventoryType inv = FHStrings::InvFromString( m_RM->Results[1] );
                colony->Inventory[inv] = amount;
            }
            else
                throw gcnew FHUIParsingException(
                    String::Format("Unable to parse colony '{0}' inventory.", plName));
        }
    }

    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+") )
    {
        m_ScanX = m_RM->GetResultInt(0);
        m_ScanY = m_RM->GetResultInt(1);
        m_ScanZ = m_RM->GetResultInt(2);
    }
    MatchShipScan(s, false);
}

void Report::MatchAliensReport(String ^s)
{
    PlanetType plType = PLANET_MAX;
    if( m_RM->Match(s, "^Home planet PL\\s+") )
        plType = PLANET_HOME;
    else if( m_RM->Match(s, "^Colony planet PL\\s+") )
        plType = PLANET_COLONY;
    else if( m_RM->Match(s, "^Mining colony PL\\s+") )
        plType = PLANET_COLONY_MINING;
    else if( m_RM->Match(s, "^Resort colony PL\\s+") )
        plType = PLANET_COLONY_RESORT;
    else if( m_RM->Match(s, "^Uncolonized planet PL\\s+") )
        plType = PLANET_UNCOLONIZED;
    if( plType != PLANET_MAX )
    {
        if( m_RM->Match(s, "^([^,;]+)\\s+\\(pl #(\\d+)\\)\\s+SP\\s+([^,;]+)\\s*$") )
        {
            StarSystem ^system = m_GameData->GetStarSystem(m_ScanX, m_ScanY, m_ScanZ, false);
            String ^name = m_RM->Results[0];
            int plNum    = m_RM->GetResultInt(1);
            Alien ^sp    = m_GameData->AddAlien(m_RM->Results[2]);

            m_ScanColony = m_GameData->AddColony(sp, name, system, plNum, false);
            if( m_ScanColony )
            {
                m_ScanColony->LastSeen = Math::Max(m_Turn, m_ScanColony->LastSeen);
                m_ScanColony->PlanetType = plType;

                if( plType == PLANET_HOME )
                {
                    sp->HomeSystem = system;
                    sp->HomePlanet = plNum;

                    system->HomeSpecies = sp;
                }
            }
        }
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(Economic base is approximately (\\d+)\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
        {
            m_ScanColony->MiBase = 10 * m_RM->GetResultInt(0);
            m_ScanColony->MaBase = 0;
        }
    }
    else if( m_ScanColony && s == "(No economic base.)" )
    {
        if( m_ScanColony->LastSeen == m_Turn )
        {
            m_ScanColony->MiBase = 0;
            m_ScanColony->MaBase = 0;
        }
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(There are (\\d+) Planetary Defense Units on the planet\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
            m_ScanColony->Inventory[INV_PD] = m_RM->GetResultInt(0);
    }
    else if( m_ScanColony && m_RM->Match(s, "^\\(There are (\\d+) shipyards on the planet\\.\\)") )
    {
        if( m_ScanColony->LastSeen == m_Turn )
            m_ScanColony->Shipyards = m_RM->GetResultInt(0);
    }
    else
        MatchShipScan(s, false);
}

void Report::MatchOrdersTemplate(String ^s)
{
    if( String::IsNullOrEmpty(s) || s[0] == ';' )
        return;
    if( s == "END" )
    {
        m_TemplatePhase = CommandPhase::Custom;
        m_ColonyProduction = nullptr;
        return;
    }

    if( m_RM->Match(s, "^START ([A-Z-]+)$") )
    {
        String ^phase = m_RM->Results[0]->ToLower();
        for( int i = 0; i < (int)CommandPhase::Custom; ++i )
        {
            CommandPhase newPhase = static_cast<CommandPhase>(i);
            if( phase == CmdCustom::PhaseAsString(newPhase)->ToLower() )
            {
                m_TemplatePhase = newPhase;
                break;
            }
        }

        if( m_TemplatePhase == CommandPhase::Production )
            m_ColonyProduction = nullptr;

        return;
    }

    if( m_TemplatePhase == CommandPhase::Custom )
        return;

    if( m_TemplatePhase == CommandPhase::Production )
    {
        // START PRODUCTION
        if( m_RM->Match(s, "^PRODUCTION\\s+PL\\s+([^,]+)$") )
        {
            Colony^ colony = m_GameData->GetColony(m_RM->Results[0]);
            if ( colony )
            {
                m_ColonyProduction = colony;
                return;
            }
            throw gcnew FHUIParsingException(
                "PRODUCTION order for unknown colony: PL " + m_RM->Results[0] );
        }

        // Ignore any production command
        // until production colony is initialized
        if( m_ColonyProduction == nullptr )
            return;
    }

    // INSTALL
    if( m_RM->Match(s, m_RM->ExpCmdInstall) )
    {
        Colony ^colony = GameData::GetColony( m_RM->Results[2] );

        ICommand ^cmd = gcnew CmdInstall(
            m_RM->GetResultInt(0),
            FHStrings::InvFromString(m_RM->Results[1]),
            colony );
        cmd->Origin = CommandOrigin::Auto;
        colony->Commands->Add( cmd );
        return;
    }

    // UNLOAD
    if( m_RM->Match(s, m_RM->ExpCmdShipUnload) )
    {
        Ship^ ship = m_GameData->GetShip(m_RM->Results[0]);  
        if ( ship )
        {
            ICommand ^cmd = gcnew ShipCmdUnload(ship);
            cmd->Origin = CommandOrigin::Auto;
            ship->AddCommand( cmd );
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("UNLOAD order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // JUMP
    if( m_RM->Match(s, "^Jump\\s+(\\w+)\\s+([^,]+),\\s+(.+)$") )
    {
        Ship^ ship = m_GameData->GetShip(m_RM->Results[1]);
        if ( ship )
        {
            String ^target = m_RM->Results[2];
            ICommand ^cmd = nullptr;
            if( target->Length > 2 &&
                target->Substring(0, 3) == "???" )
            {
                cmd = gcnew ShipCmdJump(ship, nullptr, -1);
            }
            else if( m_RM->Match(target, "(\\d+)\\s+(\\d+)\\s+(\\d+)") )
            {
                StarSystem ^jumpTarget = GameData::GetStarSystem(
                    m_RM->GetResultInt(0),
                    m_RM->GetResultInt(1),
                    m_RM->GetResultInt(2),
                    true );
                cmd = gcnew ShipCmdJump(ship, jumpTarget, -1);
            }
            else if( m_RM->Match(target, "PL\\s+([^,;]+)") )
            {
                Colony ^colony = GameData::GetColony(m_RM->Results[0]->TrimEnd());
                if( colony )
                    cmd = gcnew ShipCmdJump(ship, colony->System, colony->PlanetNum);
            }
            if( cmd )
            {
                cmd->Origin = CommandOrigin::Auto;
                ship->AddCommand( cmd );
            }
            return;
        }
        throw gcnew FHUIParsingException(
            String::Format("JUMP order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // DEVELOP
    if( m_RM->Match(s, m_RM->ExpCmdDevelopCS) )
    {
        Colony ^colony = m_GameData->GetColony(m_RM->Results[1]);
        Ship^ ship = m_GameData->GetShip(m_RM->Results[2]);
        if( !ship )
            throw gcnew FHUIParsingException("JUMP order for unknown ship: {1}" + m_RM->Results[2]);

        ICommand ^cmd = gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            colony,
            ship );
        m_ColonyProduction->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return;
    }
    if( m_RM->Match(s, m_RM->ExpCmdDevelopC) )
    {
        Colony ^colony = m_GameData->GetColony(m_RM->Results[1]);

        ICommand ^cmd = gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            colony,
            nullptr );
        m_ColonyProduction->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return;
    }
    if( m_RM->Match(s, m_RM->ExpCmdDevelop) )
    {
        ICommand ^cmd = gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            nullptr,
            nullptr );
        m_ColonyProduction->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return;
    }

    // Build CU/IU/AU
    if( m_RM->Match(s, m_RM->ExpCmdBuildIUAU) )
    {
        int amount = m_RM->GetResultInt(0);
        ICommand ^cmd = gcnew ProdCmdBuildIUAU(
                amount,
                FHStrings::InvFromString(m_RM->Results[1]),
                nullptr,
                nullptr );
        m_ColonyProduction->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return;
    }

    // RECYCLE
    if( m_RM->Match(s, m_RM->ExpCmdRecycle) )
    {
        ICommand ^cmd = gcnew ProdCmdRecycle(
            FHStrings::InvFromString( m_RM->Results[1] ),
            m_RM->GetResultInt(0) );
        m_ColonyProduction->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return;
    }
    // RECYCLE SHIP
    if( m_RM->Match(s, m_RM->ExpCmdShipRec) )
    {
        Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
        ICommand ^cmd = gcnew ShipCmdRecycle( ship );
        cmd->Origin = CommandOrigin::Auto;
        ship->AddCommand( cmd );
        return;
    }

    // AUTO
    if( m_RM->Match(s, "^Auto$") )
    {
        m_CommandMgr->AutoEnabled = true;
        return;
    } 

    // SCAN
    if( m_RM->Match(s, m_RM->ExpCmdShipScan) )
    {
        Ship ^ship = m_GameData->GetShip(m_RM->Results[0]);
        ICommand ^cmd = gcnew ShipCmdScan( ship );
        cmd->Origin = CommandOrigin::Auto;
        ship->AddCommand( cmd );
        return;
    }

    // ANY OTHER COMMAND -> CUSTOM COMMAND
    // unless phase is Jumps - no custom orders here
    if( m_TemplatePhase == CommandPhase::Jump )
        return;

    CmdCustom ^cmd = gcnew CmdCustom(m_TemplatePhase, s, 0);
    cmd->Origin = CommandOrigin::Auto;
    if( m_TemplatePhase == CommandPhase::Production )
        m_CommandMgr->AddCommand(m_ColonyProduction, cmd);
    else
        m_CommandMgr->AddCommand(cmd);

}

void Report::StartLineAggregate(PhaseType phase, String ^s, int maxLines)
{
    m_bParsingAggregate = true;
    m_StringAggregate = s;
    m_AggregateMaxLines = maxLines;
    m_PhasePreAggregate = m_Phase;
    m_Phase = phase;
}

String^ Report::FinishLineAggregate(bool resetPhase)
{
    m_bParsingAggregate = false;
    String ^ret = m_StringAggregate;
    m_StringAggregate = nullptr;
    if( resetPhase )
        m_Phase = m_PhasePreAggregate;
    return ret;
}

void Report::MatchAlienInfo(String ^s, Alien ^alien)
{
    if( m_RM->Match(s, "^Home\\s*:\\s*\\[\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*\\]\\s*") )
    {
        alien->HomeSystem = m_GameData->GetStarSystem(
            m_RM->GetResultInt(0),
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2),
            false );
        alien->HomePlanet = m_RM->GetResultInt(3);
    }

    if( m_RM->Match(s, "^Temp:(\\d+)\\s+Press:(\\d+)\\s*") )
    {
        alien->AtmReq->TempClass  = m_RM->GetResultInt(0);
        alien->AtmReq->PressClass = m_RM->GetResultInt(1);
    }

    if( m_RM->Match(s, "^Atm\\s*:\\s*([A-Za-z0-9]+)\\s+(\\d+)-(\\d+)%\\s*") )
    {
        alien->AtmReq->GasRequired = FHStrings::GasFromString( m_RM->Results[0] );
        alien->AtmReq->ReqMin = m_RM->GetResultInt(1);
        alien->AtmReq->ReqMax = m_RM->GetResultInt(2);
    }

    if( m_RM->MatchList(s, "^Poisons:\\s*", "([A-Za-z0-9]+)") )
    {
        for( int i = 0; i < m_RM->Results->Length; ++i )
        {
            alien->AtmReq->Poisonous[ FHStrings::GasFromString(m_RM->Results[i]) ] = true;
        }
    }
}

} // end namespace FHUI
