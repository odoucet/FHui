#pragma once

using namespace System;

const int TOUR_NOTES = 99999;

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
    SP_NEUTRAL,
    SP_ALLY,
    SP_ENEMY
};

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
