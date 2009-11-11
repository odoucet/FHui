#include "StdAfx.h"
#include "GameData.h"
#include "Alien.h"
#include "StarSystem.h"

namespace FHUI
{

String^ Planet::GetTooltipText()
{
    return System->GetTooltipText();
}

String^ Planet::GetNameWithOrders()
{
    if( String::IsNullOrEmpty(Name) )
    {
        if( String::IsNullOrEmpty(AlienName) )
            return nullptr;
        return "<" + AlienName + ">";
    }

    String ^n = Name;
    Colony ^colony = GameData::Player->FindColony(Name, false);

    if( colony->IsNew )
    {
        n += " (new)";
    }
    else if( colony->IsDisband )
    {
        n += " (disband)";
    }

    return n;
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
