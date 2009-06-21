#pragma once

using namespace System;

const int TURN_NOTES = 99999;

enum TechType
{
    TECH_MI,
    TECH_MA,
    TECH_ML,
    TECH_GV,
    TECH_LS,
    TECH_BI,
    TECH_MAX
};

enum SPRelType
{
    SP_PLAYER,
    SP_NEUTRAL,
    SP_ALLY,
    SP_ENEMY
};

String^ SpRelToString(SPRelType);

enum GasType
{
    GAS_O2,
    GAS_CH4,
    GAS_H2,
    GAS_HE,
    GAS_NH3,
    GAS_N2,
    GAS_CL2,
    GAS_H2O,
    GAS_CO2,
    GAS_HCL,
    GAS_F2,
    GAS_SO2,
    GAS_H2S,
    GAS_MAX
};

String^ GasToString(GasType gas);
GasType GasFromString(String^ gas);

enum PlanetType
{
    PLANET_HOME,
    PLANET_COLONY,
    PLANET_COLONY_MINING,
    PLANET_COLONY_RESORT,
    PLANET_MAX
};

String^ PlTypeToString(PlanetType);

enum InventoryType
{
    INV_RM,
    INV_CU,
    INV_IU,
    INV_AU,
    INV_PD,
    INV_SU,
    INV_FD,
    INV_FS,
    INV_DR,
    INV_FM,
    INV_FJ,
    INV_GW,
    INV_GT,
    INV_JP,
    INV_TP,
    INV_GU1,
    INV_GU2,
    INV_GU3,
    INV_GU4,
    INV_GU5,
    INV_GU6,
    INV_GU7,
    INV_GU8,
    INV_GU9,
    INV_SG1,
    INV_SG2,
    INV_SG3,
    INV_SG4,
    INV_SG5,
    INV_SG6,
    INV_SG7,
    INV_SG8,
    INV_SG9,
    INV_MAX
};

String^ InvToString(InventoryType inv);
InventoryType InvFromString(String^ inv);

enum ShipType
{
    SHIP_BAS,
    SHIP_TR,
    SHIP_PB,
    SHIP_CT,
    SHIP_ES,
    SHIP_FF,
    SHIP_DD,
    SHIP_CL,
    SHIP_CS,
    SHIP_CA,
    SHIP_CC,
    SHIP_BC,
    SHIP_BS,
    SHIP_DN,
    SHIP_SD,
    SHIP_BM,
    SHIP_BW,
    SHIP_BR,
    SHIP_MAX
};

enum ShipLocType
{
    SHIP_LOC_DEEP_SPACE,
    SHIP_LOC_ORBIT,
    SHIP_LOC_LANDED,
    SHIP_LOC_MAX
};

String^ ShipToString(ShipType ship);
ShipType ShipFromString(String^ ship);
