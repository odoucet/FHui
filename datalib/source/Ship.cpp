#include "StdAfx.h"
#include "GameData.h"
#include "Commands.h"

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

Ship^ Ship::FindRefListEntry(String ^entry)
{
    int space = entry->IndexOf(' ');
    int age = entry->IndexOf(" (A");

    if( space == -1 || age == -1 )
        throw gcnew FHUIDataIntegrityException("Invalid ship ref entry: " + entry);

    return GameData::GetShip( entry->Substring(space + 1, age - (space + 1)) );
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
    String ^location = nullptr;
    String ^nearColony = nullptr;

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

        if( PlanetNum != -1 )
        {
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
                location = String::Format("[{0} {1}]", System->PrintLocation(), PlanetNum);
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

String^ Ship::PrintCargo(bool original)
{
    return GameData::PrintInventory(original ? CargoOriginal : Cargo);
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

void Ship::StoreOriginalCargo()
{
    if( Owner == GameData::Player )
    {   // Make a copy
        CargoOriginal->CopyTo(Cargo, 0);
    }
    else
    {   // Just link
        Cargo = CargoOriginal;
    }
}

void Ship::SetupTonnage()
{
    Tonnage = Calculators::ShipTonnage(Type, Size);
    OriginalCost = Calculators::ShipBuildCost(Type, Size, SubLight);

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

String^ Ship::PrintJumpDestination()
{
    ICommand ^cmdJump = GetJumpCommand();
    if( cmdJump == nullptr )
        return nullptr;

    StarSystem ^jumpTarget = nullptr;
    int planetNum = -1;

    switch( cmdJump->GetCmdType() )
    {
    case CommandType::Jump:
        {
            ShipCmdJump ^cmd = safe_cast<ShipCmdJump^>(cmdJump);
            jumpTarget = cmd->m_JumpTarget;
            planetNum = cmd->m_PlanetNum;
            if( jumpTarget == nullptr && planetNum == -1 )
                return "???";
        }
        break;

    case CommandType::Move:
        jumpTarget = cmdJump->GetRefSystem();
        break;

    case CommandType::Wormhole:
        {
            ShipCmdWormhole ^cmd = safe_cast<ShipCmdWormhole^>(cmdJump);
            jumpTarget = cmd->m_JumpTarget;
            planetNum = cmd->m_PlanetNum;
        }
        break;

    default:
        return nullptr;
    }

    if( planetNum != -1 )
    {
        if( jumpTarget->Planets->ContainsKey(planetNum) )
        {
            Planet ^planet = jumpTarget->Planets[planetNum];
            if( String::IsNullOrEmpty(planet->Name) )
            {
                return planet->PrintLocation();
            }
            else
            {
                return "PL " + planet->Name;
            }
        }
        return jumpTarget->PrintLocation() + " " + planetNum.ToString();
    }
    if( jumpTarget )
        return jumpTarget->PrintLocation();
    return "Unknown System";
}

ICommand^ Ship::GetJumpCommand()
{
    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == CommandPhase::Jump )
            return cmd;
    }
    return nullptr;
}

ICommand^ Ship::GetProdCommand()
{
    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == CommandPhase::Production )
            return cmd;
    }
    return nullptr;
}

String^ Ship::PrintCmdSummary()
{
    int cnt = 0;
    ICommand ^lastCmd = nullptr;

    for each( ICommand ^cmd in System->GetTransfers(this) )
    {
        if( cmd->GetPhase() != CommandPhase::PreDeparture )
            continue;
        ++cnt;
        lastCmd = cmd;
    }
    if( GetPostArrivalSystem() )
    {
        for each( ICommand ^cmd in GetPostArrivalSystem()->GetTransfers(this) )
        {
            if( cmd->GetPhase() != CommandPhase::PostArrival )
                continue;
            ++cnt;
            lastCmd = cmd;
        }
    }

    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == CommandPhase::Jump )
            continue;
        ++cnt;
        lastCmd = cmd;
    }

    if( cnt == 0 )
        return nullptr;
    else if( cnt == 1 )
        return lastCmd->PrintForUI();
    else
        return String::Format("{0} commands", cnt);
}

String^ Ship::PrintCmdDetails()
{
    String ^ret = "";
    String ^cmds;

    ret += "PRE-DEPARTURE:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::PreDeparture);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n\r\n" : cmds + "\r\n";

    ret += "JUMPS:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::Jump);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n\r\n" : cmds + "\r\n";

    ret += "PRODUCTION:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::Production);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n\r\n" : cmds + "\r\n";

    ret += "POST-ARRIVAL:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::PostArrival);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n" : cmds;

    return ret;
}

String^ Ship::PrintCmdDetailsPhase(CommandPhase phase)
{
    String ^ret = "";

    if( phase == CommandPhase::PreDeparture ||
        phase == CommandPhase::PostArrival )
    {
        StarSystem ^system = System;
        if( phase == CommandPhase::PostArrival )
            system = GetPostArrivalSystem();
        if( system )
        {
            for each( ICommand ^cmd in system->GetTransfers(this) )
            {
                if( cmd->GetPhase() == phase )
                    ret += cmd->Print() + "\r\n";
            }
        }
    }

    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == phase )
            ret += cmd->PrintForUI() + "\r\n";
    }

    return ret;
}

void Ship::AddCommand(ICommand ^cmd)
{
    if( cmd->GetCmdType() == CommandType::Transfer )
    {
        System->Transfers->Add( cmd );
        return;
    }

    if( cmd->GetPhase() == CommandPhase::Jump )
    {   // If changing jump destination,
        // remove all transfers schduled in post-arrival
        StarSystem ^dstOld = GetPostArrivalSystem();
        StarSystem ^dstNew = cmd->GetRefSystem();

        if( dstOld != dstNew )
        {
            for each( ICommand ^cmd in dstOld->GetTransfers(this) )
                dstOld->Transfers->Remove(cmd);
        }
    }

    if( cmd->GetPhase() == CommandPhase::Jump ||
        cmd->GetPhase() == CommandPhase::Production )
    {   // Remove old one
        for each( ICommand ^cmdIter in Commands )
        {
            if( cmdIter->GetPhase() == cmd->GetPhase() )
            {
                Commands->Remove( cmdIter );
                break;
            }
        }
    }

    // Only single scan per phase
    if( cmd->GetCmdType() == CommandType::Scan )
    {   // Remove old one
        for each( ICommand ^cmdIter in Commands )
        {
            if( cmdIter->GetPhase() == cmd->GetPhase() &&
                cmdIter->GetCmdType() == cmd->GetCmdType() )
            {
                Commands->Remove( cmdIter );
                break;
            }
        }
    }

    Commands->Add(cmd);
}

void Ship::DelCommand(ICommand ^cmd)
{
    if( cmd->GetPhase() == CommandPhase::Jump )
    {   // Removing jump command
        // remove all transfers schduled in post-arrival
        StarSystem ^system = cmd->GetRefSystem();
        if( system )
        {
            for each( ICommand ^cmd in system->GetTransfers(this) )
                system->Transfers->Remove(cmd);
        }
    }

    Commands->Remove(cmd);
}

StarSystem^ Ship::GetPostArrivalSystem()
{
    ICommand ^jumpCmd = GetJumpCommand();
    if( jumpCmd )
        return jumpCmd->GetRefSystem();
    return System;
}

} // end namespace FHUI
