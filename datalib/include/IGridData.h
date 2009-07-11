#pragma once

#include "enums.h"

namespace FHUI
{

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
    void    Update();
    void    Update(System::Object^ sender);
    void    Reset();
    bool    Filter(IGridDataSrc ^item);

    void    SetRefSystem(StarSystem ^system);

    property GameData^      GameData;
    property bool           EnableUpdates;
    property StarSystem^    RefSystem;

    property int            DefaultLSN;
    property int            DefaultMishap;
    property bool           SelectRefSystemFromRefShip;

    property bool           MiMaBalanced { bool get(); }
};

} // end namespace FHUI