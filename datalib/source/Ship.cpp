#include "StdAfx.h"
#include "GameData.h"

namespace FHUI
{

Int32 Ship::CompareTo( Object^ obj )
{
    Ship^ ship = safe_cast<Ship^>(obj);
    return Name->CompareTo( ship->Name );
}

String^ Ship::PrintClass()
{
    String^ out;
    if ( Type == SHIP_TR)
    {
        out = String::Format( "TR{0}", Size.ToString() );
    }
    else
    {
        out = FHStrings::ShipToString( Type );
    }

    if ( SubLight )
    {
        out += "s";
    }
    return out;
}

String^ Ship::PrintRefListEntry()
{
    return PrintClassWithName() + " (A" + Age.ToString() + ")";
}

String^ Ship::PrintAgeLocation()
{
    if (EUToComplete > 0)
    {
        return String::Format( "(+{0},{1})", EUToComplete, PrintLocationShort() );
    }
    else
    {
        return String::Format( "(A{0},{1})", Age, PrintLocationShort() );
    }
}


String^ Ship::PrintLocationShort()
{
    switch( Location )
    {
        case SHIP_LOC_DEEP_SPACE:
            return "D";
        case SHIP_LOC_ORBIT:
            return String::Format("O{0}", PlanetNum);
        case SHIP_LOC_LANDED:
            return String::Format("L{0}", PlanetNum);
    }
    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid ship location: {0}!", (int)Location) );
}

String^ Ship::PrintLocation()
{
    String ^location;
    String ^nearColony;

    // First see if ship is orbiting owned colony
    if( PlanetNum != -1 )
    {
        for each( Colony ^colony in System->ColoniesOwned )
        {
            if( colony->PlanetNum == PlanetNum )
            {
                location = "PL " + colony->Name;
                break;
            }
        }
    }

    if( !location )
    {
        if( System->ColoniesOwned->Count > 0 )
        {
            // Not orbiting owned colony, but there is at least one owned colony
            // in this system, so find the largest one
            int plSize = -1;
            for each( Colony ^colony in System->ColoniesOwned )
            {
                if( colony->EconomicBase > plSize )
                {
                    plSize = colony->EconomicBase;
                    nearColony = colony->Name;
                }
            }
        }

        if( System->Planets->ContainsKey(PlanetNum) )
        {
            // Still check the planet name, it may be a named planet not yet colonized
            Planet ^planet = System->Planets[PlanetNum];
            if( String::IsNullOrEmpty(planet->Name) )
            {
                if( nearColony )
                    location = String::Format("[{0}]", nearColony);
                else
                    location = String::Format("[{0} {1}]", System->PrintLocation(), PlanetNum);
            }
            else
            {
                location = "PL " + planet->Name;
            }
        }
        else
        {
            // Use <> to mark void sector
            if( System->IsVoid )
            {
                location = String::Format("<{0}>", System->PrintLocation());
            }
            else
            {
                location = String::Format("[{0}]",
                    nearColony ? nearColony : System->PrintLocation());
            }
        }
    }

    switch( Location )
    {
    case SHIP_LOC_DEEP_SPACE:
        if ( !System->IsVoid )
            location += ", Deep";
        break;
    case SHIP_LOC_ORBIT:
        location += ", Orbit";
        if( nearColony )
            location += " PL #" + PlanetNum.ToString();
        break;
    case SHIP_LOC_LANDED:
        location += ", Landed";
        if( nearColony )
            location += " PL #" + PlanetNum.ToString();
        break;

    default:
        throw gcnew FHUIDataIntegrityException(
            String::Format("Invalid ship location! ({0})", (int)Location) );
    }

    return location;
}

String^ Ship::PrintCargo()
{
    return GameData::PrintInventory(m_Cargo);
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

int Ship::GetUpgradeCost()
{
    if( EUToComplete > 0 )
        return 0;
    return Calculators::ShipUpgradeCost(Age, OriginalCost);
}

int Ship::GetRecycleValue()
{
    if( EUToComplete )
        return (OriginalCost - EUToComplete) / 2;

    return Calculators::ShipRecycleValue(Age, OriginalCost);
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
    case OrderType::Wormhole:
        return "Wormhole to " + PrintJumpDestination();
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
    if( Type == OrderType::Wormhole )
        return String::Format("Wormhole to {0} {1} {2} {3}",
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
        if( JumpTarget->Planets->ContainsKey(PlanetNum) )
        {
            Planet ^planet = JumpTarget->Planets[PlanetNum];
            if( String::IsNullOrEmpty(planet->Name) )
            {
                return planet->PrintLocation();
            }
            else
            {
                return "PL " + planet->Name;
            }
        }
        return JumpTarget->PrintLocation() + " " + PlanetNum.ToString();
    }
    return JumpTarget->PrintLocation();
}

} // end namespace FHUI
