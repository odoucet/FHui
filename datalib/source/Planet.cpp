#include "StdAfx.h"
#include "GameData.h"

namespace FHUI
{

int Planet::CalculateLSN()
{
    int lsn =
        3 * Math::Abs(PressClass - GameData::AtmReq->PressClass) +
        3 * Math::Abs(TempClass - GameData::AtmReq->TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( Atmosphere[i] > 0 && GameData::AtmReq->Poisonous[i] )
            lsn += 3;
    }

    if( Atmosphere[GameData::AtmReq->GasRequired] < GameData::AtmReq->ReqMin ||
        Atmosphere[GameData::AtmReq->GasRequired] > GameData::AtmReq->ReqMax )
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