#include "StdAfx.h"
#include "GameData.h"
#include "Alien.h"

namespace FHUI
{

int Planet::CalculateLSN()
{
    AtmosphericReq ^atm = GameData::Player->AtmReq;

    int pc = PressClass == -1 ? 99 : PressClass;
    int tc = TempClass == -1 ? 99 : TempClass;

    int lsn =
        3 * Math::Abs(pc - atm->PressClass) +
        3 * Math::Abs(tc - atm->TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( Atmosphere[i] > 0 && atm->Poisonous[i] )
            lsn += 3;
    }

    if( Atmosphere[atm->GasRequired] < atm->ReqMin ||
        Atmosphere[atm->GasRequired] > atm->ReqMax )
    {
        lsn += 3;
    }

    lsn = Math::Min(lsn, 99); 

    return lsn;
}

String^ Planet::GetNameWithOrders()
{
    String ^n = Name;
    if( NameIsNew )
        n += " (new)";
    else if( NameIsDisband )
        n += " (disband)";

    return n;
}

void Planet::AddName(String ^name)
{
    Name = name;
    NameIsNew = true;
}

void Planet::DelName()
{
    if( NameIsNew )
    {
        Name = nullptr;
        NameIsNew = false;
    }
    else
        NameIsDisband = true;
}

String^ Planet::PrintLocation()
{
    return String::Format("{0} {1}", System->PrintLocation(), Number);
}

String^ Planet::PrintLocationAligned()
{
    return String::Format("{0} {1}", System->PrintLocationAligned(), Number);
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

} // end namespace FHUI
