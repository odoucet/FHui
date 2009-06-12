#include "stdafx.h"
#include "enums.h"

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
