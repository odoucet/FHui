#pragma once

using namespace System;

namespace FHUI
{
    ref class Colony;
    ref class Planet;
    ref class Alien;

public ref class Calculators
{
public:
    static double   Distance(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo);
    static double   Mishap(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo, int gv, int age);

    static int      ShipTonnage(ShipType type, int size);
    static int      ShipMARequired(ShipType type, int size);
    static int      ShipBuildCost(ShipType type, int size, bool sublight);
    static int      ShipMaintenanceCost(ShipType type, int size, bool sublight);
    static int      ShipUpgradeCost(int age, int originalCost);
    static int      ShipRecycleValue(int age, int originalCost);
    static int      InventoryBuildCost(InventoryType inv);
    static int      RecycleValue(InventoryType inv, int amount);
    static int      RecycleValuePop(InventoryType inv, int amount);
    static double   ShipMaintenanceDiscount(int mlLevel);

    static int      ResearchCost(int startLevel, int endLevel, bool guided);
    static int      ShipyardCost(int maTechLevel);
    static int      ColonyHideCost(Colony ^colony);

    static int      TransportCapacity(int size);
    static int      ColonyProduction(Colony^, int mi, int ma, int ls, int fleetPercentCost);
    static int      ProductionPenalty(int lsn, int ls);

    static int      LSN(Planet ^planet, Alien ^alien);
};

} // end namespace FHUI
