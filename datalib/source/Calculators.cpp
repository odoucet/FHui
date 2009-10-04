#include "StdAfx.h"
#include "Calculators.h"
#include "GameData.h"

namespace FHUI
{

double Calculators::Distance(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo)
{
    int dx = Math::Abs(xFrom - xTo);
    int dy = Math::Abs(yFrom - yTo);
    int dz = Math::Abs(zFrom - zTo);

    return Math::Sqrt(dx * dx + dy * dy + dz * dz);
}

double Calculators::Mishap(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo, int gv, int age)
{
    if( gv == 0 )
        throw gcnew FHUIDataIntegrityException("GV must not be 0 for mishap calculation!");

    double dist = Distance(xFrom, yFrom, zFrom, xTo, yTo, zTo);
    if( dist == 0 )
        return 0.0;

    return (dist * dist) / gv + (age * 2.0);
}

int Calculators::ShipTonnage(ShipType type, int size)
{
    switch( type )
    {
    case SHIP_BAS:  return size;
    case SHIP_TR:   return size * 10000;
    case SHIP_PB:   return 10000;
    case SHIP_CT:   return 20000;
    case SHIP_ES:   return 50000;
    case SHIP_FF:   return 100000;
    case SHIP_DD:   return 150000;
    case SHIP_CL:   return 200000;
    case SHIP_CS:   return 250000;
    case SHIP_CA:   return 300000;
    case SHIP_CC:   return 350000;
    case SHIP_BC:   return 400000;
    case SHIP_BS:   return 450000;
    case SHIP_DN:   return 500000;
    case SHIP_SD:   return 550000;
    case SHIP_BM:   return 600000;
    case SHIP_BW:   return 650000;
    case SHIP_BR:   return 700000;
    default:
        return 0;
    }
}

int Calculators::ShipMARequired(ShipType type, int size)
{
    switch( type )
    {
    case SHIP_BAS:  return size / 10000;
    case SHIP_TR:   return size * 2;
    case SHIP_PB:   return 2;
    case SHIP_CT:   return 4;
    case SHIP_ES:   return 10;
    case SHIP_FF:   return 20;
    case SHIP_DD:   return 30;
    case SHIP_CL:   return 40;
    case SHIP_CS:   return 50;
    case SHIP_CA:   return 60;
    case SHIP_CC:   return 70;
    case SHIP_BC:   return 80;
    case SHIP_BS:   return 90;
    case SHIP_DN:   return 100;
    case SHIP_SD:   return 110;
    case SHIP_BM:   return 120;
    case SHIP_BW:   return 130;
    case SHIP_BR:   return 140;
    default:
        return 0;
    }
}

int Calculators::ShipBuildCost(ShipType type, int size, bool sublight)
{
    int cost = ShipTonnage(type, size) / 100;
    if( sublight )
        cost = (3 * cost) / 4;
    return cost;
}

int Calculators::ShipMaintenanceCost(ShipType type, int size, bool sublight)
{
    int buildCost = ShipBuildCost(type, size, sublight);

    if( type == SHIP_BAS )
        return buildCost / 10;
    if( type == SHIP_TR )
        return buildCost / 25;
    return buildCost / 5;

}

int Calculators::ShipUpgradeCost(int age, int originalCost)
{
    return age * originalCost / 40;
}

int Calculators::ShipRecycleValue(int age, int originalCost)
{
    return (3 * originalCost * (60 - age)) / 200;
}

double Calculators::ShipMaintenanceDiscount(int mlLevel)
{
    return (100.0 - (mlLevel / 2)) / 100.0;
}

int Calculators::ResearchCost(int startLevel, int endLevel, bool guided)
{
    int sum = 0;

    for( int level = startLevel; level < endLevel; ++level )
    {
        int cost = level * level;
        if( guided )
        {
            cost = (int)Math::Ceiling(cost * 0.75);
        }
        sum += cost;
    }

    return sum;
}

int Calculators::ColonyProduction(Colony^ colony, int mi, int ma, int ls, int fleetPercentCost)
{
    int rawMaterialUnits, productionCapacity, availableToSpend;

    if (colony->MiDiff == 0)
    {
        return 0; // TODO: Fix at MiDiff parsing
    }

    rawMaterialUnits = (10 * mi * colony->MiBase) / colony->MiDiff;
    productionCapacity = (ma * colony->MaBase) / 10;

    rawMaterialUnits -= (colony->ProdPenalty * rawMaterialUnits) / 100;
    rawMaterialUnits = ((colony->EconomicEff * rawMaterialUnits) + 50) / 100;

    productionCapacity -= (colony->ProdPenalty * productionCapacity) / 100;
    productionCapacity = ((colony->EconomicEff * productionCapacity) + 50) / 100;

    if ( colony->PlanetType == PLANET_COLONY_MINING )
    {
        /* Mining colony */
        availableToSpend = (2 * rawMaterialUnits) / 3; 
    }
    else if ( colony->PlanetType == PLANET_COLONY_RESORT )
    {
        /* Resort colony */
        availableToSpend = (2 * productionCapacity) / 3; 
    }
    else
    {
        rawMaterialUnits += colony->Inventory[INV_RM];
        availableToSpend = Math::Min(rawMaterialUnits, productionCapacity);
    }
    return ( availableToSpend - ((fleetPercentCost * availableToSpend + 5000) / 10000) );
}

int Calculators::ProductionPenalty(int lsn, int ls)
{
    if( ls < lsn )
        return -1;
    if( ls == 0 )
        return 0;
    return (int)Math::Floor( (100.0 * lsn) / ls );
}

int Calculators::ShipyardCost(int maTechLevel)
{
    return maTechLevel * 10;
}

int Calculators::TransportCapacity(int size)
{
    return (10 + size / 2) * size;
}

int Calculators::ColonyHideCost(Colony ^colony)
{
    return colony->EconomicBase;
}

} // end namespace FHUI
