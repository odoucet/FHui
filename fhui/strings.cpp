#include "stdafx.h"
#include "enums.h"

String^ SpRelToString(SPRelType rel)
{
    switch( rel )
    {
    case SP_PLAYER:     return "*Player*";
    case SP_NEUTRAL:    return "Neutral";
    case SP_ALLY:       return "Ally";
    case SP_ENEMY:      return "Enemy";
    default:
        {
            int r = rel;
            throw gcnew ArgumentException(String::Format("Invalid species relation: {0}", r));
        }
    }
}

struct GasStr
{
    GasType     type;
    const char *str;
};

static GasStr s_GasStrings[GAS_MAX] =
{
    { GAS_O2,   "O2" },
    { GAS_CH4,  "CH4" },
    { GAS_H2,   "H2" },
    { GAS_HE,   "He" },
    { GAS_NH3,  "NH3" },
    { GAS_N2,   "N2" },
    { GAS_CL2,  "Cl2" },
    { GAS_H2O,  "H2O" },
    { GAS_CO2,  "CO2" },
    { GAS_HCL,  "HCl" },
    { GAS_F2,   "F2" },
    { GAS_SO2,  "SO2" },
    { GAS_H2S,  "H2S" },
};

String^ GasToString(GasType gas)
{
    if( gas == GAS_MAX )
        return "???";
    for( int i = 0; i < GAS_MAX; ++i )
        if( s_GasStrings[i].type == gas )
            return gcnew String(s_GasStrings[i].str);
    throw gcnew ArgumentException("Invalid atmospheric GAS.");
}

GasType GasFromString(String^ gas)
{
    for( int i = 0; i < GAS_MAX; ++i )
        if( String::Compare(
            (gcnew String(s_GasStrings[i].str))->ToLower(),
            gas->ToLower()) == 0 )
        {
            return s_GasStrings[i].type;
        }
    throw gcnew ArgumentException(
        String::Format("Invalid atmospheric GAS: {0}", gas) );
}

String^ PlTypeToString(PlanetType plType)
{
    switch( plType )
    {
    case PLANET_HOME:           return "HOME";
    case PLANET_COLONY:         return "Colony";
    case PLANET_COLONY_MINING:  return "Mining";
    case PLANET_COLONY_RESORT:  return "Resort";
    default:
        {
            int t = plType;
            throw gcnew ArgumentException(String::Format("Invalid planet type: {0}", t));
        }
    }
}


String^ InvToString(InventoryType inv)
{
#define INV_TO_STRING(i) case INV_##i: return #i
    switch( inv )
    {
    INV_TO_STRING(CU);
    INV_TO_STRING(IU);
    INV_TO_STRING(AU);
    INV_TO_STRING(PD);
    INV_TO_STRING(SU);
    INV_TO_STRING(FD);
    INV_TO_STRING(FS);
    INV_TO_STRING(DR);
    INV_TO_STRING(FM);
    INV_TO_STRING(FJ);
    INV_TO_STRING(GW);
    INV_TO_STRING(GT);
    INV_TO_STRING(JP);
    INV_TO_STRING(TP);
    INV_TO_STRING(GU1);
    INV_TO_STRING(GU2);
    INV_TO_STRING(GU3);
    INV_TO_STRING(GU4);
    INV_TO_STRING(GU5);
    INV_TO_STRING(GU6);
    INV_TO_STRING(GU7);
    INV_TO_STRING(GU8);
    INV_TO_STRING(GU9);
    INV_TO_STRING(SG1);
    INV_TO_STRING(SG2);
    INV_TO_STRING(SG3);
    INV_TO_STRING(SG4);
    INV_TO_STRING(SG5);
    INV_TO_STRING(SG6);
    INV_TO_STRING(SG7);
    INV_TO_STRING(SG8);
    INV_TO_STRING(SG9);
    }
#undef INV_TO_STRING

    int i = inv;
    throw gcnew ArgumentException(String::Format("Invalid inventory type: {0}", i));
}
