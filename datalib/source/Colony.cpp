#include "StdAfx.h"
#include "GameData.h"
#include "CommandManager.h"

namespace FHUI
{

Int32 Colony::CompareTo( Object^ obj )
{
    Colony^ colony = safe_cast<Colony^>(obj);
    return Name->CompareTo( colony->Name );
}

String^ Colony::PrintInventory()
{
    return GameData::PrintInventory(Inventory);
}

String^ Colony::PrintRefListEntry()
{
    return Name + (GameData::Player != Owner ? (" (" + Owner->Name + ")") : "");
}

String^ Colony::PrintBalance()
{
    if( EconomicBase <= 0 )
        return "Empty";

    if( NeedAU > 0 )
        return String::Format("+{0} AU", NeedAU);
    else if( NeedIU )
        return String::Format("+{0} IU", NeedIU);
    else
        return "Balanced";
}

String^ Colony::PrintSize()
{
    if( EconomicBase == -1 )
    {
        return "?";
    }
    else if ( EconomicBase % 10 )
    {
        return String::Format("{0}.{1}", EconomicBase / 10, EconomicBase % 10);
    }
    else
    {
        return (EconomicBase / 10).ToString();
    }
}

void Colony::CalculateBalance(bool MiMaBalanced)
{
    if( EconomicBase <= 0 ||
        MiDiff == 0 )   // can happen on destroyed home planet
    {
        NeedIU = 0;
        NeedAU = 0;
        return;
    }

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

void Colony::ProductionReset()
{
    Res = gcnew Resources;
    OrdersText = gcnew List<String^>;

    Res->TotalEU = 0;
    Res->AvailEU = 0;
    Res->AvailPop = AvailPop;
    Res->Inventory = gcnew array<int>(INV_MAX){0};
    for( int i = 0; i < INV_MAX; ++i )
        Res->Inventory[i] = Inventory[i];
}

bool Colony::IsDisbandCommandPending()
{
    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetCmdType() == CommandType::Disband )
        {
            return true;
        }
    }
    return false;
}

bool Colony::IsNameCommandPending()
{
    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetCmdType() == CommandType::Name )
        {
            return true;
        }
    }
    return false;
}

String^ Colony::PrintCmdDetails(CommandManager ^cmdMgr, int %prodSum)
{
    String ^ret = "";
    String ^cmds;

    ret += "PRE-DEPARTURE:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::PreDeparture);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n\r\n" : cmds + "\r\n";

    ret += "PRODUCTION:\r\n";
    cmds = PrintCmdDetailsPhaseProduction(cmdMgr, prodSum);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n\r\n" : cmds + "\r\n";

    ret += "POST-ARRIVAL:\r\n";
    cmds = PrintCmdDetailsPhase(CommandPhase::PostArrival);
    ret += String::IsNullOrEmpty( cmds ) ? "<none>\r\n" : cmds;

    return ret;
}

String^ Colony::PrintCmdDetailsPhase(CommandPhase phase)
{
    String ^ret = "";

    if( phase == CommandPhase::PreDeparture ||
        phase == CommandPhase::PostArrival )
    {
        for each( ICommand ^cmd in System->GetTransfers(this) )
        {
            if( cmd->GetPhase() == phase )
                ret += cmd->Print() + "\r\n";
        }
    }

    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == phase )
            ret += cmd->Print() + "\r\n";
    }

    return ret;
}

String^ Colony::PrintCmdDetailsPhaseProduction(CommandManager ^cmdMgr, int %prodSum)
{
    String ^ret = "";
    prodSum = 0;

    for each( ICommand ^cmd in Commands )
    {
        if( cmd->GetPhase() == CommandPhase::Production )
        {
            ret += cmdMgr->PrintCommandWithInfo(cmd, 0) + "\r\n";
            prodSum += cmd->GetEUCost();
        }
    }
    if( Res->AvailEU < 0 )
        ret += "!!! BUDGET EXCEEDED !!!\r\n";

    return ret;
}

} // end namespace FHUI
