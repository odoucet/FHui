#include "StdAfx.h"
#include "Calculators.h"

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

int Calculators::ShipUpgradeCost(int age, int originalCost)
{
    return age * originalCost / 40;
}

int Calculators::ShipRecycleValue(int age, int originalCost)
{
    //int only: return (((300 * originalCost) / 4) * ((6000 - (100 * age)) / 50)) / 10000;
    return (int)Math::Floor(((3 * originalCost) / 4.0) * ((60 - age) / 50.0)); 
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

int Calculators::ShipyardCost(int maTechLevel)
{
    return maTechLevel * 10;
}

int Calculators::TransportCapacity(int size)
{
    return (10 + size / 2) * size;
}

} // end namespace FHUI
