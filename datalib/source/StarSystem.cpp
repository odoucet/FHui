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
    return s == WormholeTarget
        ? 0
        : Calculators::Distance(X, Y, Z, s->X, s->Y, s->Z);
}

double StarSystem::CalcMishap(StarSystem ^s, int gv, int age)
{
    return s == WormholeTarget
        ? 0
        : Calculators::Mishap(X, Y, Z, s->X, s->Y, s->Z, gv, age);
}

double StarSystem::CalcDistance(int x, int y, int z)
{
    return (WormholeTarget &&
            x == WormholeTarget->X &&
            y == WormholeTarget->Y &&
            z == WormholeTarget->Z )
        ? 0
        : Calculators::Distance(X, Y, Z, x, y, z);
}

double StarSystem::CalcMishap(int x, int y, int z, int gv, int age)
{
    return (WormholeTarget &&
            x == WormholeTarget->X &&
            y == WormholeTarget->Y &&
            z == WormholeTarget->Z )
        ? 0
        : Calculators::Mishap(X, Y, Z, x, y, z, gv, age);
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

String^ StarSystem::PrintScanStatus()
{
    if( TurnScanned == -1 )     return s_ScanNone;
    else if( TurnScanned == 0 ) return s_ScanDipl;
    else                        return s_ScanSelf;
}

String^ StarSystem::PrintColonies(int planetNum, Alien ^player)
{
    String ^ret = "";

    for each( Colony ^colony in m_Colonies )
    {
        if( planetNum != -1 &&
            colony->PlanetNum != planetNum )
            continue;

        if( String::IsNullOrEmpty(ret) == false )
            ret += ", ";
    }

    for each( Colony ^colony in m_Colonies )
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

    for each (Planet^ planet in Planets->Values )
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
