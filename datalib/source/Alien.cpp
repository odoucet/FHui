#include "StdAfx.h"
#include "GameData.h"

namespace FHUI
{

void Alien::SortColoniesByProdOrder()
{
    Colonies->Sort( gcnew Colony::ProdOrderComparer );
}

String^ Alien::PrintHome()
{
    if( HomeSystem == nullptr )
        return "???";
    String ^plName = nullptr;
    if( HomeSystem->Planets->ContainsKey(HomePlanet) )
    {
        String ^n = HomeSystem->Planets[HomePlanet]->Name;
        if( !String::IsNullOrEmpty(n) )
            plName = String::Format("PL {0}", n);
    }

    String ^ret = String::Format( "{0} {1}",
        HomeSystem->PrintLocation(), HomePlanet );
    if( plName )
        ret += " (" + plName + ")";
    return ret;

}

String^ Alien::PrintTechLevels()
{
    if( TechEstimateTurn == -1 )
        return "No estimates.";

    return String::Format(
        "Turn {6}: MI:{0} MA:{1} ML:{2} GV:{3} LS:{4} BI:{5}",
        TechLevels[TECH_MI],
        TechLevels[TECH_MA],
        TechLevels[TECH_ML],
        TechLevels[TECH_GV],
        TechLevels[TECH_LS],
        TechLevels[TECH_BI],
        TechEstimateTurn );
}

String^ Alien::PrintRelation()
{
    switch( Relation )
    {
    case SP_PLAYER:     return "*Player*";
    case SP_NEUTRAL:    return "Neutral";
    case SP_ALLY:       return "Ally";
    case SP_ENEMY:      return "Enemy";
    case SP_PIRATE:     return "Pirate";
    default:
        throw gcnew FHUIDataIntegrityException(String::Format("Invalid species relation: {0}", (int)Relation));
    }
}

} // end namespace FHUI
