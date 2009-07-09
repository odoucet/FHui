#pragma once

#include "enums.h"

using namespace System;

namespace FHUI
{

// ---------------------------------------------------

public ref class FHStrings
{
public:
    static String^          SpRelToString(SPRelType);

    static String^          GasToString(GasType gas);
    static GasType          GasFromString(String^ gas);

    static String^          PlTypeToString(PlanetType);

    static String^          InvToString(InventoryType inv);
    static InventoryType    InvFromString(String^ inv);

    static String^          ShipToString(ShipType ship);
    static ShipType         ShipFromString(String^ ship);
};

// ---------------------------------------------------

} // end namespace FHUI
