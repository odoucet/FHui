#pragma once
#include "IGridData.h"

namespace FHUI
{

public ref class GridDataSrcBase : public IGridDataSrc
{
public:
    virtual Alien^      GetAlienForBgColor()        { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual String^     GetTooltipText()            { return "<TODO...>"; }

    virtual StarSystem^ GetFilterSystem()           { return nullptr; }
    virtual StarSystem^ GetFilterLocation(int %pl)  { return nullptr; }
    virtual Alien^      GetFilterOwner()            { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual int         GetFilterLSN()              { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual int         GetFilterNumColonies()      { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual SPRelType   GetFilterRelType()          { throw gcnew FHUIDataImplException("Not implemented!"); }
    virtual ShipType    GetFilterShipType()         { throw gcnew FHUIDataImplException("Not implemented!"); }
};

} // end namespace FHUI
