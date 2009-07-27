#include "StdAfx.h"
#include "GameData.h"
#include "Commands.h"

namespace FHUI
{

// ---------------------------------------------------------

String^ PrintInventory(array<int> ^inv)
{
    String ^ret = "";

    for( int i = 0; i < INV_MAX; ++i )
    {
        if( inv[i] )
        {
            ret = String::Format("{0}{1}{2} {3}",
                ret,
                String::IsNullOrEmpty(ret) ? "" : ", ",
                inv[i],
                FHStrings::InvToString(static_cast<InventoryType>(i)) );
        }
    }

    return ret;
}

// ---------------------------------------------------------

private ref class ColonyProdOrderComparer : public IComparer<Colony^>
{
public:
    virtual int Compare(Colony ^c1, Colony ^c2)
    {
        return c1->ProductionOrder - c2->ProductionOrder;
    }
};

void Alien::SortColoniesByProdOrder()
{
    Colonies->Sort( gcnew ColonyProdOrderComparer );
}

String^ Alien::PrintHome()
{
    if( HomeSystem == nullptr )
        return "???";
    String ^plName = nullptr;
    if( HomePlanet != -1 &&
        HomePlanet <= HomeSystem->PlanetsCount )
    {
        String ^n = HomeSystem->GetPlanets()[HomePlanet - 1]->Name;
        if( String::IsNullOrEmpty(n) )
            plName = "-";
        else
            plName = String::Format("PL {0}", n);
    }
    if( plName == nullptr )
    {
        plName = "?";
    }
    return String::Format( "{0} {1} ({2})",
        HomeSystem->PrintLocation(), HomePlanet, plName );
}

String^ Alien::PrintTechLevels()
{
    if( TechEstimateTurn == -1 )
        return "No estimates.";

    return String::Format(
        "Turn {6}: MI:{0} MA:{1} ML:{2} GV:{3} LS:{4} BI:{5}",
        TechLevels[TECH_MI],
        TechLevels[TECH_MA],
        TechLevels[TECH_ML],
        TechLevels[TECH_GV],
        TechLevels[TECH_LS],
        TechLevels[TECH_BI],
        TechEstimateTurn );
}

// ---------------------------------------------------------

int Planet::CalculateLSN(AtmosphericReq ^atmReq)
{
    int lsn =
        3 * Math::Abs(PressClass - atmReq->PressClass) +
        3 * Math::Abs(TempClass - atmReq->TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( Atmosphere[i] > 0 && atmReq->Poisonous[i] )
            lsn += 3;
    }

    if( Atmosphere[atmReq->GasRequired] < atmReq->ReqMin ||
        Atmosphere[atmReq->GasRequired] > atmReq->ReqMax )
    {
        lsn += 3;
    }

    lsn = Math::Min(lsn, 99); 

    return lsn;
}

void Planet::AddName(String ^name)
{
    Name = name;
    NameIsNew = true;
}

void Planet::DelName()
{
    Name = nullptr;
}

String^ Planet::PrintLocation()
{
    return String::Format("{0} {1}", System->PrintLocation(), Number);
}

String^ Planet::PrintComment()
{
    String ^ret = Comment;
    for each( Alien ^sp in SuspectedColonies->Keys )
        ret += String::Format(" SP {0} colony seen at turn {1}",
            sp->Name,
            SuspectedColonies[sp].ToString() );

    return ret;
}

// ---------------------------------------------------------

Planet^ StarSystem::GetPlanet(int plNum)
{
    if( plNum < 1 )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Invalid planet number: {0}!", plNum) );

    if( plNum <= m_Planets->Length )
        return m_Planets[plNum - 1];

    return nullptr;
}

String^ StarSystem::GenerateScan()
{
    String ^scan = String::Format(
        "Coordinates: x = {0}  y = {1}  z = {2}  stellar type = {3}  {4} planets.\r\n\r\n"
        "               Temp  Press Mining\r\n"
        "  #  Dia  Grav Class Class  Diff  LSN  Atmosphere\r\n"
        " ---------------------------------------------------------------------\r\n",
        X, Y, Z, Type,
        m_Planets->Length == 0 ? "?" : m_Planets->Length.ToString() );

    for( int i = 0; i < m_Planets->Length; ++i )
    {
        Planet ^planet = m_Planets[i];
        String ^plStr = String::Format(
            "{0,3}{1,5}{2,7:F2}{3,4}{4,6}{5,8:F2}{6,5}  ",
            i + 1,
            planet->Diameter,
            planet->Grav,
            planet->TempClass,
            planet->PressClass,
            (double)planet->MiDiff / 100,
            planet->LSN );
        bool anyGas = false;
        for( int gas = 0; gas < GAS_MAX; ++gas )
        {
            if( planet->Atmosphere[gas] )
            {
                plStr = String::Concat( plStr,
                    String::Format(
                        "{0}{1}({2}%)",
                        anyGas ? "," : "",
                        FHStrings::GasToString(static_cast<GasType>(gas)),
                        planet->Atmosphere[gas] ) );
                anyGas = true;
            }
        }
        if( !anyGas )
            plStr = String::Concat(plStr, "No atmosphere");

        plStr = String::Concat(plStr, "\r\n");
        scan  = String::Concat(scan, plStr);
    }

    return scan;
}

String^ StarSystem::PrintScanStatus()
{
    if( TurnScanned == -1 )     return s_ScanNone;
    else if( TurnScanned == 0 ) return s_ScanDipl;
    else                        return s_ScanSelf;
}

String^ StarSystem::PrintColonies(int planetNum, Alien ^player)
{
    String ^ret = "";

    for each( Colony ^colony in Colonies )
    {
        if( planetNum != -1 &&
            colony->PlanetNum != planetNum )
            continue;

        if( String::IsNullOrEmpty(ret) == false )
            ret += ", ";
    }

    for each( Colony ^colony in Colonies )
    {
        if( planetNum != -1 &&
            colony->PlanetNum != planetNum )
            continue;

        String ^entry = "";
        String ^size;
        if( colony->EconomicBase == -1 )
        {
            size = "?";
        }
        else if ( colony->EconomicBase % 10 )
        {
            size = String::Format("{0}.{1}", colony->EconomicBase / 10, colony->EconomicBase % 10);
        }
        else
        {
            size = (colony->EconomicBase / 10).ToString();
        }

        if( colony->Owner == player )
        {
            // Put player colony first, by name
            if( planetNum == -1 )
            {
                entry += ("#" + colony->PlanetNum.ToString() + " ");
            }

            entry += String::Format("PL {0}[{1}]", colony->Name, size );

            if( String::IsNullOrEmpty(ret) )
            {
                ret = entry;
            }
            else
            {
                ret = entry + ", " + ret;
            }
        }
        else
        {
            // Then list alien colonies by species name

            if( String::IsNullOrEmpty(ret) == false )
                ret += ", ";
            if( planetNum == -1 )
                ret += ("#" + colony->PlanetNum.ToString() + " ");
            ret += String::Format( "{0}{1}[{2}]",
                colony->PlanetType == PLANET_HOME ? "(HOME) " : "",
                colony->Owner->Name,
                size );
        }
    }

    for each (Planet^ planet in GetPlanets() )
    {
        if ( ( planet->NumColonies == 0 ) &&
             ( String::IsNullOrEmpty( planet->Name ) == false ) )
        {
            // Planet has no colonies, list name if any.
            if( String::IsNullOrEmpty(ret) == false )
                ret += ", ";

            if( planetNum == -1 )
            {
                ret += ("#" + planet->Number.ToString() + " ");
            }

            ret += "PL " + planet->Name;
        }
    }

    return ret;
}

String^ StarSystem::PrintAlienShipSummary()
{
    // TODO: Generate summary, examples below
    // "TR1, TR10, 2x TR12"
    // "2x DDS, 4x CLS"
    // TODO2: Generate one list for each species
    return "";
}

/*
String^ StarSystem::PrintShips()
{
    String ^ret = "";

    for each( Ship ^ship in Ships )
    {
    }

    return ret;
}
*/

void StarSystem::Planets::set(int plNum, Planet ^pl)
{
    Array::Resize(m_Planets, Math::Max(plNum + 1, m_Planets->Length));
    m_Planets[plNum] = pl;
}

// ---------------------------------------------------------

String^ Colony::PrintInventoryShort()
{
    return PrintInventory(Inventory);
}

String^ Colony::PrintRefListEntry(Alien ^player)
{
    return Name + (player != Owner ? (" (" + Owner->Name + ")") : "");
}

String^ Colony::PrintBalance()
{
    if( NeedAU > 0 )
        return String::Format("+{0} AU", NeedAU);
    else if( NeedIU )
        return String::Format("+{0} IU", NeedIU);
    else
        return "Balanced";
}

void Colony::CalculateBalance(bool MiMaBalanced)
{
    int miTech = Owner->TechLevelsAssumed[TECH_MI];
    int maTech = Owner->TechLevelsAssumed[TECH_MA];
    if( MiMaBalanced )
        miTech = maTech = Math::Min(miTech, maTech);

    int mi = (10 * (miTech * MiBase)) / MiDiff;
    int ma = (maTech * MaBase) / 10;

    NeedIU = 0;
    NeedAU = 0;

    int diff = Math::Abs(mi - ma);
    if( mi < ma )
    {
        NeedIU = (int)Math::Round( (double)(diff * MiDiff ) / ( miTech * 10) );
        NeedAU = -NeedIU;
    }
    else
    {
        NeedAU = (int)Math::Round( (double)(diff * 10) / maTech );
        NeedIU = -NeedAU;
    }
}

int Colony::GetMaxProductionBudget()
{
    switch( PlanetType )
    {
    case PLANET_HOME:   return int::MaxValue;
    case PLANET_COLONY: return EUAvail * 2;
    default:            return 0;
    }
}

// ---------------------------------------------------------

String^ Ship::PrintClass()
{
    return String::Format(
        "{0}{1}{2}",
        FHStrings::ShipToString( Type ),
        Type == SHIP_TR ? Size.ToString() : "",
        SubLight ? "s" : "" );
}

String^ Ship::PrintRefListEntry()
{
    return PrintClass() + " " + Name + " (A" + Age.ToString() + ")";
}

String^ Ship::PrintLocation()
{
    String ^xyz;

    if( System )
    {
        if( PlanetNum == -1 )
        {
            // check if there is player colony in the system
            int biggest = 0;
            for each( Colony ^colony in System->ColoniesOwned )
            {
                if( colony->EconomicBase > biggest )
                {
                    PlanetNum = colony->PlanetNum;
                    biggest = colony->EconomicBase;
                }
            }
        }

        if( PlanetNum != -1 )
        {
            Planet ^planet = System->Planets[PlanetNum - 1];
            if( planet != nullptr &&
                !String::IsNullOrEmpty(planet->Name) )
            {
                xyz = String::Format("PL {0}", planet->Name);
            }
            else
                xyz = String::Format("[{0} {1}]", System->PrintLocation(), PlanetNum);
        }
        else
        {
           xyz = String::Format("[{0}]", System->PrintLocation());
        }
    }
    else
    {
        if( PlanetNum != -1 )
            xyz = String::Format("[{0} {1}]", System->PrintLocation(), PlanetNum);
        else
            xyz = System->PrintLocation();
    }

    switch( Location )
    {
    case SHIP_LOC_DEEP_SPACE:   return xyz + ", Deep";
    case SHIP_LOC_ORBIT:        return xyz + ", Orbit";
    case SHIP_LOC_LANDED:       return xyz + ", Landed";
    }

    int l = Location;
    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid ship location! ({0})", l) );
}

String^ Ship::PrintCargo()
{
    return PrintInventory(m_Cargo);
}

void Ship::CalculateCapacity()
{
    switch( Type )
    {
    case SHIP_BAS:  Capacity = Size / 1000; break;
    case SHIP_TR:   Capacity = Calculators::TransportCapacity(Size); break;
    case SHIP_PB:   Capacity = 1;       break;
    case SHIP_CT:   Capacity = 2;       break;
    case SHIP_ES:   Capacity = 5;       break;
    case SHIP_FF:   Capacity = 10;      break;
    case SHIP_DD:   Capacity = 15;      break;
    case SHIP_CL:   Capacity = 20;      break;
    case SHIP_CS:   Capacity = 25;      break;
    case SHIP_CA:   Capacity = 30;      break;
    case SHIP_CC:   Capacity = 35;      break;
    case SHIP_BC:   Capacity = 40;      break;
    case SHIP_BS:   Capacity = 45;      break;
    case SHIP_DN:   Capacity = 50;      break;
    case SHIP_SD:   Capacity = 55;      break;
    case SHIP_BM:   Capacity = 60;      break;
    case SHIP_BW:   Capacity = 65;      break;
    case SHIP_BR:   Capacity = 70;      break;
    }
}

void Ship::SetupTonnage()
{
    switch( Type )
    {
    case SHIP_BAS:  Tonnage = Size; break;
    case SHIP_TR:   Tonnage = Size * 10000; break;
    case SHIP_PB:   Tonnage = 10000;  break;
    case SHIP_CT:   Tonnage = 20000;  break;
    case SHIP_ES:   Tonnage = 50000;  break;
    case SHIP_FF:   Tonnage = 100000; break;
    case SHIP_DD:   Tonnage = 150000; break;
    case SHIP_CL:   Tonnage = 200000; break;
    case SHIP_CS:   Tonnage = 250000; break;
    case SHIP_CA:   Tonnage = 300000; break;
    case SHIP_CC:   Tonnage = 350000; break;
    case SHIP_BC:   Tonnage = 400000; break;
    case SHIP_BS:   Tonnage = 450000; break;
    case SHIP_DN:   Tonnage = 500000; break;
    case SHIP_SD:   Tonnage = 550000; break;
    case SHIP_BM:   Tonnage = 600000; break;
    case SHIP_BW:   Tonnage = 650000; break;
    case SHIP_BR:   Tonnage = 700000; break;
    }

    OriginalCost = Tonnage / 100;
    if( SubLight )
        OriginalCost = (3 * OriginalCost) / 4;

    if( Type == SHIP_TR )
        WarTonnage = Tonnage / 10;
    else
        WarTonnage = Tonnage;
}

int Ship::GetMaintenanceCost()
{
    if( Type == SHIP_BAS )
        return OriginalCost / 10;
    if( Type == SHIP_TR )
        return OriginalCost / 25;
    return OriginalCost / 5;
}

String^ Ship::Order::Print()
{
    switch( Type )
    {
    case OrderType::Jump:
        return "Jump to " + PrintJumpDestination();
    case OrderType::Upgrade:
        return "Upgrade";
    case OrderType::Recycle:
        return "Recycle";
    }

    return "Invalid command";
}

String^ Ship::Order::PrintNumeric()
{
    if( Type == OrderType::Jump )
        return String::Format("Jump to {0} {1} {2} {3}",
            JumpTarget->X,
            JumpTarget->Y,
            JumpTarget->Z,
            PlanetNum );

    return Print();
}

String^ Ship::Order::PrintJumpDestination()
{
    if( PlanetNum != -1 )
    {
        Planet ^planet = JumpTarget->GetPlanet(PlanetNum);
        if( planet )
        {
            if( String::IsNullOrEmpty(planet->Name) )
                return planet->PrintLocation();
            else
                return "PL " + planet->Name;
        }
        return JumpTarget->PrintLocation() + " " + PlanetNum.ToString();
    }
    return JumpTarget->PrintLocation();
}

// ---------------------------------------------------------

GameData::GameData(void)
    : m_Species(nullptr)
    , m_TurnEUStart(0)
    , m_TurnEUProduced(0)
    , m_FleetCost(0)
    , m_FleetCostPercent(0.0)
    , m_Aliens(gcnew SortedList<String^, Alien^>)
    , m_Systems(gcnew array<StarSystem^>(0))
    , m_Colonies(gcnew SortedList<String^, Colony^>)
    , m_PlanetNames(gcnew SortedList<String^, PlanetName^>)
    , m_Ships(gcnew SortedList<String^, Ship^>)
    , m_Commands(gcnew List<ICommand^>)
    , m_TurnMax(0)
{
}

// ---------------------------------------------------------

String^ GameData::GetSummary()
{
    return String::Format(
        "{0}"
        "---------------------------\r\n"
        "{1}"
        "---------------------------\r\n"
        "{2}"
        "---------------------------\r\n"
        "{3}"
        "---------------------------\r\n"
        "{4}"
        "---------------------------\r\n"
        "{5}"
        "---------------------------\r\n",
        GetSpeciesSummary(),
        GetAllTechsSummary(),
        GetEconomicSummary(),
        GetAliensSummary(),
        GetShipsSummary(),
        GetPlanetsSummary() );
}

String^ GameData::GetSpeciesSummary()
{
    AtmosphericReq ^atm = m_Species->AtmReq;

    String ^toxicGases = "";
    for( int gas = 0; gas < GAS_MAX; ++gas )
    {
        if( atm->Poisonous[gas] )
            toxicGases = toxicGases + String::Format(",{0}", FHStrings::GasToString(static_cast<GasType>(gas)));
    }

    return String::Format(
        "Species: {0}\r\n"
        "Home: [{1} {2} {3} {4}]\r\n"
        "Temp:{5} Press:{6}  Atm:{7} {8}-{9}%\r\n"
        "Poisons:{10}\r\n",
        GetSpeciesName(),
        m_Species->HomeSystem->X,
        m_Species->HomeSystem->Y,
        m_Species->HomeSystem->Z,
        m_Species->HomePlanet,
        atm->TempClass == -1 ? "??" : atm->TempClass.ToString(),
        atm->PressClass == -1 ? "??" : atm->PressClass.ToString(),
        FHStrings::GasToString( atm->GasRequired ),
        atm->ReqMin,
        atm->ReqMax,
        toxicGases->Substring(1));
}

String^ GameData::GetAllTechsSummary()
{
    return String::Format(
        "  MI: {0} | GV: {3}\r\n"
        "  MA: {1} | LS: {4}\r\n"
        "  ML: {2} | BI: {5}\r\n",
        GetTechSummary(TECH_MI),
        GetTechSummary(TECH_MA),
        GetTechSummary(TECH_ML),
        GetTechSummary(TECH_GV),
        GetTechSummary(TECH_LS),
        GetTechSummary(TECH_BI) );
}

String^ GameData::GetEconomicSummary()
{
    return String::Format(
        "EUs  carried: {0,7}\r\n"
        "    produced: {1,7}\r\n"
        "Fleet maint.: {2} ({3:F2}%)\r\n"
        "     Budget : {4,7}\r\n",
        m_TurnEUStart,
        m_TurnEUProduced,
        m_FleetCost, m_FleetCostPercent,
        m_TurnEUProduced + m_TurnEUStart - m_FleetCost);
}

String^ GameData::GetTechSummary(TechType tech)
{
    int lev = m_Species->TechLevels[tech];
    int levTeach = m_Species->TechLevelsTeach[tech];

    if( lev != levTeach )
    {
        return String::Format("{0,2}/{1,2}", lev, levTeach);
    }

    return String::Format("{0,2}   ", lev);
}

private ref class AlienRelComparer : public IComparer<Alien^>
{
public:
    virtual int Compare(Alien ^a1, Alien ^a2)
    {
        if( a1->Relation == a2->Relation )
            return String::Compare(a1->Name, a2->Name);
        return a1->Relation - a2->Relation;
    }
};

String^ GameData::GetAliensSummary()
{
    String ^ret = "";

    List<Alien^> ^aliens = gcnew List<Alien^>;
    for each( Alien ^alien in GetAliens() )
    {
        if( alien != m_Species &&
            alien->Relation != SP_PIRATE )
        {
            aliens->Add(alien);
        }
    }

    aliens->Sort( gcnew AlienRelComparer );

    for each( Alien ^alien in aliens )
    {
        String ^prefix = "N";
        if( alien->Relation == SP_ALLY )
            prefix = "A";
        else if( alien->Relation == SP_ENEMY )
            prefix = "E";

        ret = String::Format("{0}{1}: SP {2}\r\n",
            ret, prefix, alien->Name );
    }

    return ret;
}

String^ GameData::GetPlanetsSummary()
{
    String ^ret = "";

    for each( StarSystem ^system in GetStarSystems() )
    {
        for each( Planet ^planet in system->GetPlanets() )
        {
            if( String::IsNullOrEmpty(planet->Name) )
                continue;

            ret += String::Format("PL {0} @ {1}\r\n",
                planet->Name,
                planet->PrintLocation() );
        }
    }

    return ret;
}

private ref class ShipLocComparer : public IComparer<Ship^>
{
public:
    virtual int Compare(Ship ^s1, Ship ^s2)
    {
        if( s1->System->X != s2->System->X ) return s1->System->X - s2->System->X;
        if( s1->System->Y != s2->System->Y ) return s1->System->Y - s2->System->Y;
        if( s1->System->Z != s2->System->Z ) return s1->System->Z - s2->System->Z;
        return s1->Location - s2->Location;
    }
};

String^ GameData::GetShipsSummary()
{
    List<Ship^> ^ships = GetShips(m_Species);
    if( ships->Count == 0 )
        return "You have no ships\r\n";

    ships->Sort( gcnew ShipLocComparer );

    String ^ret = "";
    for each( Ship ^ship in ships )
        ret += String::Format("{0} {1} @{2}\r\n",
            ship->PrintClass(),
            ship->Name,
            ship->PrintLocation() );

    return ret;
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
        return m_Aliens[spKey];

    throw gcnew FHUIDataIntegrityException(
        String::Format("Species not found: {0}", sp));
}

Ship^ GameData::GetShip(String ^name)
{
    return safe_cast<Ship^>(m_Ships[name->ToLower()]);
}

List<Ship^>^ GameData::GetShips(StarSystem ^sys, Alien ^sp)
{
    List<Ship^> ^ret = gcnew List<Ship^>; 

    for each( Ship ^ship in GetShips() )
    {
        if( sp && ship->Owner != sp )
            continue;
        if( sys && ship->System != sys )
            continue;
        ret->Add(ship);
    }

    return ret;
}

Colony^ GameData::GetColony(String ^name)
{
    return safe_cast<Colony^>(m_Colonies[name->ToLower()]);
}

List<Colony^>^ GameData::GetColonies(StarSystem ^sys, Alien ^sp)
{
    List<Colony^> ^ret = gcnew List<Colony^>; 

    for each( Colony ^colony in GetColonies() )
    {
        if( sp && colony->Owner != sp )
            continue;
        if( sys && colony->System != sys )
            continue;
        ret->Add(colony);
    }

    return ret;
}

// ---------------------------------------------------------

bool GameData::TurnCheck(int turn)
{
    if( turn > m_TurnMax )
    {
        m_TurnMax = turn;

        // New turn cleanup
        for each( Alien ^alien in GetAliens() )
        {
            if( alien != m_Species && alien->Relation != SP_PIRATE )
                alien->Relation = SP_NEUTRAL; // FIXME: default relation may be different...
        }

        m_TurnEUStart       = 0;
        m_TurnEUProduced    = 0;
        m_FleetCost         = 0;
        m_FleetCostPercent  = 0.0;

        // Remove all player's colonies in case one of them no longer exists
        bool removed = false;
        do {
            removed = false;
            for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
            {
                if( iter->Value->Owner == m_Species )
                {
                    m_Colonies->Remove(iter->Key);
                    removed = true;
                    break;
                }
            }
        } while( removed );

        // remove planet names
        m_PlanetNames->Clear();
        // remove ships
        m_Ships->Clear();
    }
    return turn == m_TurnMax;
}

int GameData::TurnAlign(int turn)
{
    if( turn == 0 )
    {
        return TURN_NOTES;
    }
    return turn;
}

void GameData::SetSpecies(String ^sp)
{
    if( m_Species == nullptr )
    {
        m_Species = AddAlien(0, sp);
        m_Species->Relation = SP_PLAYER;
    }

    if( String::Compare(m_Species->Name, sp) != 0 )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Reports for different species: {0} and {1}",
                m_Species->Name, sp) );
}

Alien^ GameData::AddAlien(int turn, String ^sp)
{
    String ^spKey = sp->ToLower();
    if( m_Aliens->ContainsKey(spKey) )
    {
        Alien ^alien = m_Aliens[spKey];
        alien->TurnMet = Math::Min(TurnAlign(turn), alien->TurnMet);

        return alien;
    }

    Alien ^alien = gcnew Alien(sp, turn);
    m_Aliens->Add(spKey, alien);

    return alien;
}

void GameData::SetAlienRelation(int turn, String ^sp, SPRelType rel)
{
    if( TurnCheck(turn) )
    {
        Alien ^alien = AddAlien(turn, sp);
        alien->Relation = rel;
    }
}

void GameData::SetTechLevel(int turn, Alien ^sp, TechType tech, int lev, int levTeach)
{
    if( TurnCheck(turn) || sp->TechEstimateTurn == -1 )
    {
        m_TurnMax = turn;

        sp->TechEstimateTurn        = turn;
        sp->TechLevels[tech]        = lev;
        sp->TechLevelsAssumed[tech] = lev;
        sp->TechLevelsTeach[tech]   = levTeach;
    }
}

void GameData::SetFleetCost(int turn, int cost, float percent)
{
    if( TurnCheck(turn) )
    {
        m_FleetCost = cost;
        m_FleetCostPercent = percent;
    }
}

void GameData::SetAtmosphereReq(GasType gas, int reqMin, int reqMax)
{
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( (atm->GasRequired != GAS_MAX && atm->GasRequired != gas) ||
        atm->Neutral[gas] ||
        atm->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->GasRequired = gas;
    atm->ReqMin = reqMin;
    atm->ReqMax = reqMax;
}

void GameData::SetAtmosphereNeutral(GasType gas)
{
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Poisonous[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Neutral[gas] = true;
}

void GameData::SetAtmospherePoisonous(GasType gas)
{
    AtmosphericReq ^atm = GetSpecies()->AtmReq;

    if( atm->GasRequired == gas ||
        atm->Neutral[gas] )
    {
        throw gcnew FHUIDataIntegrityException("Inconsistent atmospheric data in reports.");
    }

    atm->Poisonous[gas] = true;
}

StarSystem^ GameData::GetStarSystem(int x, int y, int z)
{
    for each( StarSystem ^system in m_Systems )
    {
        if( system->X == x &&
            system->Y == y &&
            system->Z == z )
        {
            return system;
        }
    }

    throw gcnew FHUIDataIntegrityException(
        String::Format("Trying to get not existing star system: {0} {1} {2}", x, y, z) );
}

void GameData::AddStarSystem(int x, int y, int z, String ^type, String ^comment)
{
    for each( StarSystem ^system in m_Systems )
    {
        if( system->X == x &&
            system->Y == y &&
            system->Z == z )
        {
            throw gcnew FHUIDataIntegrityException(
                String::Format("Star system already added: [{0} {1} {2}]", x, y, z) );
        }
    }

    StarSystem ^system = gcnew StarSystem(x, y, z, type);
    system->Comment = comment;
    system->IsVoid = false;
    Array::Resize(m_Systems, m_Systems->Length + 1);
    m_Systems[m_Systems->Length - 1] = system;
}

void GameData::AddPlanetScan(int turn, int x, int y, int z, Planet ^planet)
{
    StarSystem ^system = GetStarSystem(x, y, z);
    if( system == nullptr )
        throw gcnew FHUIDataIntegrityException(
            String::Format("Star system [{0} {1} {2}] not found in galaxy list!", x, y, z) );

    int plNum = planet->Number - 1;
    if( system->PlanetsCount < planet->Number ||
        system->Planets[plNum] == nullptr ||
        turn == m_TurnMax )
    {
        if( system->PlanetsCount >= planet->Number &&
            system->Planets[plNum] != nullptr )
        {
            Planet ^plExisting = system->Planets[plNum];
            if( !String::IsNullOrEmpty(plExisting->Comment) )
            {
                if( String::IsNullOrEmpty(planet->Comment) )
                    planet->Comment = plExisting->Comment;
                else
                    planet->Comment += "; " + plExisting->Comment;
            }
            //TODO: WTF? WHY DOES IT CRASH???
            //planet->SuspectedColonies = plExisting->SuspectedColonies;

            for each( Colony ^colony in m_Colonies->Values )
                if( colony->Planet == plExisting )
                    colony->Planet = planet;
        }
        system->Planets[plNum] = planet;
        system->TurnScanned   = Math::Max(system->TurnScanned, turn);
    }
}

void GameData::SetTurnStartEU(int turn, int eu)
{
    if( TurnCheck(turn) )
    {
        m_TurnEUStart = eu;
    }
}

void GameData::AddTurnProducedEU(int turn, int eu)
{
    if( TurnCheck(turn) )
    {
        m_TurnEUProduced += eu;
    }
}

Colony^ GameData::AddColony(int turn, Alien ^sp, String ^name, StarSystem ^system, int plNum)
{
    bool createColony = false;

    if( TurnCheck(turn) )
    {
        createColony = true;
    }

    if( createColony )
    {
        Colony ^colony = gcnew Colony(sp, name, system, plNum);
        m_Colonies[name->ToLower()] = colony;

        if( system->PlanetsCount < plNum )
        {
            // System is not yet known, initialize with defaults
            for(int i = 0; i < plNum; ++i)
            {
                if( system->Planets[i] == nullptr )
                    system->Planets[i] = Planet::Default(system, i + 1);
            }
        }
        colony->Planet = system->GetPlanet(plNum);
        //TODO: WTF? WHY DOES IT CRASH???
        //if( colony->Planet->SuspectedColonies->ContainsKey(sp) )
        //    colony->Planet->SuspectedColonies->Remove(sp);

        // If this is species' own colony, remove alien colonies from this system.
        // They'll be restored by 'Aliens at...' parsing. If not - it means that alien
        // colony was destroyed or assimilated.
        if( sp == m_Species )
        {
            bool bRemoved = false;
            do
            {
                bRemoved = false;
                for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
                {
                    Colony ^c = iter->Value;
                    if( c->Owner != m_Species &&
                        c->System == system )
                    {
                        m_Colonies->Remove(iter->Key);
                        bRemoved = true;
                        break; // for each
                    }
                }
            } while( bRemoved );
        }

        return colony;
    }

    return nullptr;
}

void GameData::AddPlanetName(int turn, StarSystem ^system, int pl, String ^name)
{
    if( TurnCheck(turn) )
    {
        m_PlanetNames[name->ToLower()] = gcnew PlanetName(system, pl, name);
    }
}

void GameData::Update()
{
    UpdateShips();
    UpdateAliens();
    UpdateSystems();
    LinkPlanetNames();
    UpdateHomeWorlds();
    UpdateColonies();
}

private ref class ShipWarTonnageComparer : public IComparer<Ship^>
{
public:
    virtual int Compare(Ship ^s1, Ship ^s2)
    {
        // Newer, Bigger ships first
        if( s2->WarTonnage == s1->WarTonnage )
            return s1->Age - s2->Age;
        return s2->WarTonnage - s1->WarTonnage;
    }
};

void GameData::UpdateShips()
{
    m_ShipsByTonnage = gcnew List<Ship^>;
    m_ShipsByTonnage->AddRange(m_Ships->Values);
    m_ShipsByTonnage->Sort( gcnew ShipWarTonnageComparer );
}

void GameData::UpdateAliens()
{
    for each( Alien ^alien in GetAliens() )
    {
        alien->RelationOriginal = alien->Relation;

        alien->Colonies = GetColonies(nullptr, alien);
        alien->Ships    = GetShips(nullptr, alien);
    }
}

void GameData::UpdateSystems()
{
    for each( StarSystem ^system in m_Systems )
    {
        for each( Colony ^colony in GetColonies() )
        {
            if( colony->System == system )
            {
                system->Colonies->Add(colony);
                if( colony->Owner == GetSpecies() )
                    system->ColoniesOwned->Add(colony);
                else
                    system->ColoniesAlien->Add(colony);
            }
        }
        for each( Ship ^ship in GetShips() )
        {
            if( ship->System == system )
            {
                system->Ships->Add(ship);
                if( ship->Owner == GetSpecies() )
                    system->ShipsOwned->Add(ship);
                else
                    system->ShipsAlien->Add(ship);
            }
        }

        // Calculate LSN
        int minLSN = 99999;
        int minLSNAvail = 99999;
        for each( Planet ^planet in system->GetPlanets() )
        {
            planet->LSN = planet->CalculateLSN(m_Species->AtmReq);
            planet->NumColonies = 0;
            planet->NumColoniesOwned = 0;

            // Update min LSN and num colonies
            minLSN = Math::Min(planet->LSN, minLSN);
            bool available = true;
            for each( Colony ^colony in system->Colonies )
            {
                if( colony->PlanetNum == planet->Number )
                {
                    ++planet->NumColonies;
                    if( colony->Owner == GetSpecies() )
                        ++planet->NumColoniesOwned;
                    available = false;

                    // Update planet info with colony info, if there is any difference
                    // Helps with MD changed via mining or when there is no system scan
                    if ( colony->Owner == GetSpecies() )
                    {
                        colony->CalculateBalance(false);

                        planet->MiDiff = colony->MiDiff;
                        planet->LSN = colony->LSN;
                    }

                    break;
                }
            }
            if( available )
                minLSNAvail = Math::Min(planet->LSN, minLSNAvail);
        }

        system->MinLSN      = minLSN;
        system->MinLSNAvail = minLSNAvail;

        system->UpdateMaster();
    }
}

void StarSystem::UpdateMaster()
{
    for each( Colony ^colony in Colonies )
    {
        if( Master == nullptr )
            Master = colony->Owner;
        else if( Master != colony->Owner &&
                 Master->Relation != SP_MIXED )
        {
            Master = gcnew Alien("", 0);
            Master->Relation = SP_MIXED;
        }

        Planet ^planet = Planets[colony->PlanetNum - 1];
        if( planet )
        {
            if( planet->Master == nullptr )
                planet->Master = colony->Owner;
            else if( planet->Master != colony->Owner &&
                     planet->Master->Relation != SP_MIXED )
            {
                planet->Master = Master;
            }
        }
    }
}

void GameData::LinkPlanetNames()
{
    for each( Colony ^colony in GetColonies() )
    {
        StarSystem ^system = colony->System;
        colony->Planet = system->GetPlanet( colony->PlanetNum );

        if( colony->Owner == m_Species )
        {
            if( colony->Planet == nullptr )
                throw gcnew FHUIDataIntegrityException(
                    String::Format("Colony PL {0} at [{1} {2} {3} {4}]: Planet not scanned!",
                        colony->Name,
                        system->X, system->Y, system->Z,
                        colony->PlanetNum ) );

            colony->Planet->Name = colony->Name;
        }
    }

    for each( PlanetName ^plName in m_PlanetNames->Values )
    {
        Planet ^planet = plName->System->GetPlanet( plName->PlanetNum );
        if( planet )
            planet->Name = plName->Name;
    }
}

void GameData::UpdateHomeWorlds()
{
    for each( Colony ^colony in GetColonies() )
    {
        if( colony->PlanetType == PLANET_HOME )
        {
            Alien ^sp = colony->Owner;
            if( sp->AtmReq->IsValid() )
                continue;

            Planet ^planet = colony->System->GetPlanet(colony->PlanetNum);
            if( planet )
            {
                sp->AtmReq->TempClass = planet->TempClass;
                sp->AtmReq->PressClass = planet->PressClass;
                for( int i = 0; i < GAS_MAX; ++i )
                {
                    if( planet->Atmosphere[i] )
                        sp->AtmReq->Neutral[i] = true;
                }
            }
        }
    }
}

void GameData::UpdateColonies()
{
    int order = 0;

    // Mining and resort colonies first
    for each( Colony ^colony in GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_COLONY_MINING ||
            colony->PlanetType == PLANET_COLONY_RESORT )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = false;
        }

    // Colony planets
    for each( Colony ^colony in GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_COLONY )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
        }

    // Home planet
    for each( Colony ^colony in GetSpecies()->Colonies )
        if( colony->PlanetType == PLANET_HOME )
        {
            colony->ProductionOrder = ++order;
            colony->CanProduce = true;
        }

    GetSpecies()->SortColoniesByProdOrder();
}

Ship^ GameData::AddShip(int turn, Alien ^sp, ShipType type, String ^name, bool subLight, StarSystem ^system)
{
    if( TurnCheck(turn) )
    {
        Ship ^ship = gcnew Ship(sp, type, name, subLight);
        ship->System = system;
        m_Ships[name->ToLower()] = ship;

        if( sp == m_Species )
        {
            // Remove alien colonies from this system.
            // They'll be restored by 'Aliens at...' parsing. If not - it means that alien
            // colony was destroyed, assimilated or it is hiding. Add a note.
            bool bRemoved = false;
            do
            {
                bRemoved = false;
                for each( KeyValuePair<String^, Colony^> ^iter in m_Colonies )
                {
                    Colony ^c = iter->Value;
                    if( c->Owner != m_Species &&
                        c->System == system )
                    {
                        if( c->Planet == nullptr )
                        {
                            // System is not yet known, initialize with defaults
                            for(int i = 0; i < c->PlanetNum; ++i)
                            {
                                if( c->System->Planets[i] == nullptr )
                                    c->System->Planets[i] = Planet::Default(c->System, i + 1);
                            }
                            c->Planet = c->System->GetPlanet(c->PlanetNum);
                        }
                        //TODO: WTF? WHY DOES IT CRASH???
                        //c->Planet->SuspectedColonies[c->Owner] = c->LastSeen;

                        m_Colonies->Remove(iter->Key);
                        bRemoved = true;
                        break; // for each
                    }
                }
            } while( bRemoved );
        }

        return ship;
    }

    return nullptr;
}

private ref class CommandComparer : public IComparer<ICommand^>
{
public:
    virtual int Compare(ICommand ^c1, ICommand ^c2)
    {
        return (int)c1->GetType() - (int)c2->GetType();
    }
};

void GameData::SortCommands()
{
    m_Commands->Sort( gcnew CommandComparer );
}

} // end namespace FHUI
