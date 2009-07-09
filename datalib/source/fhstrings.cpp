#include "stdafx.h"
#include "enums.h"
#include "GameData.h"

namespace FHUI
{

String^ FHStrings::SpRelToString(SPRelType rel)
{
    switch( rel )
    {
    case SP_PLAYER:     return "*Player*";
    case SP_NEUTRAL:    return "Neutral";
    case SP_ALLY:       return "Ally";
    case SP_ENEMY:      return "Enemy";
    case SP_PIRATE:     return "Pirate";
    default:
        {
            int r = rel;
            throw gcnew FHUIDataIntegrityException(String::Format("Invalid species relation: {0}", r));
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

String^ FHStrings::GasToString(GasType gas)
{
    if( gas >= GAS_MAX )
        return "???";
    for( int i = 0; i < GAS_MAX; ++i )
        if( s_GasStrings[i].type == gas )
            return gcnew String(s_GasStrings[i].str);
    throw gcnew FHUIDataIntegrityException("Invalid atmospheric GAS.");
}

GasType FHStrings::GasFromString(String^ gas)
{
    for( int i = 0; i < GAS_MAX; ++i )
        if( String::Compare(
            (gcnew String(s_GasStrings[i].str))->ToLower(),
            gas->ToLower()) == 0 )
        {
            return s_GasStrings[i].type;
        }
    throw gcnew FHUIParsingException(
        String::Format("Invalid atmospheric GAS: {0}", gas) );
}

String^ FHStrings::PlTypeToString(PlanetType plType)
{
    switch( plType )
    {
    case PLANET_HOME:           return "HOME";
    case PLANET_COLONY:         return "Colony";
    case PLANET_COLONY_MINING:  return "Mining";
    case PLANET_COLONY_RESORT:  return "Resort";
    case PLANET_UNCOLONIZED:    return "Empty";
    default:
        {
            int t = plType;
            throw gcnew FHUIDataIntegrityException(String::Format("Invalid planet type: {0}", t));
        }
    }
}

struct InvStr
{
    InventoryType     type;
    const char *str;
};

static InvStr s_InvStrings[INV_MAX] =
{
    { INV_RM, "RM" },
    { INV_CU, "CU" },
    { INV_IU, "IU" },
    { INV_AU, "AU" },
    { INV_PD, "PD" },
    { INV_SU, "SU" },
    { INV_FD, "FD" },
    { INV_FS, "FS" },
    { INV_DR, "DR" },
    { INV_FM, "FM" },
    { INV_FJ, "FJ" },
    { INV_GW, "GW" },
    { INV_GT, "GT" },
    { INV_JP, "JP" },
    { INV_TP, "TP" },
    { INV_GU1, "GU1" },
    { INV_GU2, "GU2" },
    { INV_GU3, "GU3" },
    { INV_GU4, "GU4" },
    { INV_GU5, "GU5" },
    { INV_GU6, "GU6" },
    { INV_GU7, "GU7" },
    { INV_GU8, "GU8" },
    { INV_GU9, "GU9" },
    { INV_SG1, "SG1" },
    { INV_SG2, "SG2" },
    { INV_SG3, "SG3" },
    { INV_SG4, "SG4" },
    { INV_SG5, "SG5" },
    { INV_SG6, "SG6" },
    { INV_SG7, "SG7" },
    { INV_SG8, "SG8" },
    { INV_SG9, "SG9" },
};

String^ FHStrings::InvToString(InventoryType inv)
{
    if( inv >= INV_MAX )
        return "???";
    for( int i = 0; i < INV_MAX; ++i )
        if( s_InvStrings[i].type == inv )
            return gcnew String(s_InvStrings[i].str);

    int i = inv;
    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid inventory type: {0}.", i) );
}

InventoryType FHStrings::InvFromString(String^ inv)
{
    for( int i = 0; i < INV_MAX; ++i )
        if( String::Compare(
            (gcnew String(s_InvStrings[i].str))->ToLower(),
            inv->ToLower()) == 0 )
        {
            return s_InvStrings[i].type;
        }
    throw gcnew FHUIParsingException(
        String::Format("Invalid inventory abbreviation: {0}", inv) );
}

struct ShipStr
{
    ShipType    type;
    const char *str;
};

static ShipStr s_ShipStrings[SHIP_MAX] =
{
    { SHIP_BAS, "BAS" },
    { SHIP_TR, "TR" },
    { SHIP_PB, "PB" },
    { SHIP_CT, "CT" },
    { SHIP_ES, "ES" },
    { SHIP_FF, "FF" },
    { SHIP_DD, "DD" },
    { SHIP_CL, "CL" },
    { SHIP_CS, "CS" },
    { SHIP_CA, "CA" },
    { SHIP_CC, "CC" },
    { SHIP_BC, "BC" },
    { SHIP_BS, "BS" },
    { SHIP_DN, "DN" },
    { SHIP_SD, "SD" },
    { SHIP_BM, "BM" },
    { SHIP_BW, "BW" },
    { SHIP_BR, "BR" },
};

String^ FHStrings::ShipToString(ShipType ship)
{
    if( ship >= SHIP_MAX )
        return "???";
    for( int i = 0; i < SHIP_MAX; ++i )
        if( s_ShipStrings[i].type == ship )
            return gcnew String(s_ShipStrings[i].str);

    int s = ship;
    throw gcnew FHUIDataIntegrityException(
        String::Format("Invalid ship type: {0}.", s) );
}

ShipType FHStrings::ShipFromString(String^ ship)
{
    for( int i = 0; i < SHIP_MAX; ++i )
        if( String::Compare(
            (gcnew String(s_ShipStrings[i].str))->ToLower(),
            ship->ToLower()) == 0 )
        {
            return s_ShipStrings[i].type;
        }
    throw gcnew FHUIParsingException(
        String::Format("Invalid ship abbreviation: {0}", ship) );
}

} // end namespace FHUI
