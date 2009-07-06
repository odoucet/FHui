#pragma once

#include "enums.h"

ref class Alien;
ref class StarSystem;
ref class GameData;

public interface class IGridDataSrc
{
    Alien^      GetAlienForBgColor();
    String^     GetTooltipText();

    StarSystem^ GetFilterSystem();
    Alien^      GetFilterOwner();
    int         GetFilterLSN();
    int         GetFilterNumColonies();
    SPRelType   GetFilterRelType();
    ShipType    GetFilterShipType();
};

public interface class IGridFilter
{
    void    Update(System::Object^  sender);
    void    Reset();
    bool    Filter(IGridDataSrc ^item);

    property GameData^      GameData;
    property StarSystem^    RefSystem;

    property int            DefaultLSN;
    property int            DefaultMishap;
    property bool           SelectRefSystemFromRefShip;

    property bool           MiMaBalanced { bool get(); }
};
