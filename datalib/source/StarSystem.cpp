#include "StdAfx.h"
#include "GameData.h"

namespace FHUI
{

// -----------------------------------------------

private ref class PrintKey : public IComparable
{
public:
    PrintKey(Alien ^sp, int plNum, String ^name)
        : m_Sp(sp), m_PlNum(plNum), m_Name(name)
    {
    }

    virtual int CompareTo(Object ^o)
    {
        PrintKey ^key = (PrintKey^)o;

        int n = m_Sp->Relation - key->m_Sp->Relation;
        if( n == 0 )
            n = String::Compare(m_Sp->Name, key->m_Sp->Name);
        if( n == 0 )
            n = m_PlNum - key->m_PlNum;
        if( n == 0 )
            n = m_Name->CompareTo(key->m_Name);
        return n;
    }

    Alien^  m_Sp;
    int     m_PlNum;
    String^ m_Name;
};

// -----------------------------------------------

void StarSystem::AddShip(Ship^ ship)
{
    m_Ships->Add(ship);
    if (ship->Owner == GameData::Player)
    {
        m_ShipsOwned->Add(ship);
    }
    else
    {
        m_ShipsAlien->Add(ship);
    }
}

void StarSystem::RemoveShip(Ship^ ship)
{
    m_Ships->Remove(ship);
    if (ship->Owner == GameData::Player)
    {
        m_ShipsOwned->Remove(ship);
    }
    else
    {
        m_ShipsAlien->Remove(ship);
    }
}

void StarSystem::SortShipsByTonnage()
{
    IComparer<Ship^> ^cmpr = gcnew Ship::WarTonnageComparer;
    m_ShipsOwned->Sort(cmpr);
    m_ShipsAlien->Sort(cmpr);
}

void StarSystem::AddColony(Colony^ colony)
{
    // Keep the list sorted - insert new elements in the proper place
    m_Colonies->Add(colony);
    if (colony->Owner == GameData::Player)
    {
        m_ColoniesOwned->Add(colony->PlanetNum, colony);
    }
    else
    {
        m_ColoniesAlien->Add(colony);
    }
}

void StarSystem::DeleteAlienColonies()
{
    // Remove alien colonies from this system.
    // They'll be restored by 'Aliens at...' parsing. If not - it means that alien
    // colony was destroyed, assimilated or it is hiding. Add a note.
    for each( Colony ^colony in m_ColoniesAlien )
    {
        m_Colonies->Remove( colony );
        colony->Owner->Colonies->Remove( colony );
    }

    m_ColoniesAlien->Clear();
}

double StarSystem::CalcDistance(StarSystem ^s)
{
    return IsWormholeTarget(s)
        ? 0
        : Calculators::Distance(X, Y, Z, s->X, s->Y, s->Z);
}

double StarSystem::CalcMishap(StarSystem ^s, int gv, int age)
{
    return IsWormholeTarget(s)
        ? 0
        : Calculators::Mishap(X, Y, Z, s->X, s->Y, s->Z, gv, age);
}

double StarSystem::CalcDistance(int x, int y, int z)
{
    if( HasWormhole && GameData::GetSystemId(x, y, z) == WormholeTargetId )
        return 0;

    return Calculators::Distance(X, Y, Z, x, y, z);
}

double StarSystem::CalcMishap(int x, int y, int z, int gv, int age)
{
    if( HasWormhole && GameData::GetSystemId(x, y, z) == WormholeTargetId )
        return 0;

    return Calculators::Mishap(X, Y, Z, x, y, z, gv, age);
}

Int32 StarSystem::CompareTo( Object^ obj )
{
    StarSystem^ system = safe_cast<StarSystem^>(obj);
    if ( X != system->X ) return X - system->X;
    if ( Y != system->Y ) return Y - system->Y;
    return Z - system->Z;
}

int StarSystem::CompareLocation(StarSystem ^sys)
{
    if( X != sys->X ) return X - sys->X;
    if( Y != sys->Y ) return Y - sys->Y;
                      return Z - sys->Z;
}

bool StarSystem::IsWormholeTarget(StarSystem ^system)
{
    return HasWormhole ? system->GetId() == WormholeTargetId : false;
}

StarSystem^ StarSystem::GetWormholeTarget()
{
    if( HasWormhole && WormholeTargetId != -1 )
        return GameData::GetStarSystem(WormholeTargetId);
    return nullptr;
}

void StarSystem::UpdateTooltip()
{
    GenerateScan(false);
    GenerateColoniesInfo();
    GenerateShipsInfo();

    m_Tooltip = m_TooltipScan;
    if( !String::IsNullOrEmpty(m_TooltipColonies) )
        m_Tooltip += "\r\nColonies:\r\n" + m_TooltipColonies;
    if( !String::IsNullOrEmpty(m_TooltipShips) )
        m_Tooltip += "\r\nShips:\r\n" + m_TooltipShips;
}

String^ StarSystem::GenerateColoniesInfo()
{
    if( m_TooltipColonies )
        return m_TooltipColonies;

    m_TooltipColonies = "";

    if( Colonies->Count )
    {
        SortedList<PrintKey^, Colony^> ^colonies =
            gcnew SortedList<PrintKey^, Colony^>;
        for each( Colony ^colony in Colonies )
            colonies->Add(
                gcnew PrintKey(colony->Owner, colony->PlanetNum, colony->Name),
                colony);

        Alien ^player = GameData::Player;
        for each( Colony ^colony in colonies->Values )
        {
            m_TooltipColonies += String::Format("#{0} {1} {2}{3}, size {4}{5}\r\n",
                colony->PlanetNum,
                colony->Owner->Name,
                FHStrings::PlTypeToString(colony->PlanetType),
                colony->Owner == player ? (" " + colony->Name) : "",
                colony->PrintSize(),
                colony->Owner == player ? (" [" + colony->PrintInventory(true)) + "]" : "" );
        }
    }

    return m_TooltipColonies;
}

String^ StarSystem::GenerateShipsInfo()
{
    if( m_TooltipShips )
        return m_TooltipShips;

    m_TooltipShips = "";

    if( Ships->Count )
    {

        SortedList<PrintKey^, Ship^> ^ships =
            gcnew SortedList<PrintKey^, Ship^>;
        for each( Ship ^ship in Ships )
            ships->Add(
                gcnew PrintKey(ship->Owner, ship->PlanetNum, ship->Name),
                ship);

        Alien ^player = GameData::Player;
        for each( Ship ^ship in ships->Values )
        {
            // Skip incomplete ships
            if( ship->EUToComplete > 0 )
                continue;

            String^ inv = ship->PrintCargo(true);
            m_TooltipShips += String::Format("{0}{1} {2} {3}{4}{5}\r\n",
                ship->HadMishap ? "[MISJUMPED] " : "",
                ship->Owner->Name,
                ship->PrintClassWithName(),
                ship->Type == SHIP_BAS ? String::Format("({0}k)", ship->Size / 1000) : "",
                ship->PrintAgeLocation(),
                String::IsNullOrEmpty(inv) ? "" : ": " + inv );
        }
    }

    return m_TooltipShips;
}

String^ StarSystem::GenerateScan(bool hideLSN)
{
    if( m_TooltipScan && hideLSN == false )
        return m_TooltipScan;

    String ^scan = String::Format(
        "Coordinates: x = {0}  y = {1}  z = {2}  stellar type = {3}  {4} planets.\r\n",
        X, Y, Z, Type,
        Planets->Count == 0 ? "?" : Planets->Count.ToString() );

    if( HasWormhole )
    {
        scan += "Wormhole to " +
            ( WormholeTargetId == -1
                ? "Unknown System"
                : GameData::GetStarSystem(WormholeTargetId)->PrintLocation() )
            + "\r\n";
    }

    bool first = true;
    for each(Planet^ planet in Planets->Values )
    {
        if( first )
        {
            first = false;
            scan +=
                "\r\n"
                "  #  Dia  Grav TC PC MD  LSN  Atmosphere\r\n"
                " ---------------------------------------------------------------------\r\n";
        }

        String ^plStr = String::Format(
            Globalization::CultureInfo::InvariantCulture,
            "{0,3}{1,5}{2,6:F2}{3,3}{4,3} {5,5:F2}{6,3}  ",
            planet->Number,
            planet->Diameter,
            (double)planet->Grav / 100.0,
            planet->TempClass,
            planet->PressClass,
            (double)planet->MiDiff / 100,
            (hideLSN ? 99 : planet->LSN) );
        bool anyGas = false;
        for( int gas = 0; gas < GAS_MAX; ++gas )
        {
            if( planet->Atmosphere[gas] )
            {
                plStr += String::Format(
                    "{0}{1}({2}%)",
                    anyGas ? "," : "",
                    FHStrings::GasToString(static_cast<GasType>(gas)),
                    planet->Atmosphere[gas] );
                anyGas = true;
            }
        }
        if( !anyGas )
            plStr = String::Concat(plStr, "No atmosphere");

        scan += plStr + "\r\n";
    }

    if( hideLSN == false )
        m_TooltipScan = scan;
    return scan;
}

String^ StarSystem::PrintWormholeTarget()
{
    if( HasWormhole )
    {
        if( WormholeTargetId != -1 )
            return GetWormholeTarget()->PrintLocation();
        return "Unknown";
    }
    return nullptr;
}

String^ StarSystem::PrintScanStatus()
{
    if( TurnScanned == -1 )
        return s_ScanNone;

    if( TurnScanned == 0 ||
        LastVisited == -1 ||
        (LastVisited > 0 && TurnScanned > LastVisited) )
    {
        if( IsMarkedVisited )
            return s_ScanDiplVisited;
        return s_ScanDipl;
    }

    return s_ScanSelf;
}

String^ StarSystem::PrintColoniesAll()
{
    String ^ret = "";
    Alien ^player = GameData::Player;

    // Player's colonies first
    if( ColoniesOwned->Count == 1 )
    {
        Colony ^colony = ColoniesOwned[0];
        ret += String::Format(", PL {0} [{1}]",
            colony->Name, colony->PrintSize() );
    }
    else if( ColoniesOwned->Count > 1 )
    {
        SortedList<int, Colony^> ^colonies = gcnew SortedList<int, Colony^>;
        for each( Colony ^colony in ColoniesOwned )
            colonies->Add(colony->PlanetNum, colony);

        ret += String::Format(", {0} [", player->Name);
        bool firstColony = true;
        for each( Colony ^colony in colonies->Values )
        {
            ret += String::Format("{0}{1}{2}:{3}",
                firstColony ? "" : " ",
                colony->PlanetNum,
                colony->PlanetType == PLANET_HOME ? "(HOME)" : "",
                colony->PrintSize() );

            firstColony = false;
        }
        ret += "]";
    }
    else if( ColoniesOwned->Count == 0 )
    {   // No colonies owned, print named planet if any...
        for each( Planet ^planet in Planets->Values )
            if( !String::IsNullOrEmpty(planet->Name) )
                ret += ", PL " + planet->Name;
    }

    // Alien colonies
    if( ColoniesAlien->Count > 0 )
    {
        SortedList<PrintKey^, Colony^> ^colonies =
            gcnew SortedList<PrintKey^, Colony^>;
        for each( Colony ^colony in ColoniesAlien )
            colonies->Add(
                gcnew PrintKey(colony->Owner, colony->PlanetNum, colony->Name),
                colony);

        Alien ^lastAlien = nullptr;
        bool firstColony = true;
        for each( Colony ^colony in colonies->Values )
        {
            if( colony->Owner != lastAlien )
            {
                ret += String::Format("{0}, {1} [",
                    lastAlien != nullptr ? "]" : "",
                    colony->Owner->Name);
                lastAlien = colony->Owner;
                firstColony = true;
            }

            ret += String::Format("{0}{1}{2}:{3}",
                firstColony ? "" : " ",
                colony->PlanetType == PLANET_HOME ? "(HOME)" : "",
                colony->PlanetNum,
                colony->PrintSize() );

            firstColony = false;
        }
        ret += "]";
    }

    if( ret->Length > 0 && ret[0] == ',' )
        return ret->Substring(2);
    return ret;
}

String^ StarSystem::PrintColonies(int planetNum)
{
    String ^ret = "";
    Alien ^player = GameData::Player;
    Alien ^homeSpecies = nullptr;
    bool homeInhabited = false;

    // Check if this is a home system for someone
    for each( Alien ^alien in GameData::GetAliens() )
    {
        if( alien->HomeSystem == this &&
            alien->HomePlanet == planetNum )
        {
            homeSpecies = alien;
            break;
        }
    }

    // Player's colony first
    if( ColoniesOwned->Count > 0 )
    {
        for each( Colony ^colony in ColoniesOwned )
        {
            if( colony->PlanetNum != planetNum )
                continue;

            ret += String::Format(", {0}PL {1} [{2}]",
                colony->PlanetType == PLANET_HOME ? "(HOME)" : "",
                colony->Name,
                colony->PrintSize() );

            if( colony->PlanetType == PLANET_HOME )
                homeInhabited = true;
        }
    }

    // Alien colonies
    if( ColoniesAlien->Count > 0 )
    {
        for each( Colony ^colony in ColoniesAlien )
        {
            if( colony->PlanetNum != planetNum )
                continue;

            ret += String::Format(", {0}{1} [{2}]",
                colony->Owner->Name,
                colony->PlanetType == PLANET_HOME ? "(HOME)" : "",
                colony->PrintSize() );

            if( colony->PlanetType == PLANET_HOME )
                homeInhabited = true;
        }
    }

    // Check if home is inhabited, if not add special comment
    if( homeSpecies && !homeInhabited )
    {
        ret += ", Home of SP " + homeSpecies->Name;
    }

    if( ret->Length > 0 && ret[0] == ',' )
        return ret->Substring(2);
    return ret;
}

String^ StarSystem::PrintComment()
{
    String ^ret = nullptr;
    if( Comment )
        ret = Comment;
    if( CommentHome )
    {
        if( ret )
            ret = CommentHome + "; " + ret;
        else
            ret = CommentHome;
    }

    return ret;
}

List<String^>^ StarSystem::PrintAliens()
{
    // For each species, generates line "SP Name: PL #n(size), ..., Class(Ax,loc), ..."

    List<Alien^>^ species = gcnew List<Alien^>;

    for each (Ship^ ship in m_ShipsAlien)
    {
        if (species->Contains(ship->Owner))
            continue;
        species->Add(ship->Owner);
    }
    for each (Colony^ colony in ColoniesAlien)
    {
        if (species->Contains(colony->Owner))
            continue;
        species->Add(colony->Owner);
    }
    
    List<String^>^ results = gcnew List<String^>;

    for each (Alien^ race in species)
    {
        String^ info = "";
        for each (Colony^ colony in ColoniesAlien)
        {
            if (colony->Owner == race)
            {
                info += String::Format("PL #{0}({1}) ", colony->PlanetNum, colony->EconomicBase / 10);
            }
        }

        for each (Ship^ ship in m_ShipsAlien)
        {
            if (ship->Owner == race)
            {
                info += String::Format("{0}{1}(A{2},{3}) ",
                    ship->PrintClass(),
                    (ship->Type == SHIP_BAS ? String::Format("({0}k)", ship->Size / 1000) : ""),
                    ship->Age,
                    ship->PrintLocationShort());
            }
        }

        results->Add(String::Format("SP {0}: {1}", race->Name, info));
    }
    return results;
}

int StarSystem::GetId()
{
    return GameData::GetSystemId(X, Y, Z); 
}

void StarSystem::UpdateMaster()
{
    for each( Colony ^colony in m_Colonies )
    {
        if( Master == nullptr )
        {
            Master = colony->Owner;
        }
        else if( Master != colony->Owner &&
                 Master->Relation != SP_MIXED )
        {
            Master = gcnew Alien("");
            Master->Relation = SP_MIXED;
        }

        if( Planets->ContainsKey(colony->PlanetNum) )
        {
            Planet ^planet = Planets[colony->PlanetNum];
            if( planet->Master == nullptr )
            {
                planet->Master = colony->Owner;
            }
            else if( planet->Master != colony->Owner &&
                     planet->Master->Relation != SP_MIXED )
            {
                planet->Master = Master;
            }
        }
    }
}

void StarSystem::SetWormhole(int targetId)
{
    HasWormhole = true;

    if( WormholeTargetId == -1 )
    {
        WormholeTargetId = targetId;
    }
    else if( WormholeTargetId != targetId )
        throw gcnew FHUIDataIntegrityException("System " + PrintLocation() +
            " is already marked as wormhole connected with " + GetWormholeTarget()->PrintLocation());

    if( targetId != -1 )
    {
        StarSystem ^target = GameData::GetStarSystem(targetId);
        if( target->HasWormhole == false ||
            target->WormholeTargetId != this->GetId() )
        {
            target->SetWormhole( this->GetId() );
        }

        // Generate random color for wormhole
        if( WormholeColor != System::Drawing::Color::White )
            return;

        Random ^rnd = gcnew Random;
        bool r,g,b;
        int failedColors = 0;
        while( true )
        {
            do
            {
                r = rnd->Next(0, 3) == 0;
                g = rnd->Next(0, 3) == 0;
                b = rnd->Next(0, 3) == 0;
            } while( !r && !g && !b );
            WormholeColor = System::Drawing::Color::FromArgb(
                0xff,
                r ? rnd->Next(0x50, 0xd0) : 0xff,
                g ? rnd->Next(0x50, 0xd0) : 0xff,
                b ? rnd->Next(0x50, 0xd0) : 0xff );

            // Check if wormhole color is unique enough
            if( s_WHColorsUsed && failedColors < 100 )
            {
                bool fail = false;
                for each( System::Drawing::Color color in s_WHColorsUsed )
                {
                    int diff =
                        Math::Abs(color.R - WormholeColor.R) +
                        Math::Abs(color.G - WormholeColor.G) +
                        Math::Abs(color.B - WormholeColor.B);
                    if( diff < 80 )
                    {
                        fail = true;
                        ++failedColors;
                        break;
                    }
                }
                if( fail )
                    continue;
            }
            break;
        }

        target->WormholeColor = WormholeColor;

        if( s_WHColorsUsed == nullptr )
            s_WHColorsUsed = gcnew List<System::Drawing::Color>;
        s_WHColorsUsed->Add( WormholeColor );
    }
}

List<Ship^>^ StarSystem::GetShipTargets(CommandPhase phase)
{
    List<Ship^> ^list = gcnew List<Ship^>;

    for each( Ship ^ship in GameData::Player->Ships )
    {
        switch( phase )
        {
        case CommandPhase::PreDeparture:
            if( ship->System != this )
                continue;
            // In pre-departure skip any incomplete or being built ship
            if( ship->EUToComplete > 0 || ship->BuiltThisTurn )
                continue;
            break;

        case CommandPhase::Production:
        case CommandPhase::PostArrival:
            {
                ICommand ^cmd = ship->GetJumpCommand();
                if( ship->System != this )
                {
                    if( cmd == nullptr ||
                        cmd->GetRefSystem() != this )
                        continue;
                }
                else if( cmd )
                    continue;
            }
            // Skip incomplete ships
            if( ship->EUToComplete > 0 )
                continue;
            break;

        default:
            throw gcnew FHUIDataImplException("Unsupported command phase: " + ((int)phase).ToString());
        }

        list->Add(ship);
    }

    return list;
}

List<ICommand^>^ StarSystem::GetTransfers(Colony ^colony)
{
    List<ICommand^> ^ret = gcnew List<ICommand^>;

    for each( ICommand ^cmd in Transfers )
    {
        CmdTransfer ^cmdTr = safe_cast<CmdTransfer^>(cmd);
        if( cmdTr->m_FromColony == colony ||
            cmdTr->m_ToColony == colony )
        {
            ret->Add( cmd );
        }
    }

    return ret;
}

List<ICommand^>^ StarSystem::GetTransfers(Ship ^ship)
{
    List<ICommand^> ^ret = gcnew List<ICommand^>;

    for each( ICommand ^cmd in Transfers )
    {
        CmdTransfer ^cmdTr = safe_cast<CmdTransfer^>(cmd);
        if( cmdTr->m_FromShip == ship ||
            cmdTr->m_ToShip == ship )
        {
            ret->Add( cmd );
        }
    }

    return ret;
}

bool StarSystem::IsTransferPossible(CommandPhase phase, Colony ^colony, Ship ^ship)
{
    int numColonies = ColoniesOwned->Count;
    int numShips = 0;

    if( colony )
        --numColonies;

    if( phase == CommandPhase::PreDeparture )
    {
        numShips = ShipsOwned->Count;
        if( ship )
            --numShips;
    }
    else
    {
        for each( Ship ^iShip in GameData::Player->Ships )
        {
            if( ship != iShip && iShip->GetPostArrivalSystem() == this )
                ++numShips;
        }
    }

    return (numColonies + numShips) > 0;
}

} // end namespace FHUI
