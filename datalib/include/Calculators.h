#pragma once

using namespace System;

namespace FHUI
{

public ref class Calculators
{
public:
    static double   Distance(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo);
    static double   Mishap(int xFrom, int yFrom, int zFrom, int xTo, int yTo, int zTo, int gv, int age);

    static int      ShipUpgradeCost(int age, int originalCost);
    static int      ShipRecycleValue(int age, int originalCost);

    static int      ResearchCost(int startLevel, int endLevel, bool guided);

    static int      TransportCapacity(int size);
};

} // end namespace FHUI
