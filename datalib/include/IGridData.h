#pragma once

#include "enums.h"

namespace FHUI
{

ref class Alien;
ref class StarSystem;
ref class Colony;

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
    property int            SortColumn;
    property System::Windows::Forms::SortOrder SortColumnOrder;
    property bool           GroupBySpecies;
    property StarSystem^    RefSystem;

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

    void    OnGridFill();
    void    OnGridSelectionChanged();

    property bool           EnableUpdates;
    property StarSystem^    RefSystem;
    property StarSystem^    RefSystemPrev;
    property IGridSorter^   Sorter;

    property int            DefaultLSN;
    property int            DefaultMishap;
    property bool           SelectRefSystemFromRefShip;

    property bool           MiMaBalanced { bool get(); }
};

////////////////////////////////////////////////////////////////

} // end namespace FHUI
