#pragma once

#include "enums.h"

namespace FHUI
{

ref class Alien;
ref class StarSystem;
ref class Colony;
ref class GameData;

////////////////////////////////////////////////////////////////

public interface class IGridDataSrc
{
    Alien^      GetAlienForBgColor();
    String^     GetTooltipText();

    StarSystem^ GetFilterSystem();
    StarSystem^ GetFilterLocation(int %plNum);
    Alien^      GetFilterOwner();
    int         GetFilterLSN();
    int         GetFilterNumColonies();
    SPRelType   GetFilterRelType();
    ShipType    GetFilterShipType();
};

////////////////////////////////////////////////////////////////

public interface class IGridSorter : public System::Collections::IComparer
{
    void    SetSortColumn(int index);
    void    SetGroupBySpecies(bool doGroup);
    void    SetRefSystem(StarSystem ^refSystem);

    int     AddColumn(String ^title, String ^description, Type ^type, Windows::Forms::SortOrder defaultSortOrder);
};

////////////////////////////////////////////////////////////////

public interface class IGridFilter
{
    void    Update();
    void    Update(System::Object^ sender);
    void    ResetControls(bool doUpdate);
    bool    Filter(IGridDataSrc ^item);

    void    SetRefSystem(StarSystem ^system);
    void    SetRefSystem(Colony ^colony);

    void    OnGridSetup();

    property GameData^      GameData;
    property bool           EnableUpdates;
    property StarSystem^    RefSystem;
    property IGridSorter^   Sorter;

    property int            DefaultLSN;
    property int            DefaultMishap;
    property bool           SelectRefSystemFromRefShip;

    property bool           MiMaBalanced { bool get(); }
};

////////////////////////////////////////////////////////////////

} // end namespace FHUI
