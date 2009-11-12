#include "StdAfx.h"
#include "Report.h"

namespace FHUI
{

bool Report::MatchSectionEnd(String ^s)
{
    return ( s == "* * * * * * * * * * * * * * * * * * * * * * * * *" );
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

bool Report::MatchSystemScan(String ^s)
{
    if( m_RM->Match(s, "^Coordinates:\\s+[Xx]\\s+=\\s+(\\d+)\\s+[Yy]\\s+=\\s+(\\d+)\\s+[Zz]\\s+=\\s+(\\d+)") )
    {
        //TODO: parse "stellar type = xx   N planets"
        StarSystem ^system = m_GameData->GetStarSystem(m_RM->GetResultInt(0), m_RM->GetResultInt(1), m_RM->GetResultInt(2), false);

        // optional empty line
        s = GET_NON_EMPTY_LINE();

        // optional wormhole information
        if ( ( s == "This star system is the terminus of a natural wormhole." ) ||
             ( s == "Wormhole to Unknown System" ) )
        {
            system->HasWormhole = true;
            s = GET_NON_EMPTY_LINE();
        }
        else if( m_RM->Match(s, "^Wormhole to\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$") )
        {
            system->SetWormhole( GameData::GetSystemId(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2) ) );
            s = GET_NON_EMPTY_LINE();
        }

        // static table description
        if ( s == "Temp  Press Mining" )
        {
            s = GET_LINE();
        }

        if ( ( s == "#  Dia  Grav Class Class  Diff  LSN  Atmosphere" ) ||
             ( s == "#  Dia  Grav TC PC MD  LSN  Atmosphere" ) )
        {
            s = GET_LINE();
        }
        else
            throw gcnew FHUIParsingException("System Scan: unexpected line");

        if ( s != "---------------------------------------------------------------------" )
            throw gcnew FHUIParsingException("System Scan: unexpected line");

        while ( ! String::IsNullOrEmpty( s = GET_LINE() ) )
        {
            if( ! MatchPlanetScan( s, system ) )
                throw gcnew FHUIParsingException("System Scan: unexpected line");
        }
        return true;
    }
    return false;
}

bool Report::MatchPlanetScan(String ^s, StarSystem ^system)
{
    //                       0:plNum   1:dia    2:gv            3:tc       4:pc    5:mining diff
    if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\.(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\.(\\d+)\\s+") )
    {
        int plNum = m_RM->GetResultInt(0);
        Planet ^planet = gcnew Planet(
            system,
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

        m_GameData->AddPlanetScan( system, planet );
        
        return true;
    }
    return false;
}

bool Report::MatchAlienEstimate(String ^s)
{
    if( m_RM->Match(s, "^Estimate of the technology of " ) )
    {
        do
        {
            s += " " + GET_LINE();
        }
        while( ! ( s->Contains( "(" ) && s->Contains( ")" ) ) );

        Alien^ alien;

        if( m_RM->Match(s, "SP\\s+([^,;]+)\\s+\\(government name '([^']+)', government type '([^']+)'\\)") )
        {
            alien = m_GameData->AddAlien(m_RM->Results[0]);
            alien->GovName = m_RM->Results[1];
            alien->GovType = m_RM->Results[2];
        }
        else
        {
            throw gcnew FHUIParsingException("Estimate: failed to parse alien info");
        }

        if( m_RM->Match( GET_LINE(), "MI =\\s+(\\d+), MA =\\s+(\\d+), ML =\\s+(\\d+), GV =\\s+(\\d+), LS =\\s+(\\d+), BI =\\s+(\\d+)\\.") )
        {
            if( m_Turn > alien->TechEstimateTurn )
            {
                alien->TechLevels[TECH_MI] = m_RM->GetResultInt(0);
                alien->TechLevels[TECH_MA] = m_RM->GetResultInt(1);
                alien->TechLevels[TECH_ML] = m_RM->GetResultInt(2);
                alien->TechLevels[TECH_GV] = m_RM->GetResultInt(3);
                alien->TechLevels[TECH_LS] = m_RM->GetResultInt(4);
                alien->TechLevels[TECH_BI] = m_RM->GetResultInt(5);
                alien->TechEstimateTurn = m_Turn;
            }
        }
        else
        {
            throw gcnew FHUIParsingException("Estimate: failed to parse alien tech levels");
        }
        return true;
    }
    return false;
}

bool Report::MatchMessageReceived(String ^s)
{
    if( m_RM->Match(s, "^You received the following message from SP ([^,;]+):") )
    {
        Alien ^alien = m_GameData->AddAlien(m_RM->Results[0]);
        String ^message = "";

        while( ( s = GET_LINE() ) != "*** End of Message ***" )
        {
            message += s + "\r\n";
            if( m_RM->Match(s, "([a-zA-Z0-9_.-]+@[a-zA-Z0-9_.]+\\.[a-zA-Z0-9_]+)") )
                alien->Email = m_RM->Results[0];
        }
        alien->LastMessageRecv = message;
        alien->LastMessageRecvTurn = m_GameData->CurrentTurn - 1;
        return true;
    }
    return false;
}

bool Report::MatchTechLevels(String ^s)
{
    if( s == "Tech Levels:" )
    {
        if( MatchTech( GET_LINE(), "^Mining", TECH_MI ) &&
            MatchTech( GET_LINE(), "^Manufacturing", TECH_MA ) &&
            MatchTech( GET_LINE(), "^Military", TECH_ML ) &&
            MatchTech( GET_LINE(), "^Gravitics", TECH_GV ) &&
            MatchTech( GET_LINE(), "^Life Support", TECH_LS ) && 
            MatchTech( GET_LINE(), "^Biology", TECH_BI ) )
        {
            return true;
        }
        throw gcnew FHUIParsingException("Tech levels: missing entries");
    }
    return false;
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

bool Report::MatchAtmReq(String ^s)
{
    if ( s->StartsWith( "Atmospheric Requirement" ) )
    {
        if( m_RM->Match(s, "^Atmospheric Requirement: (\\d+)%-(\\d+)% ([A-Za-z0-9]+)") )
        {
            m_GameData->SetAtmosphereReq(
                FHStrings::GasFromString( m_RM->Results[2] ), // required gas
                m_RM->GetResultInt(0),               // min level
                m_RM->GetResultInt(1) );             // max level
        }
        else
            throw gcnew FHUIParsingException("Failed to parse Atmospheric Requirement");

        if( m_RM->MatchList( GET_LINE(), "^Neutral Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAtmosphereNeutral(
                    FHStrings::GasFromString(m_RM->Results[i]) );
        }
        else
            throw gcnew FHUIParsingException("Failed to parse Atmospheric Requirement");

        if( m_RM->MatchList(GET_LINE(), "^Poisonous Gases:", "([A-Za-z0-9]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAtmospherePoisonous(
                    FHStrings::GasFromString(m_RM->Results[i]) );
        }   
        else
            throw gcnew FHUIParsingException("Failed to parse Atmospheric Requirement");

        return true;
    }
    return false;
}

bool Report::MatchSpeciesMet(String ^s)
{
    if( s->StartsWith( "Species met:" ) )
    {
        if( m_RM->MatchList( s + GET_MERGED_LINES(), "^Species met:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
            {
                m_GameData->AddAlien(m_RM->Results[i])->Relation = 
                    m_GameData->Player->RelationDefault;
            }
        }
        else
        {
            throw gcnew FHUIParsingException("Known Species: failure matching species met");
        }
        return true;
    }
    return false;
}

bool Report::MatchAllies(String ^s)
{
    if ( s->StartsWith( "Allies:" ) )
    {
        if( m_RM->MatchList( s + GET_MERGED_LINES(), "^Allies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAlienRelation(m_RM->Results[i], SP_ALLY);
        }
        else
        {
            throw gcnew FHUIParsingException("Known Species: failure matching allies");
        }
        return true;
    }
    return false;
}

bool Report::MatchEnemies(String ^s)
{
    if ( s->StartsWith( "Enemies:" ) )
    {
        if( m_RM->MatchList( s + GET_MERGED_LINES(), "^Enemies:", "SP\\s+([^,;]+)") )
        {
            for( int i = 0; i < m_RM->Results->Length; ++i )
                m_GameData->SetAlienRelation(m_RM->Results[i], SP_ENEMY);
        }  
        else
        {
            throw gcnew FHUIParsingException("Known Species: failure matching enemies");
        }
        return true;
    }
    return false;
}

bool Report::MatchColonyInfo(String ^s)
{
    PlanetType planetType = PLANET_COLONY;
    if( m_RM->Match(s, "^(HOME|COLONY)\\s+PLANET: PL\\s+") )
    {
        if( m_RM->Results[0][0] == 'H' )
            planetType = PLANET_HOME;
    }
    else if( m_RM->Match(s, "^(MINING|RESORT)\\s+COLONY: PL\\s+") )
    {
        if( m_RM->Results[0][0] == 'M' )
            planetType = PLANET_COLONY_MINING;
        else
            planetType = PLANET_COLONY_RESORT;
    }
    else
    {
        return false;
    }

    StarSystem ^system;
    Colony ^colony;

    s += " " + GET_NON_EMPTY_LINE();

    if( m_RM->Match(s, "([^,;]+)\\s+Coordinates: x = (\\d+), y = (\\d+), z = (\\d+), planet number (\\d+)") )
    {
        String ^plName = m_RM->Results[0]->TrimEnd();
        int plNum = m_RM->GetResultInt(4);

        system = m_GameData->GetStarSystem(
            m_RM->GetResultInt(1),
            m_RM->GetResultInt(2),
            m_RM->GetResultInt(3),
            false );

        if( planetType == PLANET_HOME )
        {
            GameData::Player->HomeSystem = system;
            GameData::Player->HomePlanet = plNum;
        }

        colony = m_GameData->CreateColony(
            GameData::Player,
            plName,
            system,
            plNum,
            true );

        colony->MiBase = 0;
        colony->MaBase = 0;
        colony->Shipyards = 0;
        colony->PlanetType = planetType;
        s = GET_NON_EMPTY_LINE();
    }
    else
    {
        throw gcnew FHUIParsingException(
            String::Format("Unable to parse colony coordinates: {0}", s) );
    }

    if( s == "WARNING! Home planet has not yet completely recovered from bombardment!" )
    {
        s = GET_LINE();
        if ( m_RM->Match(s, "(\\d+) IUs and (\\d+) AUs will have to be installed for complete recovery.") )
        {
            colony->RecoveryIU = m_RM->GetResultInt(0);
            colony->RecoveryAU = m_RM->GetResultInt(1);
            s = GET_NON_EMPTY_LINE();
        }
        else
        {
            throw gcnew FHUIParsingException("Unexpected line");
        }
    }

    if( m_RM->Match(s, "^Available population units = (\\d+)") )
    {
        colony->AvailPop = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^IMPORTANT!  This planet is actively hiding from alien observation!") )
    {
        colony->Hidden = true;
        s = GET_NON_EMPTY_LINE();
    }

    if( ( s == "WARNING!  This planet is currently under siege and will remain" ) &&
        ( GET_LINE() == "under siege until the combat phase of the next turn!" ) )
    {
        colony->UnderSiege = true;
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Production penalty = (\\d+)% \\(LSN = (\\d+)\\)") )
    {
        colony->ProdPenalty = m_RM->GetResultInt(0);
        colony->LSN = m_RM->GetResultInt(1);
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Economic efficiency = (\\d+)%") )
    {
        colony->EconomicEff = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Mining base = (\\d+)\\.(\\d+) \\(MI = \\d+, MD = (\\d+)\\.(\\d+)\\)") )
    {
        colony->MiBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
        colony->MiDiff = m_RM->GetResultInt(2) * 100 + m_RM->GetResultInt(3);
        s = GET_NON_EMPTY_LINE();
    }

    if( ( colony->PlanetType == PLANET_COLONY_MINING ) &&
        m_RM->Match(s, "^This mining colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
    {
        colony->EUProd  = m_RM->GetResultInt(0);
        colony->EUFleet = m_RM->GetResultInt(1);
        m_GameData->AddTurnProducedEU( colony->EUProd );
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^(\\d+) raw material units will be produced this turn\\.$") )
    {
        colony->RMProduced = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Manufacturing base = (\\d+)\\.(\\d+) \\(") )
    {
        colony->MaBase = m_RM->GetResultInt(0) * 10 + m_RM->GetResultInt(1);
        s = GET_NON_EMPTY_LINE();
    }

    if( ( colony->PlanetType == PLANET_COLONY_RESORT ) &&
        m_RM->Match(s, "^This resort colony will generate (\\d+) - (\\d+) = \\d+ economic units this turn\\.") )
    {
        colony->EUProd  = m_RM->GetResultInt(0);
        colony->EUFleet = m_RM->GetResultInt(1);
        m_GameData->AddTurnProducedEU( colony->EUProd );
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Production capacity this turn will be (\\d+)\\.$") )
    {
        colony->RMUsed = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Raw Material Units \\(RM,C1\\) carried over from last turn = (\\d+)") )
    {
        colony->Inventory[INV_RM] = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( ( ( colony->PlanetType == PLANET_COLONY ) || ( colony->PlanetType == PLANET_HOME ) ) &&
        m_RM->Match(s, "^Total available for spending this turn = (\\d+) - (\\d+) = \\d+") )
    {
        colony->Inventory[INV_RM] += (colony->RMProduced - colony->RMUsed);
        if( colony->Inventory[INV_RM] < 0 )
            colony->Inventory[INV_RM] = 0;

        colony->EUProd  = m_RM->GetResultInt(0);
        colony->EUFleet = m_RM->GetResultInt(1);
        m_GameData->AddTurnProducedEU( colony->EUProd );
        s = GET_NON_EMPTY_LINE();
    }

    if( m_RM->Match(s, "^Shipyard capacity = (\\d+)") )
    {
        colony->Shipyards = m_RM->GetResultInt(0);
        s = GET_NON_EMPTY_LINE();
    }

    if( MatchColonyInventory(s, colony) )
    {
        s = GET_NON_EMPTY_LINE();
    }

    if( MatchColonyShips(s, colony) )
    {
        s = GET_NON_EMPTY_LINE();
    }
    if( MatchSectionEnd(s) )
    {
        return true;
    }
    else
    {
        throw gcnew FHUIParsingException("End of section expected ");
    }

    return false;
}

bool Report::MatchColonyInventory(String ^s, Colony ^colony)
{
    if( s != "Planetary inventory:" )
    {
        return false;
    }
     
    while( ! String::IsNullOrEmpty( s = GET_LINE() ) )
    {
        if( m_RM->Match(s, "^[\\w\\s.-]+\\(PD,C3\\) = (\\d+) (warship equivalence = \\d+ tons)") )
            colony->Inventory[INV_PD] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SU,C20\\) = (\\d+)") )
            colony->Inventory[INV_SU] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(CU,C1\\) = (\\d+)") )
            colony->Inventory[INV_CU] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(IU,C1\\) = (\\d+)") )
            colony->Inventory[INV_IU] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(AU,C1\\) = (\\d+)") )
            colony->Inventory[INV_AU] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FS,C1\\) = (\\d+)") )
            colony->Inventory[INV_FS] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FD,C1\\) = (\\d+)") )
            colony->Inventory[INV_FD] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(DR,C1\\) = (\\d+)") )
            colony->Inventory[INV_DR] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FM,C5\\) = (\\d+)") )
            colony->Inventory[INV_FM] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(FJ,C5\\) = (\\d+)") )
            colony->Inventory[INV_FJ] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GW,C100\\) = (\\d+)") )
            colony->Inventory[INV_GW] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GT,C20\\) = (\\d+)") )
            colony->Inventory[INV_GT] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(JP,C10\\) = (\\d+)") )
            colony->Inventory[INV_JP] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(TP,C100\\) = (\\d+)") )
            colony->Inventory[INV_TP] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU1,C5\\) = (\\d+)") )
            colony->Inventory[INV_GU1] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU2,C10\\) = (\\d+)") )
            colony->Inventory[INV_GU2] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU3,C15\\) = (\\d+)") )
            colony->Inventory[INV_GU3] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU4,C20\\) = (\\d+)") )
            colony->Inventory[INV_GU4] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU5,C25\\) = (\\d+)") )
            colony->Inventory[INV_GU5] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU6,C30\\) = (\\d+)") )
            colony->Inventory[INV_GU6] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU7,C35\\) = (\\d+)") )
            colony->Inventory[INV_GU7] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU8,C40\\) = (\\d+)") )
            colony->Inventory[INV_GU8] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(GU9,C45\\) = (\\d+)") )
            colony->Inventory[INV_GU9] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG1,C5\\) = (\\d+)") )
            colony->Inventory[INV_SG1] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG2,C10\\) = (\\d+)") )
            colony->Inventory[INV_SG2] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG3,C15\\) = (\\d+)") )
            colony->Inventory[INV_SG3] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG4,C20\\) = (\\d+)") )
            colony->Inventory[INV_SG4] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG5,C25\\) = (\\d+)") )
            colony->Inventory[INV_SG5] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG6,C30\\) = (\\d+)") )
            colony->Inventory[INV_SG6] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG7,C35\\) = (\\d+)") )
            colony->Inventory[INV_SG7] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG8,C40\\) = (\\d+)") )
            colony->Inventory[INV_SG8] = m_RM->GetResultInt(0);
        else if( m_RM->Match(s, "^[\\w\\s.-]+\\(SG9,C45\\) = (\\d+)") )
            colony->Inventory[INV_SG9] = m_RM->GetResultInt(0);
    }
    return true;
}

bool Report::MatchColonyShips(String ^s, Colony ^colony)
{
    if( ! s->StartsWith( "Ships at PL" ) )
    {
        return false;
    }

    if( ( s = GET_LINE() )->StartsWith( "Name" ) &&
        ( s = GET_LINE() )->StartsWith( "----------" ) )
    {
        while( ! String::IsNullOrEmpty( s = GET_LINE() ) )
        {
            if ( false == MatchShipInfo(s, colony->System, colony) )
                throw gcnew FHUIParsingException("Failed to parse ship");
        }
        return true;
    }
    return false;
}

bool Report::MatchShipInfo(String ^s, StarSystem ^system, Colony ^colony)
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
                return false;
        }
        catch( FHUIParsingException^ )
        {
            return false;
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

    Ship ^ship;

    if( bMatch )
    {
        if( m_Phase != PHASE_LOG_USER )
            system->LastVisited = m_Turn;
        else
            system->LastVisited = 0;

        if( bIncomplete )
        {
            location = SHIP_LOC_LANDED;
            if( colony )
                planetNum = colony->PlanetNum;
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

        if( colony )
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

        if( m_RM->Match(s, "^SP ([^,;]+)\\s*$") )
        {
            String ^spName = m_RM->Results[0];
            Alien ^sp = m_GameData->AddAlien(spName);
            ship = m_GameData->CreateShip( sp, type, name, subLight, system );

            if( bInFD )
            {
                sp->Relation  = SP_PIRATE;
                if( ship )
                    ship->IsPirate = true;
            }
        }
        else
        {
            ship = m_GameData->CreateShip( GameData::Player, type, name, subLight, system);
            if( ship == nullptr )
                return false;

            if( bIncomplete )
            {
                if( m_RM->Match(s, "^Left to pay = (\\d+)$") )
                {
                    int amount = m_RM->GetResultInt(0);
                    ship->EUToComplete = amount;
                    ship->CanJump = false;
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
                    ship->Cargo[inv] = amount;
                }
                else
                    throw gcnew FHUIParsingException(
                        String::Format("Unable to parse ship '{0}' inventory.", name));
            }
        }
    }

    ship->Age         = age;
    ship->Size        = size;
    ship->Location    = location;
    ship->PlanetNum   = planetNum;
    ship->CalculateCapacity();
    if( capacity != 0 &&
        capacity != ship->Capacity )
    {
        throw gcnew FHUIDataIntegrityException(
            String::Format("Calculated ship '{0}' capacity ({1}) doesn't match report ({2}).",
                name, ship->Capacity, capacity) );
    }
    return true;
}

bool Report::MatchOtherPlanetsShips(String ^s)
{
    if( s != "Other planets and ships:" )
    {
        return false;
    }

    StarSystem^ system;

    while( s = GET_NON_EMPTY_LINE() )
    {
        if( MatchSectionEnd(s) )
        {
            return true;
        }

        if( m_RM->Match(s, "^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+#(\\d+)\\s+PL\\s+([^,]+)(,?)") )
        {
            system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                false);

            int plNum = m_RM->GetResultInt(3);
            String^ plName = m_RM->Results[4];

            bool isObserver = ( m_RM->Results[5]->Contains("CU") );

            // Treat as colony of size 0
            Colony^ colony = m_GameData->CreateColony(
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
            // When reading other ships and planets,
            // ship may be outside of any system just in deep, empty space
            // add this location temporarily to star system list
            system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2),
                true);
        }
        MatchShipInfo(s, system, nullptr);
    }

    return false;
}

bool Report::MatchAliens(String ^s)
{
    if( ! s->StartsWith("Aliens at") )
    {
        return false;
    }

    StarSystem ^system;
    Colony^ colony;
    Alien ^alien;

    while ( ! String::IsNullOrEmpty(s) )
    {
        if( m_RM->Match(s, "^Aliens at\\s+x\\s+=\\s+(\\d+), y\\s+=\\s+(\\d+), z\\s+=\\s+(\\d+)") )
        {
            system = m_GameData->GetStarSystem(
                m_RM->GetResultInt(0),
                m_RM->GetResultInt(1),
                m_RM->GetResultInt(2), 
                false);
            s = GET_LINE();
            continue;
        }

        if( system == nullptr )
        {
            throw gcnew FHUIParsingException("Unexpected content");
        }

        PlanetType plType = PLANET_MAX;
        if( m_RM->Match(s, "^Colony planet PL\\s+") )
            plType = PLANET_COLONY;
        else if( m_RM->Match(s, "^Home planet PL\\s+") )
            plType = PLANET_HOME;
        else if( m_RM->Match(s, "^Colony planet PL\\s+") )
            plType = PLANET_COLONY;
        else if( m_RM->Match(s, "^Mining colony PL\\s+") )
            plType = PLANET_COLONY_MINING;
        else if( m_RM->Match(s, "^Resort colony PL\\s+") )
            plType = PLANET_COLONY_RESORT;
        else if( m_RM->Match(s, "^Uncolonized planet PL\\s+") )
            plType = PLANET_UNCOLONIZED;
        else if( MatchShipInfo(s, system, nullptr) )
        {
            s = GET_LINE();
            continue;
        }

        if( plType == PLANET_MAX )
        {
            throw gcnew FHUIParsingException("Unexpected content");
        }

        if( m_RM->Match(s, "^([^,;]+)\\s+\\(pl #(\\d+)\\)\\s+SP\\s+([^,;]+)\\s*$") )
        {
            String ^name = m_RM->Results[0];
            int plNum    = m_RM->GetResultInt(1);
            alien        = m_GameData->AddAlien(m_RM->Results[2]);

            if( colony = m_GameData->CreateColony(alien, name, system, plNum, false) )
            {
                colony->LastSeen = Math::Max(m_Turn, colony->LastSeen);
                colony->PlanetType = plType;

                if( plType == PLANET_HOME )
                {
                    alien->HomeSystem = system;
                    alien->HomePlanet = plNum;

                    system->HomeSpecies = alien;
                }
            }
            s = GET_LINE();
        }
        else
        {
            throw gcnew FHUIParsingException("Unexpected content");
        }

        if( ! colony )
        {
            // TODO: is it ok to quit here?
            return true;
        }

        if( m_RM->Match(s, "^\\(Economic base is approximately (\\d+)\\.\\)") )
        {
            if( colony->LastSeen == m_Turn )
            {
                colony->MiBase = 10 * m_RM->GetResultInt(0);
                colony->MaBase = 0;
            }
            s = GET_LINE();
        }
        else if( s == "(No economic base.)" )
        {
            if( colony->LastSeen == m_Turn )
            {
                colony->MiBase = 0;
                colony->MaBase = 0;
            }
            s = GET_LINE();
        }
        else
        {
            throw gcnew FHUIParsingException("Unexpected content");
        }

        if( m_RM->Match(s, "^\\(There are (\\d+) Planetary Defense Units on the planet\\.\\)") )
        {
            if( colony->LastSeen == m_Turn )
                colony->Inventory[INV_PD] = m_RM->GetResultInt(0);
            s = GET_LINE();
        }

        if( m_RM->Match(s, "^\\(There (are|is) (\\d+) shipyards? on the planet\\.\\)") )
        {
            if( colony->LastSeen == m_Turn )
                colony->Shipyards = m_RM->GetResultInt(1);
            s = GET_LINE();
        }
        if( s == "(Colony is actively hiding from alien observation.)" )
        {
            colony->Hidden = true;
            s = GET_LINE();
        }
    }
    return true;
}

bool Report::MatchTemplateEntry(String ^s)
{
    if( s->StartsWith( ";" ) )
        return true;

    if( s == "END" )
    {
        m_TemplatePhase = CommandPhase::Custom;
        m_TemplateColony = nullptr;
        return true;
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
            m_TemplateColony = nullptr;

        return true;
    }

    if( m_TemplatePhase == CommandPhase::Custom )
        return true;

    if( m_TemplatePhase == CommandPhase::Production )
    {
        // START PRODUCTION
        if( m_RM->Match(s, "^PRODUCTION\\s+PL\\s+([^,]+)$") )
        {
            Colony^ colony = GameData::Player->FindColony(m_RM->Results[0], false);
            if ( colony )
            {
                m_TemplateColony = colony;
                return true;
            }
            throw gcnew FHUIParsingException(
                "PRODUCTION order for unknown colony: PL " + m_RM->Results[0] );
        }

        // Ignore any production command
        // until production colony is initialized
        if( m_TemplateColony == nullptr )
            return true;
    }

    // INSTALL
    if( m_RM->Match(s, m_RM->ExpCmdInstall) )
    {
        Colony ^colony = GameData::Player->FindColony( m_RM->Results[2], false );

        ICommand ^cmd = gcnew CmdInstall(
            m_RM->GetResultInt(0),
            FHStrings::InvFromString(m_RM->Results[1]),
            colony );
        cmd->Origin = CommandOrigin::Auto;
        colony->Commands->Add( cmd );
        return true;
    }

    // UNLOAD
    if( m_RM->Match(s, m_RM->ExpCmdShipUnload) )
    {
        Ship^ ship = GameData::Player->FindShip(m_RM->Results[0], false);  
        if ( ship )
        {
            ICommand ^cmd = gcnew ShipCmdUnload(ship);
            cmd->Origin = CommandOrigin::Auto;
            ship->AddCommand( cmd );
            return true;
        }
        throw gcnew FHUIParsingException(
            String::Format("UNLOAD order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // JUMP
    if( m_RM->Match(s, "^Jump\\s+(\\w+)\\s+([^,]+),\\s+(.+)$") )
    {
        Ship^ ship = GameData::Player->FindShip(m_RM->Results[1], false);
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
                Colony ^colony = GameData::Player->FindColony(m_RM->Results[0]->TrimEnd(), false);
                if( colony )
                    cmd = gcnew ShipCmdJump(ship, colony->System, colony->PlanetNum);
            }
            if( cmd )
            {
                cmd->Origin = CommandOrigin::Auto;
                ship->AddCommand( cmd );
            }
            return true;
        }
        throw gcnew FHUIParsingException(
            String::Format("JUMP order for unknown ship: {0} {1}", m_RM->Results[0], m_RM->Results[1]) );
    }

    // DEVELOP
    if( m_RM->Match(s, m_RM->ExpCmdDevelopCS) )
    {
        Colony ^colony = GameData::Player->FindColony(m_RM->Results[1], false);
        Ship^ ship = GameData::Player->FindShip(m_RM->Results[2], false);
        if( !ship )
            throw gcnew FHUIParsingException("JUMP order for unknown ship: {0}" + m_RM->Results[2]);

        int cost = m_RM->GetResultInt(0) ? m_RM->GetResultInt(0) : ship->Capacity;

        ICommand ^cmd = gcnew ProdCmdDevelop( cost, colony, ship );
        m_TemplateColony->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return true;
    }
    if( m_RM->Match(s, m_RM->ExpCmdDevelopC) )
    {
        Colony ^colony = GameData::Player->FindColony(m_RM->Results[1], false);

        ICommand ^cmd = gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            colony,
            nullptr );
        m_TemplateColony->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return true;
    }
    if( m_RM->Match(s, m_RM->ExpCmdDevelop) )
    {
        ICommand ^cmd = gcnew ProdCmdDevelop(
            m_RM->GetResultInt(0),
            nullptr,
            nullptr );
        m_TemplateColony->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return true;
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
        m_TemplateColony->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return true;
    }

    // RECYCLE
    if( m_RM->Match(s, m_RM->ExpCmdRecycle) )
    {
        ICommand ^cmd = gcnew ProdCmdRecycle(
            FHStrings::InvFromString( m_RM->Results[1] ),
            m_RM->GetResultInt(0) );
        m_TemplateColony->Commands->Add( cmd );
        cmd->Origin = CommandOrigin::Auto;
        return true;
    }
    // RECYCLE SHIP
    if( m_RM->Match(s, m_RM->ExpCmdShipRec) )
    {
        Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
        ICommand ^cmd = gcnew ShipCmdRecycle( ship );
        cmd->Origin = CommandOrigin::Auto;
        ship->AddCommand( cmd );
        return true;
    }

    // AUTO
    if( s == "Auto" )
    {
        m_CommandMgr->AutoEnabled = true;
        return true;
    } 

    // SCAN
    if( m_RM->Match(s, m_RM->ExpCmdShipScan) )
    {
        Ship ^ship = GameData::Player->FindShip(m_RM->Results[0], false);
        ICommand ^cmd = gcnew ShipCmdScan( ship );
        cmd->Origin = CommandOrigin::Auto;
        ship->AddCommand( cmd );
        return true;
    }

    // ANY OTHER COMMAND -> CUSTOM COMMAND
    // unless phase is Jumps - no custom orders here
    if( m_TemplatePhase != CommandPhase::Jump )
    {
        CmdCustom ^cmd = gcnew CmdCustom(m_TemplatePhase, s, 0);
        cmd->Origin = CommandOrigin::Auto;
        if( m_TemplatePhase == CommandPhase::Production )
        {
            m_CommandMgr->AddCommand(m_TemplateColony, cmd);
        }
        else
        {
            m_CommandMgr->AddCommand(cmd);
        }
        return true;
    }
    return false;
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
