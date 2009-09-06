#include "StdAfx.h"
#include "GameData.h"

namespace FHUI
{

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

String^ StarSystem::GenerateScan()
{
    String ^scan = String::Format(
        "Coordinates: x = {0}  y = {1}  z = {2}  stellar type = {3}  {4} planets.\r\n\r\n"
        "               Temp  Press Mining\r\n"
        "  #  Dia  Grav Class Class  Diff  LSN  Atmosphere\r\n"
        " ---------------------------------------------------------------------\r\n",
        X, Y, Z, Type,
        Planets->Count == 0 ? "?" : Planets->Count.ToString() );

    for each(Planet^ planet in Planets->Values )
    {
        String ^plStr = String::Format(
            "{0,3}{1,5}{2,7:F2}{3,4}{4,6}{5,8:F2}{6,5}  ",
            planet->Number,
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

String^ StarSystem::PrintWormholeTarget()
{
    if( HasWormhole )
    {
        if( WormholeTargetId != -1 )
            return GameData::GetStarSystem(WormholeTargetId)->PrintLocation();
        return "Unknown System";
    }
    return nullptr;
}

String^ StarSystem::PrintScanStatus()
{
    if( TurnScanned == -1 )     return s_ScanNone;
    else if( TurnScanned == 0 ) return s_ScanDipl;
    else                        return s_ScanSelf;
}

private ref class ColoniesPrintKey : public IComparable
{
public:
    ColoniesPrintKey(Alien ^sp, int plNum)
        : m_Sp(sp), m_PlNum(plNum)
    {
    }

    virtual int CompareTo(Object ^o)
    {
        ColoniesPrintKey ^key = (ColoniesPrintKey^)o;

        int n = String::Compare(m_Sp->Name, key->m_Sp->Name);
        if( n == 0 )
            return m_PlNum - key->m_PlNum;
        return n;
    }

    Alien^  m_Sp;
    int     m_PlNum;
};

String^ StarSystem::PrintColoniesAll()
{
    String ^ret = "";
    Alien ^player = GameData::Player;
    Alien ^homeSpecies = nullptr;
    bool homeInhabited = false;

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
                colony->PlanetType == PLANET_HOME ? "(HOME)" : "",
                colony->PlanetNum,
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
        SortedList<ColoniesPrintKey^, Colony^> ^colonies =
            gcnew SortedList<ColoniesPrintKey^, Colony^>;
        for each( Colony ^colony in ColoniesAlien )
            colonies->Add(
                gcnew ColoniesPrintKey(colony->Owner, colony->PlanetNum),
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
        String^ info;
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
            Master = gcnew Alien("", 0);
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

} // end namespace FHUI
