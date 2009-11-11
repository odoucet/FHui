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

    String ^plName = "";
    for each( Colony ^colony in HomeSystem->Colonies )
    {
        if( colony->Owner == this &&
            colony->PlanetType == PLANET_HOME )
        {
            plName = String::Format(" {0} (PL {1})", colony->PlanetNum, colony->Name);
            break;
        }
    }

    return HomeSystem->PrintLocation() + plName;
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

Colony^ Alien::FindColony(String ^name, bool allowMissing)
{
    String ^nameLower = name->ToLower();
    for each( Colony ^colony in Colonies )
        if( colony->Name->ToLower() == nameLower )
            return colony;

    if( allowMissing )
        return nullptr;

    throw gcnew FHUIDataIntegrityException("Player's colony not found when required: " + name);
}

Ship^ Alien::FindShip(String ^name, bool allowMissing)
{
    String ^nameLower = name->ToLower();
    for each( Ship ^ship in Ships )
        if( ship->Name->ToLower() == nameLower )
            return ship;

    if( allowMissing )
        return nullptr;

    throw gcnew FHUIDataIntegrityException("Player's ship not found when required: " + name);
}

} // end namespace FHUI
