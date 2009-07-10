#include "StdAfx.h"
#include "IFHUIPlugin.h"

using namespace System;
using namespace System::Data;

namespace FHUI
{

public ref class NullGridPlugin : public IGridPlugin
{
public:
    virtual void        AddColumnsSystems(DataTable^) {}
    virtual void        AddRowDataSystems(DataRow^, StarSystem^, IGridFilter^) {}
    virtual void        GridFormatSystems(DataGridView^) {}

    virtual void        AddColumnsPlanets(DataTable^) {}
    virtual void        AddRowDataPlanets(DataRow^, Planet^, IGridFilter^) {}
    virtual void        GridFormatPlanets(DataGridView^) {}

    virtual void        AddColumnsColonies(DataTable^) {}
    virtual void        AddRowDataColonies(DataRow^, Colony^, IGridFilter^) {}
    virtual void        GridFormatColonies(DataGridView^) {}

    virtual void        AddColumnsShips(DataTable^) {}
    virtual void        AddRowDataShips(DataRow^, Ship^, IGridFilter^) {}
    virtual void        GridFormatShips(DataGridView^) {}

    virtual void        AddColumnsAliens(DataTable^) {}
    virtual void        AddRowDataAliens(DataRow^, Alien^, IGridFilter^) {}
    virtual void        GridFormatAliens(DataGridView^) {}
};

} // end namespace FHUIPlugin
