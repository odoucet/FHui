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

int Calculators::InventoryBuildCost(InventoryType inv)
{
    switch( inv )
    {
    case INV_CU:
    case INV_IU:    
    case INV_AU:
    case INV_PD:    return 1;
    case INV_SU:    return 110;
    case INV_FD:    return 50;
    case INV_FS:    return 25;
    case INV_DR:    return 50;
    case INV_FM:    return 100;
    case INV_FJ:    return 125;
    case INV_GW:    return 1000;
    case INV_GT:    return 500;
    case INV_JP:    return 100;
    case INV_TP:    return 50000 / GameData::Player->TechLevels[TECH_BI];
    case INV_GU1:
    case INV_GU2:   
    case INV_GU3:
    case INV_GU4:
    case INV_GU5:
    case INV_GU6:
    case INV_GU7:
    case INV_GU8:
    case INV_GU9:   return 250 * (1 + ((int)inv - (int)INV_GU1));
    case INV_SG1:
    case INV_SG2:
    case INV_SG3:
    case INV_SG4:
    case INV_SG5:
    case INV_SG6:
    case INV_SG7:
    case INV_SG8:
    case INV_SG9:   return 250 * (1 + ((int)inv - (int)INV_SG1));
    default:
        throw gcnew FHUIDataImplException("Invalid inventory for build value calculation: " + ((int)inv).ToString());
    }
}

int Calculators::RecycleValue(InventoryType inv, int amount)
{
    switch( inv )
    {
    case INV_RM:    return amount / 5;
    case INV_IU:
    case INV_AU:    return 0;   // can't recycle iu/au
    default:
        return (amount * InventoryBuildCost(inv)) / 2;
    }
}

int Calculators::RecycleValuePop(InventoryType inv, int amount)
{
    if( inv == INV_CU || inv == INV_PD )
        return amount;
    return 0;
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


int Calculators::LSN(Planet ^planet, Alien ^alien)
{
    AtmosphericReq ^atm = alien->AtmReq;

    int pc = planet->PressClass == -1 ? 99 : planet->PressClass;
    int tc = planet->TempClass == -1 ? 99 : planet->TempClass;

    int lsn =
        3 * Math::Abs(pc - atm->PressClass) +
        3 * Math::Abs(tc - atm->TempClass);

    for( int i = 0; i < GAS_MAX; ++i )
    {
        if( planet->Atmosphere[i] > 0 && atm->Poisonous[i] )
            lsn += 3;
    }

    if( planet->Atmosphere[atm->GasRequired] < atm->ReqMin ||
        planet->Atmosphere[atm->GasRequired] > atm->ReqMax )
    {
        lsn += 3;
    }

    return Math::Min(lsn, 99); 
}

} // end namespace FHUI
