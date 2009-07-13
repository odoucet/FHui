#pragma once

using namespace System;
using namespace System::Data;
using namespace System::Windows::Forms;

namespace FHUI
{

ref class StarSystem;
ref class Planet;
ref class Colony;
ref class Alien;
ref class Ship;
ref class GameData;
interface class IGridFilter;

// ---------------------------------------------------------
//
// Base class for all plugins
//
public interface class IPluginBase
{
public:
    void        SetGameData(GameData^);
};

// ---------------------------------------------------------

public enum class GridType
{
    Systems,
    Planets,
    Colonies,
    Ships,
    Aliens,
};

// ---------------------------------------------------------
// Grid Plugin - add new colums to data grids
public interface class IGridPlugin : public IPluginBase
{
public:
    void        AddColumns(GridType, DataTable^);
    void        GridFormat(GridType, DataGridView^);

    void        AddRowData(DataRow^, StarSystem^, IGridFilter^);
    void        AddRowData(DataRow^, Planet^, IGridFilter^);
    void        AddRowData(DataRow^, Colony^, IGridFilter^);
    void        AddRowData(DataRow^, Ship^, IGridFilter^);
    void        AddRowData(DataRow^, Alien^, IGridFilter^);
};

// ---------------------------------------------------------
// Orders plugin
public interface class IOrdersPlugin : public IPluginBase
{
public:
    String^     GenerateCombat();
    String^     GeneratePreDeparture();
    String^     GenerateJumps();
    String^     GenerateProduction(Colony^);
    String^     GeneratePostArrival();
    String^     GenerateStrikes();
};

// ---------------------------------------------------------

} // end namespace FHUIPlugin
