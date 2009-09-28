#include "StdAfx.h"
#include "GameData.h"

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
    Res->AvailCU = AvailPop;
    Res->Inventory = gcnew array<int>(INV_MAX){0};
    for( int i = 0; i < INV_MAX; ++i )
        Res->Inventory[i] = Inventory[i];
}

} // end namespace FHUI
