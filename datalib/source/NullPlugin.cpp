#include "StdAfx.h"
#include "IFHUIPlugin.h"

using namespace System;
using namespace System::Data;

namespace FHUI
{

public ref class NullPluginBase : public IPluginBase
{
public:
    virtual void        SetGameData(GameData^ gd) { m_GameData = gd; }

protected:
    GameData^   m_GameData;
};

public ref class NullGridPlugin : public IGridPlugin, public NullPluginBase
{
public:
    virtual void        AddColumns(GridType, DataTable^) {}
    virtual void        GridFormat(GridType, DataGridView^) {}
    virtual void        AddRowData(DataRow^, StarSystem^, IGridFilter^) {}
    virtual void        AddRowData(DataRow^, Planet^, IGridFilter^) {}
    virtual void        AddRowData(DataRow^, Colony^, IGridFilter^) {}
    virtual void        AddRowData(DataRow^, Ship^, IGridFilter^) {}
    virtual void        AddRowData(DataRow^, Alien^, IGridFilter^) {}
};

} // end namespace FHUIPlugin
