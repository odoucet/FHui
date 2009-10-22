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
